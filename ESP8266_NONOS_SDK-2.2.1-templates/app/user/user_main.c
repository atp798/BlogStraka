#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "user_interface.h"

LOCAL os_timer_t timer;
char ledStatus = 1; 

void timer_cb(){
    GPIO_OUTPUT_SET(GPIO_ID_PIN(2), ledStatus);
    ledStatus = !ledStatus;
}

void ICACHE_FLASH_ATTR 
user_rf_pre_init(){
}

uint32	ICACHE_FLASH_ATTR 
user_rf_cal_sector_set(void){
}

void ICACHE_FLASH_ATTR 
user_init(void)
{
     PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U,FUNC_GPIO2);
     os_timer_disarm(&timer);
     os_timer_setfn(&timer,(os_timer_func_t *)timer_cb,NULL);
     os_timer_arm(&timer,1000,1);
}
