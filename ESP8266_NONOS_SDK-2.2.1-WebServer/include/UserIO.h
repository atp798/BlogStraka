#ifndef __USER_IO_H__
#define __USER_IO_H__

#define LED_FAST_FLASH	1
#define LED_LOW_FLASH	2
#define LED_LIGHT_UP	3
#define LED_GO_OUT		4

#ifdef DEBUG_PRINT
	#define LED_PIN     	2
	#define LED_MUX			PERIPHS_IO_MUX_GPIO2_U
	#define LED_PIN_FUNC	FUNC_GPIO2
#else
	#define LED_PIN     	1
	#define LED_PIN_FUNC	FUNC_GPIO1
	#define LED_MUX			PERIPHS_IO_MUX_U0TXD_U
#endif

#define RELAY_PIN   	0
#define RELAY_PIN_FUNC	FUNC_GPIO0
#define RELAY_MUX		PERIPHS_IO_MUX_GPIO0_U
#define RELAY_ON		0
#define RELAY_OFF		1

#define TOUCH_PIN   	3
#define TOUCH_PIN_FUNC	FUNC_GPIO3
#define TOUCH_MUX		PERIPHS_IO_MUX_U0RXD_U


void IOInit(void);
#endif
