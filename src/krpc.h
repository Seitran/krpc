/* This header file is used to export function provided by this
 * module, mainly include rpc request runtine and rpc response 
 * handler register, and so on. See below for detail.
 * Welcome questions or suggestions.
 */
#ifndef __KRPC_H__
#define __KRPC_H__

enum krpc_operation_type {
	KRPC_OPTYPE_MEMREAD = 0,
	KRPC_OPTYPE_MEMWRITE,
	KRPC_OPTYPE_MAX,
};

#define KRPC_MAX_STRING_LEN (20)

typedef int (*msg_handler)(u8*, int);

int krpc_msg_handler_register(int optype, msg_handler handler);

#endif


