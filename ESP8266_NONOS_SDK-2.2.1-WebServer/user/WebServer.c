#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
#include "espconn.h"
#include "upgrade.h"
#include "WebServer.h"
#include "FlashParam.h"
#include "user_config.h"
#include "GlobalInfo.h"

#ifdef SERVER_SSL_ENABLE
#include "ssl/cert.h"
#include "ssl/private_key.h"
#endif

//declare as static const char[] to save RAM space;
static const char pathWifiConf[] = "wificonfig";
static const char pathServConf[] = "serverconfig";
static const char pathConfFinish[] = "configfinish";
static const char pathWebCtrl[] = "webcontrol";
static const char pathTest[] = "test";

static const char pageWifiConf[] = "<html><body><form action=\"http://192.168.4.1/wificonfig\" method=\"post\">\
<p>WIFI ssid  :<input type=\"text\" name=\"ssid\" /></p>\
<p>WIFI passwd:<input type=\"text\" name=\"passwd\" /></p>\
<input type=\"submit\" value=\"Submit\" />\
</form></body></html>";
static const char pageServConf[]  = "<html><body><form action=\"http://192.168.4.1/serverconfig\" method=\"post\">\
<p>MQTT Server Addr:<input type=\"text\" name=\"serv\" />\
Use Domain<input type=\"checkbox\" name=\"domain\"></p>\
<p>MQTT Server Port:<input type=\"text\" name=\"port\" /></p>\
<input type=\"submit\" value=\"Submit\" />\
</form></body></html>";
static const char pageConfFinish[]  = "<html><body><h1>Config Finish, wait seconds to reboot!</h1></br>"
		"<button type=\"button\" onclick=\"window.location.href='/webcontrol'\">Control</button>"
		"</body></html>";
static const char pageWebCtrl[]  = "<html><body><h1>Sorry, this page is unimplemented!Please use tcp tools like curl to control it!</h1></body></html>";

static const char pageTest[] = "Test page!";


extern struct FlashProtectParam stFlashProtParam;
os_event_t *WebTaskQueue;
LOCAL struct espconn espConnServ;
LOCAL esp_tcp espTcp;
bool WebServOn = false;
LOCAL struct station_config stationConf;

void ICACHE_FLASH_ATTR
WebServTask(os_event_t *e){
	struct espconn *pEspConn;
	struct station_config *pStatConf;
	uint8 WifiMode;
	switch(e->sig){
	case WSIG_START_SERV:
		WifiMode = wifi_get_opmode_default();
		if(!(WifiMode & SOFTAP_MODE)){
			WifiMode = WifiMode | SOFTAP_MODE;
			wifi_set_opmode(WifiMode);
		}
		WebServInit(e->par);
		break;
	case WSIG_DISCONN:
		pEspConn = (struct espconn*) e->par;
		if(espconn_disconnect(pEspConn)!=0){  //error code is ESPCONN_ARG
			TRACE("client disconnect failed, argument illegal\r\n");
		}
		break;
	case WSIG_WIFI_CHANGE:
		pStatConf = (struct station_config *)e->par;
		bool ret = WifiStationConfig(pStatConf);
		TRACE("wifi station connect return:%s",ret?"true":"false");
		break;
	case WSIG_REMOTE_SERVCHG:
		wifi_set_opmode(STATION_MODE);
		system_os_post(TCPCOMM_TASK_PRIO,TSIG_REMOTE_SERVCHG,0x00);
		break;
	default:
		break;
	}
}

LOCAL void ICACHE_FLASH_ATTR
WebServResponse(void *arg, HttpStatusCode statCode, const char *pData, const char *pRedirect)
{
    uint16 length = 0;
    char *pBuf = NULL;
    char headBuf[256];
    os_memset(headBuf, 0, 256);
    struct espconn * pEspConnClient = arg;

    char *pCodeDspt=NULL;
    switch(statCode){
    case SUCCESS:
    	pCodeDspt = "OK";
    	break;
    case REDIRECTION:
    	pCodeDspt = "redirection";
    	break;
    case BAD_REQUEST:
    	pCodeDspt = "Bad Request";
    	length = os_strlen(headBuf);
    	break;
    case SERV_ERROR:
    	pCodeDspt = "Server Internal Error";
    	break;
    default:
    	pCodeDspt = "Server Internal Error";
    	break;
    }
	os_sprintf(headBuf, "HTTP/1.1 %d %s\r\nServer: ESP8266\r\nContent-type: text/html;charset=utf-8\r\nPragma: no-cache\r\n",statCode,pCodeDspt);
	if(pRedirect){
		os_sprintf(headBuf+os_strlen(headBuf),"Location: http://192.168.4.1/%s\r\n\r\n",pRedirect);
	}else{
		os_sprintf(headBuf+os_strlen(headBuf),"\r\n");
	}
	length = os_strlen(headBuf);

	if(statCode == SUCCESS){
		os_sprintf(headBuf+os_strlen(headBuf)-2,"Content-Length: %d\r\n\r\n",pData ? os_strlen(pData) : 0);
		length = os_strlen(headBuf);
		if (pData) {
			length = os_strlen(headBuf);
			pBuf = (char *)os_zalloc(length + os_strlen(pData) + 1);
			if(pBuf != NULL){
				os_memcpy(pBuf, headBuf, length);
				os_memcpy(pBuf + length, pData, os_strlen(pData));
				length += os_strlen(pData);
			}else{
				statCode = SERV_ERROR;
				//ignore the redirection because of the unexpected error
				os_sprintf(headBuf, "HTTP/1.1 500 Server Internal Error\r\nContent-Length: 0\r\nServer: ESP8266\r\n\r\n");
				length = os_strlen(headBuf);
			}
		}
	}

    TRACE("head:%s",headBuf);

    if (pData && pBuf!=NULL) {
        TRACE("buf:%s",pBuf);
#ifdef WEB_SERV_SSL_ENABLE
        espconn_secure_sent(pEspConnClient, pBuf, length);
#else
        espconn_sent(pEspConnClient, pBuf, length);
#endif
    } else {
#ifdef WEB_SERV_SSL_ENABLE
        espconn_secure_sent(pEspConnClient, headBuf, length);
#else
        espconn_sent(pEspConnClient, headBuf, length);
#endif
    }
    if (pBuf) {
        os_free(pBuf);
    }
}

//this function will modify the string context param pData pointed to
LOCAL bool ICACHE_FLASH_ATTR
ParsePostData(char *pData, unsigned short length, PostParam *pParam){
	if(pData==NULL)return false;
	if(pData[0]=='{'){
		pParam->eEncode = JSON_ENCODE;
		pParam->jsonData = cJSON_Parse(pData);
		if(NULL == pParam->jsonData){
			TRACE("parse json string from post data error:%s\r\n",pData);
			return false;
		}
	}else{
		pParam->eEncode = URL_ENCODE;
		char *pPtr = pData;
		pParam->jsonData = cJSON_CreateObject();

		while(pPtr){
			char *pSplit = NULL;
			char *pKV = NULL;

			pSplit = os_strstr(pPtr,"&");
			pKV = os_strstr(pPtr,"=");
			if(pKV){
				if(pSplit!=NULL){
					pSplit[0] = '\0';
				}
				pKV[0] = '\0';
				cJSON_AddStringToObject(pParam->jsonData,pPtr,pKV+1);
			}
			if(pSplit==NULL){
				break;
			}
			pPtr = pSplit +1;
		}
	}
	return true;

}

LOCAL bool ICACHE_FLASH_ATTR
GetPostData(char *pRecv, unsigned short length, char **pPostData){
	if(pRecv==NULL){ return false;}
	*pPostData = (char*)os_strstr(pRecv,"\r\n\r\n");
	if(*pPostData==NULL){
		return false;
	}
	*pPostData += 4;

	char* pContLen = (char *)os_strstr(pRecv,"Content-Length: ");
	if(pContLen == NULL){ return false;}
	pContLen+=16;

	char *pLenEnd = (char *)os_strstr(pContLen,"\r\n");
	if(pLenEnd == NULL || pLenEnd-pContLen>9){ return false;}

	char lenBuf[11]={0};
	os_memcpy(lenBuf, pContLen, pLenEnd-pContLen);
	uint32 contLen = atoi(lenBuf);
	if(length-(*pPostData-pRecv) != contLen){ return false;}
	return true;
}

LOCAL bool ICACHE_FLASH_ATTR
ParseURL(char *pRecv, unsigned short length, URLParam *pUrlParam){
	if(pRecv==NULL){ return false;}
	char *pTemp = NULL;
	char *pTemp2 = NULL;

	if(os_strncmp(pRecv,"GET ",4)==0){
		pUrlParam->eMethod = GET;
	}else if(os_strncmp(pRecv,"POST ",5)==0){
		pUrlParam->eMethod = POST;
	}else{ return false;}

	pTemp = (char*)os_strstr(pRecv,"/");
	if(pTemp==NULL){
		return false;
	}else{
		char *pEnd = (char*)os_strstr(pTemp," HTTP");
		if(pEnd==NULL || pEnd<pTemp){ return false; }

		pTemp2 = (char*)os_strstr(pTemp,"?");
		if(pTemp2!=NULL && pEnd>pTemp2){
			os_memcpy(pUrlParam->szPath, pTemp+1, (pTemp2-pTemp-1 )<MAX_PATH?(pTemp2-pTemp-1 ):MAX_PATH);
			os_memcpy(pUrlParam->szParam,pTemp2+1,(pEnd-pTemp2-1)<MAX_PARAM?(pEnd-pTemp2-1):MAX_PARAM);
		}else{
			if(pEnd-pTemp-1>0){
				os_memcpy(pUrlParam->szPath, pTemp+1, (pEnd-pTemp-1)<MAX_PATH?(pEnd-pTemp-1):MAX_PATH);
			}else{
				os_memset(pUrlParam->szPath,0,sizeof(pUrlParam->szPath));
				os_memset(pUrlParam->szParam,0,sizeof(pUrlParam->szParam));
			}
		}
	}
	return true;
}

/******************************************************************************
 * Parameters   : arg -- Additional argument to pass to the callback function
 *                pusrdata -- The received data (or NULL when the connection has been closed!)
 *                length -- The length of received data
*******************************************************************************/
LOCAL void ICACHE_FLASH_ATTR
WebServRecvCB(void *arg, char *pDataRecv, unsigned short length)
{
	HttpStatusCode respCode;
	const char *pRespBuf = NULL;
	const char *pRedirect = NULL;

	URLParam *pUrlParam = NULL;
	PostParam * pPostParam = NULL;
	TRACE("recv:\r\n%s",pDataRecv);

	struct espconn *pEspConn = arg;
	if(pDataRecv == NULL){
		//uint8	espconn_disconnect(struct	espconn	*espconn);  //should call by task and not be called in any callback func
    	system_os_post(WEBSERV_TASK_PRIO, WSIG_DISCONN, (uint32)pEspConn);
		respCode = BAD_REQUEST;
		goto EXIT_FAIL;
	}
	pUrlParam = (URLParam *)os_zalloc(sizeof(URLParam));
	if(pUrlParam == NULL){
		respCode = SERV_ERROR;
		goto EXIT_FAIL;
	}
	if(!ParseURL(pDataRecv,length,pUrlParam)){
		TRACE("parse url error\r\n");
		respCode = BAD_REQUEST;
		goto EXIT_FREE_URLPARAM;
	}
	TRACE("host:%s\r\n",pUrlParam->szHost);
	TRACE("param:%s\r\n",pUrlParam->szParam);
	TRACE("path:%s\r\n",pUrlParam->szPath);
	TRACE("method:%d\r\n",pUrlParam->eMethod);

	if(pUrlParam->eMethod == GET){
		if((pUrlParam->szPath[0]==0)||(os_strncmp(pUrlParam->szPath,pathWifiConf,sizeof(pathWifiConf))==0) ){
			pRespBuf = (char*)pageWifiConf;
			respCode = SUCCESS;
			goto EXIT_FREE_URLPARAM;
		}else if(os_strncmp(pUrlParam->szPath,pathServConf,sizeof(pathServConf))==0){
			pRespBuf = (char*)pageServConf;
			respCode = SUCCESS;
			goto EXIT_FREE_URLPARAM;
		}else if(os_strncmp(pUrlParam->szPath,pathConfFinish,sizeof(pathConfFinish))==0){
			pRespBuf = (char*)pageConfFinish;
			respCode = SUCCESS;
			goto EXIT_FREE_URLPARAM;
		}else if(os_strncmp(pUrlParam->szPath,pathWebCtrl,sizeof(pathWebCtrl))==0){
			pRespBuf = (char*)pageWebCtrl;
			respCode = SUCCESS;
			goto EXIT_FREE_URLPARAM;
		}else if(os_strncmp(pUrlParam->szPath,pathTest,sizeof(pathTest))==0){
			pRespBuf = (char*)pageTest;
			respCode = SUCCESS;
			goto EXIT_FREE_URLPARAM;
		}
		respCode = BAD_REQUEST;
		goto EXIT_FREE_URLPARAM;
	}else if(pUrlParam->eMethod == POST){
		char *pPostData = NULL;
		if(!GetPostData(pDataRecv,length,&pPostData)||pPostData==NULL){
			TRACE("get post data failed\r\n");
			respCode = BAD_REQUEST;
			pRedirect = pUrlParam->szPath;
			goto EXIT_FREE_URLPARAM;
		}
		pPostParam = (PostParam *)os_zalloc(sizeof(PostParam));
		if(pPostParam == NULL){
			TRACE("system alloc for post param failed\r\n");
			respCode = SERV_ERROR;
			pRedirect = pUrlParam->szPath;
			goto EXIT_FREE_POSTPARAM;
		}
		if(ParsePostData(pPostData, length - (pPostData-pDataRecv), pPostParam)==false){
			TRACE("parse post data failed\r\n");
			respCode = BAD_REQUEST;
			pRedirect = pUrlParam->szPath;
			goto EXIT_FREE_POSTPARAM;
		}
		if(os_strncmp(pUrlParam->szPath,"wificonfig",10)==0){
			cJSON * ssid = cJSON_GetObjectItem(pPostParam->jsonData,"ssid");
			cJSON * passwd = cJSON_GetObjectItem(pPostParam->jsonData,"passwd");

			char * str = cJSON_Print(pPostParam->jsonData);
			TRACE("json parse:%s\r\n",str);
			cJSON_free(str);

			if(ssid==NULL || os_strlen(ssid->valuestring)>SSID_LEN || ((passwd!=NULL)&&(os_strlen(passwd->valuestring)>PASSWD_LEN) ) ){
				if(ssid==NULL)TRACE("ssid is null");
				else TRACE("ssid:%s fail,%d\r\n",ssid->valuestring,os_strlen(ssid->valuestring));

				respCode = BAD_REQUEST;
				pRedirect = pathWifiConf;
				goto EXIT_FREE_JSONPARAM;
			}

			TRACE("ssid:%s\r\n",ssid->valuestring);

			os_memset(&stationConf,0,sizeof(stationConf));
			os_memcpy(stationConf.ssid,ssid->valuestring,os_strlen(ssid->valuestring));

			if(passwd!=NULL){
				os_memcpy(stationConf.password,passwd->valuestring,os_strlen(passwd->valuestring));
			}else{
				os_memset(stationConf.password,0,sizeof(stationConf.password));
			}
			TRACE("passwd:%s\r\n",stationConf.password);

			stationConf.bssid_set = 0;  //ignoring to check the mac address of AP

			stFlashProtParam.WorkStatus |= WIFI_CONF_BIT ;
			SaveFlashProtParam();

			//WifiStationConfig(&stationConf);  //better not to call this in callback func
			system_os_post(WEBSERV_TASK_PRIO, WSIG_WIFI_CHANGE,(uint32) &stationConf);
			system_os_post(TCPCOMM_TASK_PRIO, TSIG_WIFI_CHANGE, 0x00);

			respCode = SUCCESS;
			pRespBuf = (char*)pageServConf;
			//pRedirect = pathServConf;
			goto EXIT_FREE_JSONPARAM;
		}else if(os_strncmp(pUrlParam->szPath,"serverconfig",12)==0){
			cJSON *server = cJSON_GetObjectItem(pPostParam->jsonData,"serv");
			cJSON *port = cJSON_GetObjectItem(pPostParam->jsonData,"port");
			cJSON *domain = cJSON_GetObjectItem(pPostParam->jsonData,"domain");

			if(server==NULL){
				respCode = BAD_REQUEST;
				pRedirect = pathServConf;
				goto EXIT_FREE_JSONPARAM;
			}
			TRACE("server:%s\r\n",server->valuestring);
			if(domain != NULL && (os_strncmp(domain->valuestring, "on",2)==0) && (os_strlen(server->valuestring)<=DOMAIN_LEN)){
				TRACE("domain:%s\r\n",domain->valuestring);
				stFlashProtParam.Domain = 1;
				os_memset(stFlashProtParam.RemoteAddr.Domain,0,sizeof(stFlashProtParam.RemoteAddr.Domain));
				os_memcpy(stFlashProtParam.RemoteAddr.Domain,server->valuestring,os_strlen(server->valuestring));
			}else{
				stFlashProtParam.Domain = 0;
				stFlashProtParam.RemoteAddr.IP.addr=ipaddr_addr(server->valuestring);
			}
			if(port!=NULL && (os_strlen(port->valuestring)<=PORT_NUM_LEN) ){
				TRACE("port:%s\r\n",port->valuestring);
				stFlashProtParam.RemotePort = atoi(port->valuestring);
			}else{
				stFlashProtParam.RemotePort = 80;
			}
			stFlashProtParam.WorkStatus |= TCP_SERV_BIT | REMOTE_SERV_CONF_BIT;
			stFlashProtParam.WorkStatus &= ~WEB_SERV_BIT;
			SaveFlashProtParam();

			system_os_post(WEBSERV_TASK_PRIO,WSIG_REMOTE_SERVCHG,0x00);

			respCode = SUCCESS;
			pRespBuf = (char*)pageConfFinish;
			//pRedirect = pathConfFinish;
			goto EXIT_FREE_JSONPARAM;
		}else{
			TRACE("path unrecognized\r\n");
			respCode = BAD_REQUEST;
			pRedirect = pathWebCtrl;
			goto EXIT_FREE_POSTPARAM;
		}
	}
EXIT_FREE_JSONPARAM:
	cJSON_Delete(pPostParam->jsonData);
EXIT_FREE_POSTPARAM:
	os_free(pPostParam);
EXIT_FREE_URLPARAM:
	os_free(pUrlParam);
EXIT_FAIL:
	WebServResponse(pEspConn,respCode,pRespBuf,pRedirect);
	return;
}

LOCAL void ICACHE_FLASH_ATTR
WebServSentCB(void *arg)
{
}

LOCAL ICACHE_FLASH_ATTR
void WebServReconCB(void *arg, sint8 err){
    struct espconn *pEspConn = arg;
    os_printf("server: "IPSTR":%d reconnected\n", IP2STR(pEspConn->proto.tcp->remote_ip),pEspConn->proto.tcp->remote_port);
}

LOCAL ICACHE_FLASH_ATTR
void WebServDisconCB(void *arg){
    struct espconn *pEspConn = arg;
    os_printf("server: "IPSTR":%d disconnected\n", IP2STR(pEspConn->proto.tcp->remote_ip),pEspConn->proto.tcp->remote_port);
}

LOCAL void ICACHE_FLASH_ATTR
WebServListenCB(void *arg){
    struct espconn *pEspConn = arg;
    os_printf("server: "IPSTR":%d connected\n", IP2STR(pEspConn->proto.tcp->remote_ip),pEspConn->proto.tcp->remote_port);

    espconn_regist_recvcb(pEspConn, WebServRecvCB);
    espconn_regist_sentcb(pEspConn, WebServSentCB);
    espconn_regist_reconcb(pEspConn, WebServReconCB);
    espconn_regist_disconcb(pEspConn, WebServDisconCB);
}

void ICACHE_FLASH_ATTR
WebServInit(uint32 port){
	espConnServ.type = ESPCONN_TCP;
	espConnServ.state = ESPCONN_NONE;
	espConnServ.proto.tcp = &espTcp;
	espConnServ.proto.tcp->local_port = port;
	espconn_regist_connectcb(&espConnServ, WebServListenCB);

#ifdef WEB_SERV_SSL_ENABLE
    espconn_secure_set_default_certificate(default_certificate, default_certificate_len);
    espconn_secure_set_default_private_key(default_private_key, default_private_key_len);
    espconn_secure_accept(&espConnServ);
#else
	espconn_accept(&espConnServ);
#endif
	espconn_regist_time(&espConnServ,600, 1);			// client connectted timeout, unit: second, 0~7200
	WebServOn = true;
}
