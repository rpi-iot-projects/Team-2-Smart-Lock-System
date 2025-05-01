#include "main.h"
#include "fp.h"
#include "uart.h"
#include "lock_actions.h"

extern UART_HandleTypeDef huart3;
extern uint8_t uart3_rxBuffer[];

void gen_char_profile(uint8_t buffer_id)
{
	uint8_t packet[13] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF,
						  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	packet[6] = 0x01;
	packet[7] = 0x00;
	packet[8] = 0x04;
	packet[9] = 0x02;
	packet[10] = buffer_id;
	packet[11] = 0x00;
	packet[12] = 0x07 + buffer_id;
	HAL_UART_Transmit(&huart3, (uint8_t*)packet, 13, HAL_MAX_DELAY);
	if (HAL_UART_Receive(&huart3, uart3_rxBuffer, 12, HAL_MAX_DELAY) == HAL_OK){
		return;
	}
}

void poll_fp()
{
	int res = 0;
	if(genImg(2) == 1){
		// finger detected
		res = cmp_char();
		// fp match
		if (res == 1){
			action_unlock(AUTH_FP);
		}
	}
}

int genImg(uint8_t id)
{
	uint8_t packet[12] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF,
						  0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	packet[6] = 0x01;
	packet[7] = 0x00;
	packet[8] = 0x03;
	packet[9] = 0x01;
	packet[10] = 0x00;
	packet[11] = 0x05;
	HAL_UART_Transmit(&huart3, (uint8_t*)packet, 12, HAL_MAX_DELAY);
	if (HAL_UART_Receive(&huart3, uart3_rxBuffer, 12, HAL_MAX_DELAY) == HAL_OK){
		if(uart3_rxBuffer[9] == 0x00){
			gen_char_profile(id);
			return 1;
		}
	}
	return 0;
}

int cmp_char(){
	uint8_t packet[12] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF,
						  0x01, 0x00, 0x03, 0x03, 0x00, 0x07};
	HAL_UART_Transmit(&huart3, (uint8_t*)packet, 12, HAL_MAX_DELAY);
	if (HAL_UART_Receive(&huart3, uart3_rxBuffer, 13, HAL_MAX_DELAY) == HAL_OK){
		if(uart3_rxBuffer[9] == 0x00){
			return 1;
		}
		else{
			return 0;
		}
	}
	return 0;
}
