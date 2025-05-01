#include "lock_actions.h"
#include "socket.h"
#include "fp.h"

extern int32_t sock;
extern TIM_HandleTypeDef htim6;

uint8_t lock_state = 1;

uint8_t password[12];
uint8_t temp_password[12];
uint8_t temp_password_status = 0;

uint8_t report_buffer[16] = {0};


void action_lock(void)
{
	HAL_TIM_Base_Stop_IT(&htim6);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, GPIO_PIN_SET);
	uint8_t tx_data[16] = {0};
	tx_data[1] = 0x02;  //device id
	tx_data[2] = 0x02;  //locked
	socket_send(sock, tx_data, 16);
}


void action_unlock(uint8_t auth)
{
	//printf("auth access\r\n");
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, GPIO_PIN_RESET);
	uint8_t tx_data[16] = {0};
	tx_data[1] = 0x02;  //device id
	tx_data[2] = 0x01;  //unlocked
	tx_data[3] = auth;
	socket_send(sock, tx_data, 16);
	__HAL_TIM_SET_COUNTER(&htim6, 0);
	HAL_TIM_Base_Start_IT(&htim6);
}


void action_deny(uint8_t auth)
{
	uint8_t tx_data[16] = {0};
	tx_data[1] = 0x02;  //device id
	tx_data[2] = 0x01;  //denied access
	tx_data[3] = auth;
	socket_send(sock, tx_data, 16);
}


void action_new_password(uint8_t* pwd)
{
	//printf("recv new password\r\n");
	memcpy(password, pwd, 12);
}


void action_new_temp_password(uint8_t* pwd)
{
	//printf("recv new OTP\r\n");
	memcpy(temp_password, pwd, 12);
	temp_password_status = 1;
}


void action_disable_temp_password()
{
	//printf("disable OTP\r\n");
	memset(temp_password, 0, 12);
	temp_password_status = 0;
}


int compare_pwd(uint8_t *user_pwd)
{
	int res = 0;
	res = memcmp(user_pwd, password, 12);
	if(res == 0){
		return AUTH_PWD;
	}

	res = memcmp(user_pwd, temp_password, 12);
	if(res == 0){
		if(temp_password_status){
			return AUTH_TEMP_PWD;
		}
	}
	return AUTH_DENY;
}


void action_enable_camera()
{
	// printf("Enable camera\r\n");
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
}

void action_disable_camera()
{
	// printf("Disable camera\r\n");
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
}

void action_new_fp()
{
	genImg(1);
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM6)
	{
		action_lock();
	}
	else if (htim->Instance == TIM7)
	{
		poll_fp();
	}
}
