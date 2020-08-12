/*************************************************************************
 * Name       	: main.c
 * Data       	: 2020-8-8
 * Description	: Client-Terminal main fun
 * Author     	: ZhangXueSong
 * Version		: v1.0
 * Modification	: None
 *************************************************************************
 */

#include "structure.h"


/* user types */
int user_type = 1;

 /*************************************************************************
  * Name		: main
  * Description	: Client-Terminal Entry Function
  * Inputs		: argc: input param num
  * 				  argv[1]: Server IP
  * 				  argv[2]: port
  * Outputs		: success: 0 	error: other
  * Author		: ZhangXueSong
  * Other		: None
  **************************************************************************
  */
int main(int argc, const char* argv[])
{
	int liv_meunRev = 0;
	int liv_again   = 0;
	char lcv_inputStr[20] = {0};
	int ret = 0,  liv_author = 0;
	int liv_port = 0, liv_fd = 0;
	struct sockaddr_in liv_addr;
	EMP_TRANS_TYPES lpv_info;

	/* input param detect */
	if (argc != 3) {
		printf("%s.c-%d£ºInputs param err! eg: run 1024 192.168.1.22\n", \
			__func__, __LINE__);
		ret = -1;
		goto ERROR_PART0;
	}

	/* port string convert to int type, and detect port range */
	liv_port = atoi(argv[2]);
	if ((liv_port < 0x400) || (liv_port > 0xBFFF)) {
		printf("%s.c-%d£ºInputs port err, please input port range [1024 - 49151]\n", \
			__func__, __LINE__);
		ret = -2;
		goto ERROR_PART0;
	}

	/* Create socket interface */
	liv_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (liv_fd < 0) {
		perror("socket: ");
		ret = -3;
		goto ERROR_PART0;
	}

	/* set server port and ip */
	bzero(&liv_addr, sizeof(struct sockaddr_in));
	liv_addr.sin_family = AF_INET;
	liv_addr.sin_port = htons(liv_port);
	if (inet_pton(AF_INET, argv[1], (void*)&liv_addr.sin_addr.s_addr) < 0) {
		perror("inet_pton: ");
		ret = -4;
		goto ERROR_PART1;
	}

	/* connect client */
	if (connect(liv_fd, (void *)&liv_addr, sizeof(liv_addr)) < 0){
		perror("Listen: ");
		ret = -5;
		goto ERROR_PART1;
	}

	/* login input userID and pwd */
	do{
		liv_author = user_login(liv_fd, &lpv_info);
		
	}while(liv_author < 0);

	/* prompt for login user */
	if (liv_author) {
		printf("Currently login permissions: user\n");
		user_type = 1;
	}
	else {
		user_type = 0;
		printf("Currently login permissions: root \n");
	}

	/* Set up a loop to receive client link requests */
	while (1) {

		/* menu */
		printf("--------------------------------------------------------------------\n");
		printf(" (1). ADD: add UserID and PassWord, and employees basic information.\n");
		printf(" (2). DEL: (root) according userID delete userID/PassWord and info. \n");
		printf(" (3). MDP: modify your's password, but not support your's userID.   \n");
		printf(" (4). MDI: modify your's data info, but only root can modify salary.\n");
		printf(" (5). FND: (root) according userID find userID and PassWord.        \n");
		printf(" (0). Exit Client.\n");
		printf(" PROMPT: Please inputs num, and  press down Enter.\n");
		printf("---------------------------------------------------------------------\n");
		printf("Please input num (0-5) > ");
		scanf("%d", &liv_meunRev);
		getchar();

		/* Interactive interface */
		switch(liv_meunRev){
			case 1:
				do{
					liv_again = 0; 								/* continuous ? */
					user_add(liv_fd, &lpv_info, user_type); 	/* add userID ... */

					printf("You need again input?(y or n) > ");
					bzero(&lcv_inputStr, sizeof(lcv_inputStr));
					scanf("%s", lcv_inputStr);
					getchar();

					/* you nend again £¿ liv_again = 1(OK) */
					if(!strncasecmp("y", lcv_inputStr, 1)){
							liv_again = 1;	
					}
				}while(liv_again == 1);
				break;
		
			case 2:
				user_delete(liv_fd, &lpv_info, user_type);
				break;

			case 3:
				user_modify_pwd(liv_fd, &lpv_info, user_type);
				break;

			case 4:
				user_modify_info(liv_fd, &lpv_info, user_type);
				break;

			case 5:
				user_find(liv_fd, &lpv_info, user_type);
				break;

			case 0:
				printf("Look forward to seeing you next time, bye ~\n");
				goto ERROR_PART1;

			default:
				printf("Please inputs effective number!\n");
		}
	}

ERROR_PART1:
	close(liv_fd);

ERROR_PART0:
	return ret;
}
