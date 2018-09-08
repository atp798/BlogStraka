#ifndef __FLASH_PARAM_H__
#define __FLASH_PARAM_H__

#define SYS_PROTECT_SECT    	(UserParamStartSect+1)
#define WORK_MODE_OFFSET  		0

#define AP_INIT_CONF_SERV 		0x87  	//softAP, wait for router param, and http server to control devicce
#define STA_LAN_WEB_CTRL_SERV	0x1E   	//--
#define STA_LAN_BOTH_SERV		0XE1	//
#define STA_AP_LAN_BOTH_SERV	0x5A
#define STA_REPORT_TCP_SERV		0x78	//--station report, and tcp server to accept command
#define STA_AP_INET_BOTH_SERV	0xA5	//station report, and web server、tcp server to receive command

#define WEB_SERV_BIT 			0x01
#define TCP_SERV_BIT			0x02
#define MQTT_CLIENT_BIT			0x04

#define WIFI_CONF_BIT			0x10
#define REMOTE_SERV_CONF_BIT	0x20
#define MQTT_CONF_BIT			0x40
#define SOFTAP_CONF_BIT			0x80


#define SSID_LEN 		31
#define PASSWD_LEN 		63
#define DOMAIN_LEN 		19
#define PORT_NUM_LEN 	5

struct FlashProtectParam{
	uint16 ConfHolder;
	//uint32 ConfMagic;
	uint8 WorkStatus;
	uint8 Domain;  //0: use ip,  1:use domain
	uint16 RemotePort;
	union{
		struct ip_addr IP;
		uint8 Domain[DOMAIN_LEN+1];
	}RemoteAddr;
};


SpiFlashOpResult SPIFlashEraseWrite(uint32 sect,uint32 offset,uint32 *src, uint32 size);
bool LoadFlashProtParam();
bool SaveFlashProtParam();
#endif
