#include "ets_sys.h"
#include "os_type.h"
#include "mem.h"
#include "osapi.h"
#include "spi_flash.h"
#include "user_interface.h"
#include "FlashParam.h"
#include "user_config.h"
#include "GlobalInfo.h"

struct FlashProtectParam stFlashProtParam;
uint32 UserParamStartSect = 0;

SpiFlashOpResult ICACHE_FLASH_ATTR
SPIFlashEraseWrite(uint32 sect,uint32 offset,uint32 *src, uint32 size){
    if(offset+size>4096){
        return SPI_FLASH_RESULT_ERR;
    }
    uint32 data[4096];
    /*
     uint32 *data = (uint32 *)os_malloc(4096);
     if(data==NULL) return SPI_FLASH_RESULT_ERR;
      */
    SpiFlashOpResult ret;
    ret = spi_flash_read(sect*4096,data,4096);
    if(ret!=SPI_FLASH_RESULT_OK) return ret;
    os_memcpy(data+offset,src,size);
    spi_flash_erase_sector(sect);
    return spi_flash_write(sect*4096,data,4096);
}

bool ICACHE_FLASH_ATTR
LoadFlashProtParam(){
	if(!system_param_load(SYS_PROTECT_SECT,0, &stFlashProtParam, sizeof(struct FlashProtectParam))){
		stFlashProtParam.WorkStatus = WEB_SERV_BIT;
		system_param_save_with_protect(SYS_PROTECT_SECT,&stFlashProtParam,sizeof(struct FlashProtectParam));
		TRACE("load flash protected param failed!\r\n");
	}
	if(stFlashProtParam.ConfHolder != CONF_HOLDER){
		stFlashProtParam.ConfHolder = CONF_HOLDER;
		stFlashProtParam.WorkStatus = WEB_SERV_BIT;

		system_param_save_with_protect(SYS_PROTECT_SECT,&stFlashProtParam,sizeof(struct FlashProtectParam));

		wifi_set_opmode(SOFTAP_MODE); //

		struct softap_config config;
		wifi_softap_get_config(&config); // Get config first.

		//generate special ssid to avoid conflict
		uint8 ssid[32];
		os_memset(ssid, 0, 32);
		os_memcpy(ssid, "ESP_", 4);
		uint8 APMac[6];
		wifi_get_macaddr(SOFTAP_IF,APMac);
		uint8 i;
		for(i=0;i<6;i++){
		   uint8 tmp = APMac[i]%62;
		   if(tmp<26){
			   ssid[4+i] = tmp+'a';
		   }else if(tmp<52){
			   ssid[4+i] = tmp+'A'-26;
		   }else{
			   ssid[4+i] = tmp+'0'-52;
		   }
		}
		os_memset(config.ssid, 0, 32);
		os_memcpy(config.ssid, ssid, os_strlen(ssid));

		os_memset(config.password, 0, 64);
		os_memcpy(config.password, INIT_AP_PASSWD, sizeof(INIT_AP_PASSWD));
		config.authmode = AUTH_WPA_WPA2_PSK;
		config.ssid_len = 0;// or its actual length, read until string end
		config.max_connection = 4; // how many stations can connect to ESP8266 softAP at most.max 4
		//config.channel = 1; //support 1~13
		//config.ssid_hidden = 0;//default 0
		//config.beacon_interval = 100;//100~60000ms, default 100

		wifi_softap_set_config(&config);// Set ESP8266 softap config, ensure to call this func after softap enabled, execute immediatelly
	}
	TRACE("flash protected param:\r\nWork Status:%02x\r\n",stFlashProtParam.WorkStatus);
	if(stFlashProtParam.Domain){
		TRACE("remote domain:%s:%d\r\n",stFlashProtParam.RemoteAddr.Domain,stFlashProtParam.RemotePort);
	}else{
		TRACE("remote:"IPSTR":%d",IP2STR(&stFlashProtParam.RemoteAddr.IP.addr),stFlashProtParam.RemotePort);
	}

	return true;
}

bool ICACHE_FLASH_ATTR
SaveFlashProtParam(){
	return system_param_save_with_protect(SYS_PROTECT_SECT,&stFlashProtParam,sizeof(struct FlashProtectParam));
}
