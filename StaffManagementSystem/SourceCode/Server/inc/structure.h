/*************************************************************************
 * Name       	: structure.h
 * Data       	: 2020/08/09
 * Description	: Function-defined transport protocol.
 * Author     	: WangXueDong
 * Version		: V0.0.0.1
 * Modification	:
 *************************************************************************
 */
#ifndef _H_STRUCTURE_H_
#define _H_STRUCTURE_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sqlite3.h>
#include<signal.h>
#include <sys/types.h>
#include <sys/socket.h>

/* employee info */
typedef struct {
	int	 no;
	char name[100];
	char phone[100];
	char addree[100];
	char age;			
	int level;
	double salary;
	int position;
}EMP_INFO_TYPES;

/* login info */
typedef struct{
	int type;				/* login types */
	int no;
	char usrname[20];		/* log in user name */
	char usrpwd[20];		/* log in user password */
}USER_LOGIN_TYPES;

/* trans info */
typedef struct{
	int res;
	int cmd;
	char msg[256];
	USER_LOGIN_TYPES user;
	EMP_INFO_TYPES   data;
}EMP_TRANS_TYPES;

/* extern func */
int user_login(int fd, EMP_TRANS_TYPES* info, sqlite3* db);
int user_add(int fd, EMP_TRANS_TYPES* info, sqlite3* db);
int user_modify_pwd(int fd, EMP_TRANS_TYPES* info, sqlite3* db);
int user_find(int fd, EMP_TRANS_TYPES* info, sqlite3* db);
int user_delete(int fd, EMP_TRANS_TYPES* info, sqlite3* db);
int user_modify_info(int fd, EMP_TRANS_TYPES* info, sqlite3* db);

#endif
