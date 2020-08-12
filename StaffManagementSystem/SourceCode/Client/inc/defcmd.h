/*************************************************************************
 * Name       	: defcmd.h
 * Data       	: 2020/08/09
 * Description	: All CMD of client and server.
 * Author     	: WangXueDong
 * Version		: V0.0.0.1
 * Modification	:
 *************************************************************************
 */
#ifndef _H_DEFCMD_H_
#define _H_DEFCMD_H_

/* request cmd */
#define CMD_TEST			(0xA5A5A5A5UL)		/* client and server ping test */
#define CMD_LOGIN			(0xFA000001UL)		/* client requests login  */
#define CMD_ADD				(0xFA000002UL)		/* client requests add    */
#define CMD_MODIFY_PWD		(0xFA000003UL)		/* modify user pwd */
#define CMD_FIND			(0xFA000004UL)		/* client requests find */
#define CMD_DELETE			(0xFA000005UL)		/* client requests del  */
#define CMD_MODIFY_INFO		(0xFA000006UL)		/* modify user info */
/* ... ... */

#endif	