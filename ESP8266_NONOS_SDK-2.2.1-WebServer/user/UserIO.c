#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "gpio.h"
#include "user_interface.h"
#include "eagle_soc.h"
#include "unistd.h" //sleep
#include "GlobalInfo.h"
#include "UserIO.h"

uint8 LEDMode = LED_LOW_FLASH;
LOCAL os_timer_t timerLED;
char LEDStatus = 1; //light up when low level
char TimerCounter = 0;

char RelayStatus = RELAY_OFF;

void
GPIO_INTR_Handler(void* arg){
    u32 GPIOStatus = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
    GPIO_REG_WRITE( GPIO_STATUS_W1TC_ADDRESS, GPIOStatus);//clear interrupt status
    ETS_GPIO_INTR_DISABLE();
    if ( GPIOStatus & BIT( TOUCH_PIN )){  //judge whether interrupt source is TOUCH_PIN
        //if(GPIO_INPUT_GET(TOUCH_PIN)){
        //}
    	RelayStatus = !RelayStatus;
        GPIO_OUTPUT_SET(GPIO_ID_PIN(RELAY_PIN), RelayStatus);
    }
	os_delay_us(2*1000); //max 65535us
    ETS_GPIO_INTR_ENABLE();
}

void ICACHE_FLASH_ATTR
TouchINTRInit(){
    ETS_GPIO_INTR_DISABLE();
    ETS_GPIO_INTR_ATTACH( GPIO_INTR_Handler, NULL );
    gpio_pin_intr_state_set( GPIO_ID_PIN(TOUCH_PIN),
                             GPIO_PIN_INTR_POSEDGE ); //DISABLE\POSEDGE\NEGEDGE\LOLEVEL\HILEVEL
    GPIO_REG_WRITE( GPIO_STATUS_W1TC_ADDRESS, BIT(TOUCH_PIN)); //remove the GPIO interrupt mark of this pin
    ETS_GPIO_INTR_ENABLE();
}

void ICACHE_FLASH_ATTR
LEDTimerCB(){
    os_timer_disarm(&timerLED);
    TimerCounter++;
    uint8 freq = 1;
    switch (LEDMode) {
        case LED_LOW_FLASH:
			TimerCounter %= 5;
			if(TimerCounter==0){
				LEDStatus = !LEDStatus;
			}
            break;
        case LED_FAST_FLASH:
			TimerCounter %= 2;
			if(TimerCounter==0){
				LEDStatus = !LEDStatus;
			}
			break;
		case LED_LIGHT_UP:
			TimerCounter %= 10000;  //in case overflow
			LEDStatus = 0;
			break;
		case LED_GO_OUT:
			TimerCounter %= 10000;
			LEDStatus = 1;
			break;
        default:
            break;
    }
    GPIO_OUTPUT_SET(GPIO_ID_PIN(LED_PIN), LEDStatus);
    os_timer_arm(&timerLED,100,1); //set timer interval,unit:ms
}

void ICACHE_FLASH_ATTR
LEDTimerInit(){
    os_timer_disarm(&timerLED);
    os_timer_setfn(&timerLED,(os_timer_func_t *)LEDTimerCB,NULL); //set callback func
    os_timer_arm(&timerLED,100,1); //set timer interval,unit:ms
}

void ICACHE_FLASH_ATTR
IOInit(void){
	PIN_FUNC_SELECT(RELAY_MUX,RELAY_PIN_FUNC);
	PIN_PULLUP_EN(RELAY_MUX);
	GPIO_OUTPUT_SET(GPIO_ID_PIN(RELAY_PIN), RELAY_OFF);

	PIN_FUNC_SELECT(LED_MUX,LED_PIN_FUNC);
	PIN_PULLUP_DIS(LED_MUX);
	GPIO_OUTPUT_SET(GPIO_ID_PIN(LED_PIN), 1);

#ifdef DEBUG_PRINT
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0TXD_U,FUNC_U0TXD);
#endif

	PIN_FUNC_SELECT(TOUCH_MUX,TOUCH_PIN_FUNC);
	PIN_PULLUP_EN(TOUCH_MUX);
	GPIO_DIS_OUTPUT(TOUCH_PIN);//config to input mode

    LEDTimerInit();

    TouchINTRInit();
}
