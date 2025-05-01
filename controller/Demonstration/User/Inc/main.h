/**
  **********************************************************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
  **********************************************************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  **********************************************************************************************************************
  */

/* Define to prevent recursive inclusion -----------------------------------------------------------------------------*/
#ifndef MAIN_H
#define MAIN_H

/* Includes ----------------------------------------------------------------------------------------------------------*/
#include "webserver_main.h"
#include "socket.h"
#include "stm32u5xx_hal.h"

/* Exported types ----------------------------------------------------------------------------------------------------*/
/* Exported constants ------------------------------------------------------------------------------------------------*/
/* Exported macro ----------------------------------------------------------------------------------------------------*/
#define U_WIFI_SSID "City Station"
#define U_WIFI_PWD "CityStation2022"


/* Exported functions ----------------------------------------------------------------------------------------------- */
void Error_Handler();
void LED_ON();
void LED_OFF();
void general_init();
/* Private defines ---------------------------------------------------------------------------------------------------*/


#endif /* MAIN_H */
