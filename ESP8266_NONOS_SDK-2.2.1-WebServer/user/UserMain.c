#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "user_interface.h" 
#include "eagle_soc.h"
#include "unistd.h" //sleep
#include "espconn.h"
#include "mem.h"  //os_malloc
#include "GlobalInfo.h"
#include "user_config.h"
#include "driver/uart.h"
#include "WebServer.h"
#include "TCPComm.h"
#include "FlashParam.h"
#include "WifiManager.h"
#include "UserIO.h"

extern uint32 UserParamStartSect;
extern struct FlashProtectParam stFlashProtParam;

extern os_event_t *WebTaskQueue;
extern os_event_t *TCPTaskQueue;

void ICACHE_FLASH_ATTR
user_rf_pre_init(){
}

uint32 ICACHE_FLASH_ATTR
user_rf_cal_sector_set(void)
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    bool bFota;
#ifdef FOTA
    bFota = true;
#else
    bFota = false;
#endif

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            UserParamStartSect = bFota? 0x3C:0x6C;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            UserParamStartSect = bFota? 0x7C:0xCC;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
            rf_cal_sec = 512 - 5;
            UserParamStartSect = bFota? 0x7C:0xD0;
            break;
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            UserParamStartSect = bFota? 0xFC:0xD0;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
            rf_cal_sec = 1024 - 5;
            UserParamStartSect = bFota? 0x7C:0xD0;
            break;
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            UserParamStartSect = bFota? 0xFC:0xD0;
            break;

        case FLASH_SIZE_64M_MAP_1024_1024:
            rf_cal_sec = 2048 - 5;
            UserParamStartSect = bFota? 0xFC:0xD0;
            break;
        case FLASH_SIZE_128M_MAP_1024_1024:
            rf_cal_sec = 4096 - 5;
            UserParamStartSect = bFota? 0xFC:0xD0;
            break;
        default:
            rf_cal_sec = 0;
            UserParamStartSect = 0;
            break;
    }

    return rf_cal_sec;
}

void ICACHE_FLASH_ATTR
PrintInfo(void){
	os_printf("\r\n\r\nSDK  version: %s\n",system_get_sdk_version());
	os_printf("CHIP ID: %ld\n",system_get_chip_id());
	os_printf("CPU freq:%d\r\n", system_get_cpu_freq());
	os_printf("free heap size:%d\r\n", system_get_free_heap_size());
	os_printf("meminfo: ");
	system_print_meminfo();
	os_printf("\r\nshow malloc info:");
	system_show_malloc();
	uint8 macAddr[6]={0};
	wifi_get_macaddr(SOFTAP_IF,macAddr);
	//os_printf("\r\nSoftAP Mac:%x-%x-%x-%x-%x-%x\n",macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);
	os_printf("\r\nSoftAP MAC:"MACSTR"\r\n",MAC2STR(macAddr));
	wifi_get_macaddr(STATION_IF,macAddr);
	os_printf("\r\nStation MAC:"MACSTR"\r\n",MAC2STR(macAddr));

	struct station_config config;
	if(!wifi_station_get_config_default(&config)){
		os_printf("wifi station config get failed\r\n");
	}else{
		os_printf("wifi station config get\r\nssid:%s\r\npasswd:%s\r\n",config.ssid,config.password);
	}
	struct softap_config apConf;
	if(!wifi_softap_get_config_default(&apConf)){
		os_printf("wifi softap config get failed\r\n");
	}else{
		os_printf("wifi softap config get\r\nssid:%s\r\npasswd:%s\r\n",apConf.ssid,apConf.password);
	}

	os_printf("system init successfully\r\n");
	os_printf("*********************************\r\n");
}

void ICACHE_FLASH_ATTR 
InitDoneCB(void){

    WebTaskQueue = (os_event_t *)os_malloc(sizeof(os_event_t)*WEBTASK_QUEUE_LEN);
    TCPTaskQueue = (os_event_t *)os_malloc(sizeof(os_event_t)*TCPTASK_QUEUE_LEN);
	system_os_task(WebServTask,WEBSERV_TASK_PRIO,WebTaskQueue,WEBTASK_QUEUE_LEN);
	system_os_task(TCPCommTask,TCPCOMM_TASK_PRIO,TCPTaskQueue,TCPTASK_QUEUE_LEN);

	WifiInitConfig();

	if((stFlashProtParam.WorkStatus & WEB_SERV_BIT)||!(stFlashProtParam.WorkStatus & REMOTE_SERV_CONF_BIT)){
		WebServInit(WEB_SERV_PORT);
	}
	if(stFlashProtParam.WorkStatus & TCP_SERV_BIT){
		system_os_post(TCPCOMM_TASK_PRIO,TSIG_REMOTE_SERVCHG,0x00);
	}
	if(stFlashProtParam.WorkStatus & MQTT_CLIENT_BIT ){
	}

	PrintInfo();
}

void ICACHE_FLASH_ATTR
user_init(void)
{
	uart_div_modify(UART0,UART_CLK_FREQ/BIT_RATE_115200);
	os_delay_us(60*1000); //max 65535us

	system_init_done_cb(InitDoneCB);

	IOInit();

    LoadFlashProtParam();

	wifi_station_set_reconnect_policy(true);//set whether retry connect when line break

	if(!spi_flash_erase_protect_enable()){
		os_printf("spi flash erase protect enable failed\r\n");
	}
}
