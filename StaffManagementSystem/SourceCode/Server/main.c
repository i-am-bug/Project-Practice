/*************************************************************************
 * Name       	: main.c
 * Data       	: 2020-8-8
 * Description	: Server-Terminal main fun
 * Author     	: WangXueDong
 * Version		: v1.0
 * Modification	: None
 *************************************************************************
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "defcmd.h"
#include "structure.h"

#if 1
#define PRINTF_MAIN printf
#else
#define PRINTF_MAIN
#endif

#define SEVER_LISTEN_MAX 		(4UL) 	/* sever Listen client maxnum */

 /* database memory path */
#define PATH_USER_INFO		"./data/user_info.db"


 /*************************************************************************
  * Name		: my_user_client
  * Description	: proess client cmd
  * Inputs		: fd: File descriptor  db:File descriptor
  * Outputs		: success: 0 	error: other
  * Author		: WangXueDong
  * Other		: None
  **************************************************************************
  */
int my_user_client(int fd, sqlite3 *db)
{
	EMP_TRANS_TYPES lpv_cmd; 	/* cmd struct val */

	while (1){

		/* recvice client data: Blocking reception */
		bzero(&lpv_cmd, sizeof(EMP_TRANS_TYPES));

		if (recv(fd, &lpv_cmd, sizeof(EMP_TRANS_TYPES), 0) < 0){
			printf("client quit\n");
			goto EXIT_PART0;
		}
		/* operat client cmd */
		switch (lpv_cmd.cmd){
			case CMD_LOGIN:
				PRINTF_MAIN("now Client login!\n");
				user_login(fd, &lpv_cmd, db);
				break;

			case CMD_ADD:
				PRINTF_MAIN("now Client add user!\n");
				user_add(fd, &lpv_cmd, db);
				break;

			case CMD_MODIFY_PWD:
				user_modify_pwd(fd, &lpv_cmd, db);
				break;

			case CMD_FIND:
				user_find(fd, &lpv_cmd, db);
				break;

			case CMD_DELETE:
				user_delete(fd, &lpv_cmd, db);
				break;

			case CMD_MODIFY_INFO:
				user_modify_info(fd, &lpv_cmd, db);
				break;
		}
	}

EXIT_PART0:
	close(fd);
	return 0;
}

 /*************************************************************************
  * Name		: main
  * Description	: Server-Terminal Entry Function
  * Inputs		: argc: input param num
  * 				  argv[1]: Server IP
  * 				  argv[2]: port
  * Outputs		: success: 0 	error: other
  * Author		: WangXueDong
  * Other		: None
  **************************************************************************
  */
int main(int argc, const char* argv[])
{
	int ret = 0;
	int liv_port = 0;
	sqlite3 *db;
	char* errmsg;
	pid_t pid;
	int liv_listen_fd = 0, liv_client_fd = 0;
	struct sockaddr_in liv_saddr, liv_caddr;
	const int lic_optval = 1;
	socklen_t lsv_caddr = sizeof(liv_caddr);

	/* input param detect */
	if (argc != 3) {
		PRINTF_MAIN("%s.c-%d£ºInputs param err! eg: run 1024 192.168.1.22\n", \
			__func__, __LINE__);
		ret = -1;
		goto ERROR_PART0;
	}

	/* port string convert to int type, and detect port range */
	liv_port = atoi(argv[2]);
	if ((liv_port < 0x400) || (liv_port > 0xBFFF)) {
		PRINTF_MAIN("%s.c-%d£ºInputs port err, please input port range [1024 - 49151]\n", \
			__func__, __LINE__);
		ret = -2;
		goto ERROR_PART0;
	}

	/* open database */
	ret = sqlite3_open(PATH_USER_INFO, &db);
	if (SQLITE_OK != SQLITE_OK){
		printf("Error opening the database. \nCause of error: %s\n",  sqlite3_errmsg(db));
		ret = -3;
		goto ERROR_PART0;
	}

	/* Create user/password table and information table // add defult root/pwd(1) */
	sqlite3_exec(db, "create table user(type Integer, name text primary key,passwd text, no Integer);", NULL, NULL, &errmsg);
	sqlite3_exec(db, "create table info(name text primary key, addr char, age char, level char, no char,salary float, phone char,type char);", NULL, NULL, &errmsg);
	sqlite3_exec(db, "insert into user values(0, 'root', '1234', 0);", NULL, NULL, &errmsg);

	/* Create socket interface */
	liv_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (liv_listen_fd < 0) {
		perror("socket: ");
		ret = -4;
		goto ERROR_PART1;
	}

	/* set server port and ip */
	bzero(&liv_saddr, sizeof(liv_saddr));
	liv_saddr.sin_family = AF_INET;
	liv_saddr.sin_port = htons(liv_port);
	if (inet_pton(AF_INET, argv[1], (void*)&liv_saddr.sin_addr.s_addr) < 0) {
		perror("inet_pton: ");
		ret = -5;
		goto ERROR_PART2;
	}

	/* assigning a name to a socket */
	if (bind(liv_listen_fd, (void*)&liv_saddr, sizeof(liv_saddr))) {
		perror("bind: ");
		ret = -6;
		goto ERROR_PART2;
	}

	/* Allow local address and port reuse */
	if (setsockopt(liv_listen_fd, SOL_SOCKET, SO_REUSEADDR, \
		(void*)&lic_optval, sizeof(int)) < 0) {
		perror("setsockopt: ");
		ret = -7;
		goto ERROR_PART2;
	}

	/* Listen Client */
	if (listen(liv_listen_fd, SEVER_LISTEN_MAX) < 0) {
		perror("Listen: ");
		ret = -8;
		goto ERROR_PART2;
	}

	/* del zombie process */
	signal(SIGCHLD, SIG_IGN);

	/* Set up a loop to receive client link requests */
	while (1) {

		/* accept clientv request */
		liv_client_fd = accept(liv_listen_fd, NULL, NULL);
		if (liv_client_fd < 0) {
			perror("accept: ");
			ret = -9;
			goto ERROR_PART2;
		}

		/* create child fork */
		pid = fork();
		if (pid < 0) {					/* create fork error */
			perror("fork: ");
			ret = -10;
			goto ERROR_PART2;
		}
		else if(pid == 0){				/* child fork operat */
			close(liv_listen_fd);

			/* deal with */
			my_user_client(liv_client_fd, db);
		}
		else {							/* wait recv client */
			close(liv_client_fd);
		}
	}

ERROR_PART2:
	close(liv_listen_fd);

ERROR_PART1:
	sqlite3_close(db);

ERROR_PART0:
	return ret;
}
