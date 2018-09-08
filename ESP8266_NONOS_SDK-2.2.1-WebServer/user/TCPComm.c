#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
#include "espconn.h"
#include "upgrade.h"
#include "TCPComm.h"
#include "FlashParam.h"
#include "user_config.h"
#include "GlobalInfo.h"
#include "UserIO.h"

#ifdef SERVER_SSL_ENABLE
#include "ssl/cert.h"
#include "ssl/private_key.h"
#endif

extern struct FlashProtectParam stFlashProtParam;

os_event_t *TCPTaskQueue;
LOCAL struct espconn connServer;
LOCAL struct espconn connClient;
LOCAL esp_tcp espTcp;
bool TCPServOn = false;
bool TCPClientOn = false;

LOCAL os_timer_t tmStation;

struct espconn connDNSTmp;
esp_tcp tcpDNSTmp;
ip_addr_t ipDNS;
LOCAL os_timer_t tmDNS;
LOCAL uint8 DNSRetryCtn = 0;
LOCAL bool DNSFound = false;

extern char RelayStatus;
extern WifiStaGotIPCallBack staGotIPCB;


LOCAL void TCPResponse(void *arg, const char *pData, uint16 length);

void ICACHE_FLASH_ATTR
StationTimerCB(){
	system_os_post(TCPCOMM_TASK_PRIO, TSIG_REPORT,0x00);
}

void ICACHE_FLASH_ATTR
StationTimerInit(){
    os_timer_disarm(&tmStation);
    os_timer_setfn(&tmStation,(os_timer_func_t *)StationTimerCB,NULL); //set callback func
    os_timer_arm(&tmStation,STA_TIMER_INTERVAL,1); //set timer interval,unit:ms
}

LOCAL void ICACHE_FLASH_ATTR
DNSFoundCB(const char *name, ip_addr_t *ipAddr, void *arg)
{
    struct espconn *pEspConn = (struct espconn *)arg;
    if (ipAddr == NULL) {
    	DNSRetryCtn++;
        TRACE("domain dns not found\r\n");
        return;
    }

    DNSFound = true;
    TRACE("domain dns found "IPSTR"\n",IP2STR(ipAddr));
	TCPClientInit(ipAddr->addr, stFlashProtParam.RemotePort);
	StationTimerInit();
}

LOCAL void ICACHE_FLASH_ATTR
DNSRetryTimerCB(void *arg)
{
    struct espconn *pEspConn = arg;
    TRACE("DNS retry\n");
    if(DNSFound){
	    os_timer_disarm(&tmDNS);
    	return;
    }
    espconn_gethostbyname(pEspConn, (char *)stFlashProtParam.RemoteAddr.Domain, &ipDNS, DNSFoundCB);
	if(DNSRetryCtn >= MAX_DNS_RETRY ){
		system_os_post(TCPCOMM_TASK_PRIO, TSIG_DNS_FAILED, 0x00);
		DNSRetryCtn = 0;
	    os_timer_disarm(&tmDNS);
	}else{
		os_timer_arm(&tmDNS, 5000, 0);
	}
}

void ICACHE_FLASH_ATTR
WifiStaConnCB(){
	if(!TCPServOn){
		TCPServInit(TCP_SERV_PORT);
	}
	if(!TCPClientOn){
		if(stFlashProtParam.Domain){
			struct ip_info stationIP;
			wifi_get_ip_info(STATION_IF,&stationIP);
			memcpy(tcpDNSTmp.local_ip,&stationIP.ip.addr,4);

			connDNSTmp.type = ESPCONN_TCP;
			connDNSTmp.state = ESPCONN_NONE;
			connDNSTmp.proto.tcp = &tcpDNSTmp;

	        DNSFound = false;
		    espconn_gethostbyname(&connDNSTmp,stFlashProtParam.RemoteAddr.Domain , &ipDNS, DNSFoundCB);

		    os_timer_disarm(&tmDNS);
		    os_timer_setfn(&tmDNS, (os_timer_func_t *)DNSRetryTimerCB, &connDNSTmp);
		    os_timer_arm(&tmDNS, 5000, 0);
		}else{
			TCPClientInit(stFlashProtParam.RemoteAddr.IP.addr, stFlashProtParam.RemotePort);
			StationTimerInit();
		}
	}
}

void TCPCommTask(os_event_t *e){
	struct espconn *pEspConn;
	char *pData;
	switch(e->sig){
		case TSIG_START_SERV:
			if(!TCPServOn){
				TCPServInit(e->par);
			}
			break;
		case TSIG_REPORT:
			if(!TCPClientOn) return;
			LOCAL char szSendBuf[64];
			os_memset(szSendBuf,0,sizeof(szSendBuf));
			os_sprintf(szSendBuf,"{\"relay\":\"%s\"}",RelayStatus?"off":"on");
			TCPResponse(&connClient,szSendBuf,(uint16)os_strlen(szSendBuf));
			break;
		case TSIG_WIFI_CHANGE:
			break;
		case TSIG_DISCONN:
			pEspConn = (struct espconn*) (e->par);
			if(espconn_disconnect(pEspConn)!=0){  //error code is ESPCONN_ARG
				TRACE("client disconnect failed, argument illegal\r\n");
			}
			break;
		case TSIG_REMOTE_SERVCHG:
			staGotIPCB = WifiStaConnCB;
			break;
		case TSIG_DNS_FAILED:
			stFlashProtParam.WorkStatus |= WEB_SERV_BIT;
			stFlashProtParam.WorkStatus &= ~REMOTE_SERV_CONF_BIT;
			stFlashProtParam.WorkStatus &= ~TCP_SERV_BIT;
			wifi_set_opmode(STATIONAP_MODE);
			system_os_post(WEBSERV_TASK_PRIO, WSIG_START_SERV, WEB_SERV_PORT);
			break;
		default:
			break;
	}
}

LOCAL void ICACHE_FLASH_ATTR
TCPResponse(void *arg, const char *pData, uint16 length){
    struct espconn * pEspConn = arg;
	char respBuf[64];
	os_memset(respBuf,0,sizeof(respBuf));
	os_printf("response:%s\r\n",pData);

	respBuf[0] = REPORT_PREFIX >> 8;
	respBuf[1] = REPORT_PREFIX & 0xff;
	respBuf[2] = length>>8;
	respBuf[3] = length & 0xff;
	os_memcpy(respBuf+4,pData,length);

#ifdef TCP_SERV_SSL_ENABLE
        espconn_secure_sent(pEspConn, respBuf, length+4);
#else
        espconn_sent(pEspConn,respBuf, length+4);
#endif
}

LOCAL void ICACHE_FLASH_ATTR
TCPServRecvCB(void *arg, char *pDataRecv, unsigned short length)
{
	struct espconn *pEspConn = arg;
	char respBuf[64];
	os_memset(respBuf,0,sizeof(respBuf));

	TRACE("recved:%02x%02x%02x%02x%s,len:%d\r\n",pDataRecv[0],pDataRecv[1],pDataRecv[2],pDataRecv[3],pDataRecv+4,length);
	if(length>=4 && (pDataRecv[0]==(COMM_PREFIX >> 8)) && (pDataRecv[1] == (COMM_PREFIX & 0xff )) ){
		uint16 len;
		len = (pDataRecv[2]<<8) + pDataRecv[3];
		if(length-4 == len){
			if(os_strncmp(pDataRecv+4,"relay-on",8)){
		        GPIO_OUTPUT_SET(GPIO_ID_PIN(RELAY_PIN), RELAY_ON);
		        RelayStatus = RELAY_ON;
				os_sprintf(respBuf,"ok");
			}else if(os_strncmp(pDataRecv+4,"relay-off",9)){
		        GPIO_OUTPUT_SET(GPIO_ID_PIN(RELAY_PIN), RELAY_OFF);
		        RelayStatus = RELAY_OFF;
				os_sprintf(respBuf,"ok");
			}else if(os_strncmp(pDataRecv+4,"webserv-on",10)){
				stFlashProtParam.WorkStatus |= WEB_SERV_BIT;
				SaveFlashProtParam();
				system_os_post(WEBSERV_TASK_PRIO,WSIG_START_SERV,WEB_SERV_PORT);
				os_sprintf(respBuf,"ok");
			}else if(os_strncmp(pDataRecv+4,"webserv-off",11)){
				stFlashProtParam.WorkStatus &= ~WEB_SERV_BIT;
				SaveFlashProtParam();
				os_sprintf(respBuf,"ok");
				goto EXIT_RESPONSE;
			}else{
				os_sprintf(respBuf,"failed");
			}
		}else{
			os_sprintf(respBuf,"failed");
		}
	}else{
		TRACE("tcp server parse fail, error format\r\n");
	}
EXIT_FAILED:
	os_sprintf(respBuf,"failed");
EXIT_RESPONSE:
	TCPResponse(pEspConn,respBuf,os_strlen(respBuf));
}

LOCAL void ICACHE_FLASH_ATTR
TCPServSentCB(void *arg)
{
}

LOCAL ICACHE_FLASH_ATTR
void TCPServRecon(void *arg, sint8 err){
    struct espconn *pEspConn = arg;
	TCPServOn = false;
    os_printf("client : "IPSTR":%d come up with fault, try to reconnect\r\n", IP2STR(pEspConn->proto.tcp->remote_ip),pEspConn->proto.tcp->remote_port);

	sint8 retCode = 0;
#ifdef TCP_SERV_SSL_ENABLE
    espconn_secure_set_default_certificate(default_certificate, default_certificate_len);
    espconn_secure_set_default_private_key(default_private_key, default_private_key_len);
    retCode = espconn_secure_accept(&connServer);
#else
    retCode = espconn_accept(&connServer);
	//espconn_regist_time(&connServer,600, 1);			// client connectted timeout, unit: second, 0~7200, this api don't support ssl, third param 1 is only apply to this tcp, 0 for all
#endif
	if(retCode == 0){
		TCPServOn = true;
	}
}

LOCAL ICACHE_FLASH_ATTR
void TCPServDiscon(void *arg){
    struct espconn *pEspConn = arg;
    os_printf("client: "IPSTR":%d disconnected\r\n", IP2STR(pEspConn->proto.tcp->remote_ip),pEspConn->proto.tcp->remote_port);
    TCPServOn = false;
}

LOCAL void ICACHE_FLASH_ATTR
TCPServListen(void *arg){
    struct espconn *pEspConn = arg;
    os_printf("client: "IPSTR":%d connected\r\n", IP2STR(pEspConn->proto.tcp->remote_ip),pEspConn->proto.tcp->remote_port);

    espconn_regist_recvcb(pEspConn, TCPServRecvCB);
    espconn_regist_sentcb(pEspConn, TCPServSentCB);
    espconn_regist_reconcb(pEspConn, TCPServRecon);
    espconn_regist_disconcb(pEspConn, TCPServDiscon);
}

void ICACHE_FLASH_ATTR
TCPServInit(uint32 port){
	struct ip_info stationIP;
	wifi_get_ip_info(STATION_IF,&stationIP);
	memcpy(espTcp.local_ip,&stationIP.ip.addr,4);

	connServer.type = ESPCONN_TCP;
	connServer.state = ESPCONN_NONE;
	connServer.proto.tcp = &espTcp;
	connServer.proto.tcp->local_port = port;
	espconn_regist_connectcb(&connServer, TCPServListen);

	sint8 retCode = 0;
#ifdef TCP_SERV_SSL_ENABLE
    espconn_secure_set_default_certificate(default_certificate, default_certificate_len);
    espconn_secure_set_default_private_key(default_private_key, default_private_key_len);
    retCode = espconn_secure_accept(&connServer);
#else
    retCode = espconn_accept(&connServer);
	espconn_regist_time(&connServer,600, 1);			// client connectted timeout, unit: second, 0~7200, this api don't support ssl, third param 1 is only apply to this tcp, 0 for all
#endif
	if(retCode == 0){
		TCPServOn = true;
	    os_printf("tcp server start up, listen on port:%d\r\n", port);
	}else{
		TRACE("tcp server start failed, code:%d\r\n",retCode);
	}
}

/********************* TCP Client **********************/


static void ICACHE_FLASH_ATTR
TCPClientSendCB(void *arg) {
	//os_printf("tcp_client_send_cb\n");
}

static void ICACHE_FLASH_ATTR
TCPClientRecvCB(void *arg, char *pDataRecv, unsigned short len) {
	os_printf("tcp client received:%s\n",pDataRecv);
}

static void ICACHE_FLASH_ATTR
TCPClientReconnCB(void *arg,sint8 err) {
    struct espconn *pEspConn = arg;
    TCPClientOn = false;
    os_printf("fault happened, try to reconnect to server: "IPSTR":%d\n", IP2STR(pEspConn->proto.tcp->remote_ip),pEspConn->proto.tcp->remote_port);

    switch(err){
    case ESPCONN_TIMEOUT:
    	break;
    case ESPCONN_ABRT: //exception
        break;
    case ESPCONN_RST:  //exception when reset
        break;
    case ESPCONN_CLSD: //error when close
        break;
    case ESPCONN_CONN:  //tcp connect failed
        break;
    case ESPCONN_HANDSHAKE:  //ssl handshake failed
        break;
    case ESPCONN_SSL_INVALID_DATA: //ssl data proceed fault
        break;
    default:
        break;
    }

}

static void ICACHE_FLASH_ATTR
TCPClientDisconnCB(void *arg) {
    struct espconn *pEspConn = arg;
    os_printf("disconnected from server: "IPSTR":%d\r\n", IP2STR(pEspConn->proto.tcp->remote_ip),pEspConn->proto.tcp->remote_port);
    TCPClientOn = false;
}

static void ICACHE_FLASH_ATTR
TCPClientConnCB(void *arg) {
    struct espconn *pEspConn = arg;
    os_printf("connected to server: "IPSTR":%d\r\n", IP2STR(pEspConn->proto.tcp->remote_ip),pEspConn->proto.tcp->remote_port);

    sint8 retCode = espconn_set_opt(pEspConn,ESPCONN_KEEPALIVE);
    if(retCode == 0){
    	TRACE("set keepalive success\r\n");
    	uint16 optVal;
    	espconn_get_keepalive(pEspConn,ESPCONN_KEEPIDLE,&optVal);
    	TRACE("keep idle:%d\n",optVal);

    	espconn_get_keepalive(pEspConn,ESPCONN_KEEPINTVL,&optVal);
    	TRACE("keep interval:%d\n",optVal);

    	espconn_get_keepalive(pEspConn,ESPCONN_KEEPCNT,&optVal);
    	TRACE("keep keepcount:%d\n",optVal);
    }else{
    	TRACE("set keepalive failed\r\n");
    }

	espconn_regist_recvcb(pEspConn, TCPClientRecvCB);
	espconn_regist_sentcb(pEspConn, TCPClientSendCB);
	espconn_regist_disconcb(pEspConn, TCPClientDisconnCB);
	espconn_regist_reconcb(pEspConn, TCPClientReconnCB); //callback to deal with faults

	u8 *sendBuf = "hi";
	TCPResponse(pEspConn,sendBuf,os_strlen(sendBuf));
}

void ICACHE_FLASH_ATTR
TCPClientInit(uint32 ipAddr, uint16 port) {
	static esp_tcp espTCP;
	//uint32 u32IP = ipaddr_addr(ipAddr);

	os_printf("Initialize tcp client...\r\n");

	connClient.type = ESPCONN_TCP;
	connClient.state = ESPCONN_NONE;
	connClient.proto.tcp = &espTCP;
	os_memcpy(connClient.proto.tcp->remote_ip, &ipAddr, 4);//set server ip
	connClient.proto.tcp->remote_port = port;			//set server port
	connClient.proto.tcp->local_port = espconn_port();

	espconn_regist_connectcb(&connClient, TCPClientConnCB);

	sint8 retCode = 0;
	retCode = espconn_connect(&connClient);

	if(retCode == 0){ TCPClientOn = true;}
    os_printf("connect to "IPSTR":%d return code:%d\r\n", IP2STR(connClient.proto.tcp->remote_ip),connClient.proto.tcp->remote_port,retCode);
}


