/*************************************************************************
 * Name       	: operadata.c
 * Data       	: 2020-8-9
 * Description	: Database related operation fun.
 * Author     	: WangXueDong
 * Version		: v1.0
 * Modification	: None
 *************************************************************************
 */

#include "structure.h"

char buf[512] = {0};

/*************************************************************************
 * Name		    : mutex_front
 * Description	: Lock file
 * Inputs		: 
 * Outputs		: success: 0 	error: other
 * Author		: WangXueDong
 * Other		: None
 **************************************************************************
 */
static int mutex_front(EMP_TRANS_TYPES* info, sqlite3* db)
{
	char* errmsg;
	char** resultp;
	int nrow, ncloumn;

	bzero(buf, sizeof(buf));

	sprintf(buf, "select * from user where name = '%s' and no = 1;", info->user.usrname);
	sqlite3_get_table(db, buf, &resultp, &nrow, &ncloumn, &errmsg);

	/* conflict */
	if (nrow == 1){
		strcpy(info->msg, "The current userID already occupied!\n");
		info->res = 0;
		return -1;
	}

	/* Do not conflict */
	sprintf(buf, "update user set no = 1 where name = '%s' ", info->user.usrname);
	sqlite3_exec(db, buf, NULL, NULL, &errmsg);	

	return 0;
}

/*************************************************************************
 * Name		    : mutex_after
 * Description	: unLock file
 * Inputs		:
 * Outputs		: success: 0 	error: other
 * Author		: WangXueDong
 * Other		: None
 **************************************************************************
 */
static int mutex_after(EMP_TRANS_TYPES* info, sqlite3* db)
{
	char* errmsg;

	bzero(buf, sizeof(buf));
	sprintf(buf, "update user set no = 0 where name = '%s' ", info->user.usrname);
	sqlite3_exec(db, buf, NULL, NULL, &errmsg);

	return 0;
}

/*************************************************************************
 * Name		    : user_login
 * Description	: user login. input userID/password.
 * Inputs		:
 * Outputs		: success: 0 	error: other
 * Author		: WangXueDong
 * Other		: None
 **************************************************************************
 */
int user_login(int fd, EMP_TRANS_TYPES* info, sqlite3* db)
{
	int ret = 0;
	int row, col;
	char* errmsg;
	char** resultp;

	bzero(buf, sizeof(buf));

	/* dete passwprd */
	sprintf(buf, "select * from user where name = '%s' and passwd = '%s';", info->user.usrname, info->user.usrpwd);
	ret = sqlite3_get_table(db, buf, &resultp, &row, &col, &errmsg);
	if (SQLITE_OK != ret){
		printf("%s\n", errmsg);
		return ret;
	}

	/* already in the table */
	if (row){
		strcpy(info->msg, "Now userID login successfully\n");
		info->res = 1;

		/* dete user type */
		if (strcmp("0", resultp[col]) == 0) {
			info->user.type = 0;		/* root */
		}
		else {
			info->user.type = 1;		/* user */
		}
	}
	else {	/* no in the table */

		strcpy(info->msg, "Not found now input userID, Please try again...\n");
		info->res = 0;
	}

	/* send to client */
	if (send(fd, info, sizeof(EMP_TRANS_TYPES), 0) < 0){
		perror("send: ");
	}

	return 0;
}

/*************************************************************************
 * Name		    : user_add
 * Description	: add userID passwprd info ...
 * Inputs		:
 * Outputs		: success: 0 	error: other
 * Author		: WangXueDong
 * Other		: None
 **************************************************************************
 */
int user_add(int fd, EMP_TRANS_TYPES* info, sqlite3* db)
{
	int ret = 0;
	char* errmsg;

	bzero(buf, sizeof(buf));

	/* add user login info */
	sprintf(buf, "insert into user values('%d','%s','%s',0);"
		, info->user.type, info->user.usrname, info->user.usrpwd);

	printf("%s\n", buf);

	ret = sqlite3_exec(db, buf, NULL, NULL, &errmsg);
	if (SQLITE_OK != ret){
		strcpy(info->msg, "The current user already exists\n");
		info->res = 0;
		ret = -1;
		goto ERROR_PART;
	}

	/* add employee info */
	sprintf(buf, "insert into info values('%s','%s','%d','%d','%d','%f','%s','%d');",
		info->data.name, info->data.addree, info->data.age, info->data.level, info->data.no, 
		info->data.salary, info->data.phone, info->data.position);
	
	ret = sqlite3_exec(db, buf, NULL, NULL, &errmsg);
	if (SQLITE_OK != ret){
		strcpy(info->msg, "Server add userID/Password successfully...\nBut add user info faild!\n");
		info->res = 0;
		ret = -2;
		goto ERROR_PART;
	}

	/* info */
	printf("name:%s\taddree:%s\tage:%d\tlevel:%d\tno:%d\tsalary:%f\tphone:%s\tposition:%d\n",
       	info->data.name, info->data.addree, info->data.age, info->data.level, info->data.no,
         info->data.salary, info->data.phone, info->data.position);

	/* add userID and Info successfully */
	strcpy(info->msg, "Server add userID and Info successfully!\n");
	info->res = 1;
	ret = 0;

ERROR_PART:

	/* send to client */
	if (send(fd, info, sizeof(EMP_TRANS_TYPES), 0) < 0){
		perror("send: ");
	}

	return ret;
}

/*************************************************************************
 * Name		    : user_modify_pwd
 * Description	: modify passwprd ...
 * Inputs		:
 * Outputs		: success: 0 	error: other
 * Author		: WangXueDong
 * Other		: None
 **************************************************************************
 */
int user_modify_pwd(int fd, EMP_TRANS_TYPES* info, sqlite3* db)
{
	int ret = 0;
	char* errmsg;
	int k, nrow, ncloumn;
	char** resultp;

	bzero(buf, sizeof(buf));

	/* detect lock is return */
	if (mutex_front(info, db) == -1){
		info->res = 0;
		if (send(fd, info, sizeof(EMP_TRANS_TYPES), 0) < 0){
			perror("send: ");
		}
		return 0;
	}

	/* modify password */
	sprintf(buf, "update user set passwd = %s where name = '%s' ",
		info->user.usrpwd, info->user.usrname);
	ret = sqlite3_exec(db, buf, NULL, NULL, &errmsg);
	if (SQLITE_OK != sqlite3_exec(db, buf, NULL, NULL, &errmsg)){
		strcpy(info->msg, "Sorry, modify password Faild!\n");
		info->res = 0;   
		goto ERROR_PART;
	}

	strcpy(info->msg, "OK, modify password successfully\n");
	info->res = 1;

ERROR_PART:

	/* send to client */
	if (send(fd, info, sizeof(EMP_TRANS_TYPES), 0) < 0) {
		perror("send: ");
	}

	/* unlock */
	mutex_after(info, db);

	return 0;
}

/*************************************************************************
 * Name		    : user_find
 * Description	: find userID passwprd (root)...
 * Inputs		:
 * Outputs		: success: 0 	error: other
 * Author		: WangXueDong
 * Other		: None
 **************************************************************************
 */
int user_find(int fd, EMP_TRANS_TYPES* info, sqlite3* db)
{
	int ret = 0;
	char* errmsg;
	char** resultp;
	int nrow, ncloum;
	int i = 0, j = 0, index = 0;

	bzero(buf, sizeof(buf));

	/* find userID name */
	sprintf(buf, "select *from user where name = '%s' ", info->user.usrname);
	ret = sqlite3_get_table(db, buf, &resultp, &nrow, &ncloum, &errmsg);
	if (SQLITE_OK != ret){
		printf("%s\n", errmsg);
		info->res = 0;
		goto ERROR_PART;
	}

	/* cat str info */
	for (index = ncloum, j = 0; j < ncloum; j++){
		strcat(info->msg, resultp[index++]);
		strcat(info->msg, " - ");
	}

	info->res = 1;

ERROR_PART:

	/* send to client */
	if (send(fd, info, sizeof(EMP_TRANS_TYPES), 0) < 0) {
		perror("send: ");
	}

	return 0;
}

/*************************************************************************
 * Name		    : user_delete
 * Description	: delete user (root)...
 * Inputs		:
 * Outputs		: success: 0 	error: other
 * Author		: WangXueDong
 * Other		: None
 **************************************************************************
 */
int user_delete(int fd, EMP_TRANS_TYPES* info, sqlite3* db)
{
	int ret = 0;
	int nrow, ncloumn;
	char* errmsg;
	char** resultp;

	bzero(buf, sizeof(buf));

	/* detect lock is return */
	if (mutex_front(info, db) < 0) {
		
		if (send(fd, info, sizeof(EMP_TRANS_TYPES), 0) < 0) {
			perror("send: ");
		}
		return 0;
	}

	/* del user password */
	sprintf(buf, "delete from user where name = '%s' ", info->user.usrname);
	ret = sqlite3_exec(db, buf, NULL, NULL, &errmsg);
	if (SQLITE_OK != ret){
		strcpy(info->msg, "Delete pwd Faild!\n");
		info->res = 0;
		goto ERROR_PART;
	}

	/* del user info */
	sprintf(buf, "delete from info where name = '%s' ", info->user.usrname);
	ret = sqlite3_exec(db, buf, NULL, NULL, &errmsg);
	if (SQLITE_OK != ret){
		strcpy(info->msg, "Delete userID/Password successfully! but Delete user info Faild!\n");
		info->res = 0;
		goto ERROR_PART;
	}

	strcpy(info->msg, "Delete user userID/Password and info successfully!\n");
	info->res = 1;

ERROR_PART:

	/* send to client */
	if (send(fd, info, sizeof(EMP_TRANS_TYPES), 0) < 0) {
		perror("send: ");
	}

	/* unlock */
	mutex_after(info, db);

	return 0;
}

/*************************************************************************
 * Name		    : user_modify_info
 * Description	: modify data info ...
 * Inputs		:
 * Outputs		: success: 0 	error: other
 * Author		: WangXueDong
 * Other		: None
 **************************************************************************
 */
int user_modify_info(int fd, EMP_TRANS_TYPES* info, sqlite3* db)
{
	int ret = 0;
	char* errmsg;
	char** resultp;
	int nrow, ncloumn;

	/* detect lock is return */
	if (mutex_front(info, db) == -1) {
		info->res = 0;

		if (send(fd, info, sizeof(EMP_TRANS_TYPES), 0) < 0) {
			perror("send: ");
		}
		return 0;
	}

	/* root modify salary */
	if (info->user.type == 0){
		sprintf(buf, "update info set salary = %lf where name = '%s' ", info->data.salary, info->data.name);
		ret = sqlite3_exec(db, buf, NULL, NULL, &errmsg);
		if (SQLITE_OK != ret){
			strcpy(info->msg, "Sorry, modify salary faild!\n");
			info->res = 0; 
			goto ERROR_PART;
		}
	}

	/* all modify adree and name */
	sprintf(buf, "update  set addr = '%s' where name = '%s' ", info->data.addree, info->data.name);
	ret = sqlite3_exec(db, buf, NULL, NULL, &errmsg);
	if (SQLITE_OK != ret){
		strcpy(info->msg, "Sorry, modify salary successfully! but modify other info faild!\n");
		info->res = 0; 
		goto ERROR_PART;
	}

	strcpy(info->msg, "Ok, modify all info successfully!\n");
	info->res = 1;

ERROR_PART:

	/* send to client */
	if (send(fd, info, sizeof(EMP_TRANS_TYPES), 0) < 0) {
		perror("send: ");
	}

	/* unlock */
	mutex_after(info, db);

	return 0;
}
