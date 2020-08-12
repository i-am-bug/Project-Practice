/*************************************************************************
 * Name       	: operadata.c
 * Data       	: 2020-8-9
 * Description	: Database related operation fun.
 * Author     	: FanYiBao
 * Version		: v1.0
 * Modification	: None
 *************************************************************************
 */

#include "structure.h"
#include "defcmd.h"

char nameID_buf[20] = {0};
char passWord_buf[20] = {0};

/*************************************************************************
 * Name		    : user_login
 * Description	: user login. input userID/password.
 * Inputs		: 
 * Outputs		: success: 0 	error: other
 * Author		: FanYiBao
 * Other		: None
 **************************************************************************
 */
int user_login(int fd, EMP_TRANS_TYPES* info)
{
	int ret = 0;

	/* input user name and pwd */
	printf("Please Login system：\n");
	printf("nameID > ");
	scanf("%s", info->user.usrname);
	strcpy(nameID_buf, info->user.usrname);

	printf("passWord > ");
	scanf("%s", info->user.usrpwd);
	strcpy(passWord_buf, info->user.usrpwd);

	/* add msg cmd */
	info->cmd = CMD_LOGIN;	

	/* send msg to sever userID and pwd */
	ret = send(fd, info, sizeof(EMP_TRANS_TYPES), 0);
	if (ret < 0){
		printf("Failed to send to server!\n");
		return -1;
	}

	/* recver msg from sever */
	ret = recv(fd, info, sizeof(EMP_TRANS_TYPES), 0);
	if (ret < 0){
		printf("Failed to get data from server!\n");
		return -2;
	}

	printf("PROMPT: %s", info->msg);
	printf("--------------------------------------------------------------------------------\n");

	/* sever opert success or faild */
	if (0 == info->res) { return -3;}
	
	/* return 0:root 1:user */
	return info->user.type;
}

/*************************************************************************
 * Name		    : user_add
 * Description	: add userID passwprd info ...
 * Inputs		:
 * Outputs		: success: 0 	error: other
 * Author		: FanYiBao
 * Other		: None
 **************************************************************************
 */
int user_add(int fd, EMP_TRANS_TYPES* info, int types)
{
	int ret = 0;

	/* choose user or root types */
	if(types){
		info->user.type = 1; 	/* user */ 
	}
	else{
		printf("Please choose Creat root(0) or user(1) > ");
		scanf("%d", &ret);
		if(ret == 0){
			info->user.type = 0;    /* root */
		}
		else { info->user.type = 1;}
	}

	/* input user name and pwd */
	printf("Please add userID and passWord：\n");
	printf("nameID > ");
	scanf("%s", info->user.usrname);

	printf("passWord > ");
	scanf("%s", info->user.usrpwd);

	/* add Employee */
	printf("Please input user other info: \n");
	printf(".No(>1) > "); 			
	scanf("%d", &info->data.no);

	printf(".Name > ");
	scanf("%s", info->data.name);

	printf(".Age(15-65) > ");
	scanf("%hhd", &info->data.age);
	
	printf(".Phone > ");
	scanf("%s", info->data.phone);

	printf(".Addree(str) > ");
	scanf("%s", info->data.addree);

	printf(".Level(1-5) > ");
	scanf("%d", &info->data.level);

	printf(".Position(1-20) > ");
	scanf("%d", &info->data.position);

	printf(".Salary > ");
	scanf("%lf", &info->data.salary);

	/* add msg cmd */
	info->cmd = CMD_ADD;	
	
	/* send msg to sever userID and pwd */
	ret = send(fd, info, sizeof(EMP_TRANS_TYPES), 0);
	if (ret < 0){
		printf("Failed to send to server!\n");
		return -1;
	}

	/* recver msg from sever */
	ret = recv(fd, info, sizeof(EMP_TRANS_TYPES), 0);
	if (ret < 0){
		printf("Failed to get data from server!\n");
		return -2;
	}

	printf("PROMPT: %s", info->msg);
	printf("--------------------------------------------------------------------------------\n");

	/* sever opert success or faild */
	if (0 == info->res) { return -3;}
	
	/* return 0:root 1:user */
	return info->user.type;
}

/*************************************************************************
 * Name		    : user_modify_pwd
 * Description	: modify passwprd ...
 * Inputs		:
 * Outputs		: success: 0 	error: other
 * Author		: FanYiBao
 * Other		: None
 **************************************************************************
 */
int user_modify_pwd(int fd, EMP_TRANS_TYPES* info, int types)
{
	int ret = 0;
	char liv_buf[20] = {0};

	printf("Please input your's old password > ");
	scanf("%s", liv_buf);
	getchar();

	/* input password error */
	if(0 != strcmp(liv_buf, passWord_buf)){
		printf("You input old password error. Please again!\n");
		return -1;
	}

	printf("Please input your's new password > ");
	scanf("%s", info->user.usrpwd);
	getchar();

	strcpy(info->user.usrname, nameID_buf);
    
	/* add msg cmd */
    info->cmd = CMD_MODIFY_PWD;        
        
    /* send msg to sever userID and pwd */
    ret = send(fd, info, sizeof(EMP_TRANS_TYPES), 0); 
    if (ret < 0){                                                                                     
        printf("Failed to send to server!\n");
        return -1; 
    }   

    /* recver msg from sever */
    ret = recv(fd, info, sizeof(EMP_TRANS_TYPES), 0); 
    if (ret < 0){ 
        printf("Failed to get data from server!\n");
        return -2; 
    }   

    printf("PROMPT: %s", info->msg);
    printf("--------------------------------------------------------------------------------\n");

    /* sever opert success or faild */
    if (0 == info->res) { return -3;}
        
    /* return 0:root 1:user */
    return info->user.type;
}

/*************************************************************************
 * Name		    : user_find
 * Description	: find userID passwprd (root)...
 * Inputs		:
 * Outputs		: success: 0 	error: other
 * Author		: FanYiBao
 * Other		: None
 **************************************************************************
 */
int user_find(int fd, EMP_TRANS_TYPES* info, int types)
{
	int ret = 0;

    /* Printing tips */
    if(types){      /* user */
        printf("You is't  root user, No permission to Find userID Info!!\n");
        printf("Please switch to root ... !!! \n");
        return -1; 
    }                                                                                                 

    printf("Please input you want del userID > ");
    scanf("%s", info->user.usrname);
    getchar();

    /* add msg cmd */
    info->cmd = CMD_FIND;

    /* send msg to sever userID and pwd */
    ret = send(fd, info, sizeof(EMP_TRANS_TYPES), 0); 
    if (ret < 0){ 
        printf("Failed to send to server!\n");
        return -1; 
    }   

    /* recver msg from sever */
    ret = recv(fd, info, sizeof(EMP_TRANS_TYPES), 0); 
    if (ret < 0){ 
        printf("Failed to get data from server!\n");
        return -2; 
    }

    /* sever opert success or faild */
	if(info->res){
    	printf("RESULT: %s\n", info->msg);
    	printf("--------------------------------------------------------------------------------\n");
	}
	else{
    	printf("--------------------------------------------------------------------------------\n");
		return -3;
	}

    /* return 0:root 1:user */
    return info->user.type;
}

/*************************************************************************
 * Name		    : user_delete
 * Description	: delete user (root)...
 * Inputs		:
 * Outputs		: success: 0 	error: other
 * Author		: FanYiBao
 * Other		: None
 **************************************************************************
 */
int user_delete(int fd, EMP_TRANS_TYPES* info, int types)
{
	int ret = 0;

	/* Printing tips */
	if(types){ 		/* user */
		printf("You is't  root user, No permission to delete userID!!\n");
		printf("Please switch to root ... !!! \n");
		return -1;
	}

	printf("Please input you want del userID > ");
	scanf("%s", info->user.usrname);
	getchar();

	/* add msg cmd */
	info->cmd = CMD_DELETE;

	/* send msg to sever userID and pwd */
	ret = send(fd, info, sizeof(EMP_TRANS_TYPES), 0);
	if (ret < 0){
		printf("Failed to send to server!\n");
		return -1;
	}

	/* recver msg from sever */
	ret = recv(fd, info, sizeof(EMP_TRANS_TYPES), 0);
	if (ret < 0){
		printf("Failed to get data from server!\n");
		return -2;
	}

	printf("PROMPT: %s", info->msg);
	printf("--------------------------------------------------------------------------------\n");

	/* sever opert success or faild */
	if (0 == info->res) { return -3;}
	
	/* return 0:root 1:user */
	return info->user.type;
}

/*************************************************************************
 * Name		    : user_modify_info
 * Description	: modify data info ...
 * Inputs		:
 * Outputs		: success: 0 	error: other
 * Author		: FanYiBao
 * Other		: None
 **************************************************************************
 */
int user_modify_info(int fd, EMP_TRANS_TYPES* info, int types)
{
    int ret = 0;                                                                                                                    
    char liv_buf[20] = {0};

    printf("Please input your's old password > ");
    scanf("%s", liv_buf);
    getchar();

    /* input password error */
    if(0 != strcmp(liv_buf, passWord_buf)){
        printf("You input old password error. Please again!\n");
        return -1; 
    }   

    strcpy(info->user.usrname, nameID_buf);
    
	/* add Employee */
	printf("Please input user other info: \n");
	printf(".No(>1) > "); 			
	scanf("%d", &info->data.no);

	printf(".Name > ");
	scanf("%s", info->data.name);

	printf(".Age(15-65) > ");
	scanf("%hhd", &info->data.age);
	
	printf(".Phone > ");
	scanf("%s", info->data.phone);

	printf(".Addree(str) > ");
	scanf("%s", info->data.addree);

	printf(".Level(1-5) > ");
	scanf("%d", &info->data.level);

	printf(".Position(1-20) > ");
	scanf("%d", &info->data.position);

	if(0 == types){
		printf(".Salary > ");
		scanf("%lf", &info->data.salary);
	}

    /* add msg cmd */
    info->cmd = CMD_MODIFY_INFO;    
    
    /* send msg to sever userID and pwd */
    ret = send(fd, info, sizeof(EMP_TRANS_TYPES), 0); 
    if (ret < 0){    
        printf("Failed to send to server!\n");
        return -1; 
    }   

    /* recver msg from sever */
    ret = recv(fd, info, sizeof(EMP_TRANS_TYPES), 0); 
    if (ret < 0){ 
        printf("Failed to get data from server!\n");
        return -2; 
    }   

    printf("PROMPT: %s", info->msg);
    printf("--------------------------------------------------------------------------------\n");

    /* sever opert success or faild */
    if (0 == info->res) { return -3;}
    
    /* return 0:root 1:user */
    return info->user.type;
}
