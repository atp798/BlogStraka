#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#define DEBUG_PRINT  //for TRACE

#define USE_OPTIMIZE_PRINTF

//#define FOTA

#define CONF_HOLDER			0x2A15

#define INIT_AP_PASSWD		"esp8266ex"

#define WEB_SERV_PORT		80
#define TCP_SERV_PORT		5050

#define STA_TIMER_INTERVAL	1000

#define REPORT_PREFIX		0xA1E5

#define COMM_PREFIX			0x417A

#define MAX_DNS_RETRY		10

#define WEBSERV_TASK_PRIO	USER_TASK_PRIO_0  	//0 is the lowest priority
#define TCPCOMM_TASK_PRIO	USER_TASK_PRIO_1	//priority 2 is reserved for mqtt client

#endif

