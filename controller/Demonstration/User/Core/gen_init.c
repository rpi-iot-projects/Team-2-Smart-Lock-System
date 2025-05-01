#include "aes.h"
#include "uart.h"
#include "tim.h"

extern TIM_HandleTypeDef htim7;
void general_init(){
	AES_init();
	TIM6_Init();
	TIM7_Init();
	UART_Init();
	HAL_TIM_Base_Start_IT(&htim7);
}
