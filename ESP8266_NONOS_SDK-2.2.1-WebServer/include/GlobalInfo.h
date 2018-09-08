#ifndef __GLOBAL_VAR_H__
#define __GLOBAL_VAR_H__

#ifdef DEBUG_PRINT
#define TRACE(format,...) os_printf(format, ##__VA_ARGS__)
#else
#define TRACE(format,...)
#endif

#define WSIG_START_SERV		0
#define WSIG_DISCONN 		1
#define WSIG_WIFI_CHANGE	2
#define WSIG_REMOTE_SERVCHG	3

#define TSIG_START_SERV		0
#define TSIG_REPORT			1
#define TSIG_DISCONN		2
#define TSIG_WIFI_CHANGE	3
#define TSIG_REMOTE_SERVCHG	4
#define TSIG_DNS_FAILED		5

typedef void (*WifiStaGotIPCallBack)(void);

#endif
