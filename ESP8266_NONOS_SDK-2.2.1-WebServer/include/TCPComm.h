#ifndef __TCP_COMM_H__
#define __TCP_COMM_H__
#include "c_types.h"

#define TCPTASK_QUEUE_LEN	4

typedef struct ReportProtocol{
	uint8 Prefix[2];
	uint16 DataLen;
	uint32* DataBuf;
}ReportProtocol;

void TCPCommTask(os_event_t *e);
void TCPServInit(uint32 port);
bool TCPClientInit(uint32 ipAddr, uint16 port);

#endif
