/**
  **********************************************************************************************************************
  * @file    app_main.c
  * @author  MCD Application Team
  * @brief   App main program body
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

/* Includes ----------------------------------------------------------------------------------------------------------*/
#include "main.h"
/* Private typedef ---------------------------------------------------------------------------------------------------*/
/* Private define ----------------------------------------------------------------------------------------------------*/
/* Private macro -----------------------------------------------------------------------------------------------------*/
/* Private variables -------------------------------------------------------------------------------------------------*/
/* Private function prototypes ---------------------------------------------------------------------------------------*/

/**
  * @brief  App main program
  * @param  None
  * @retval None
  */
void app_entry(void)
{
  // No buffer for printf usage, just print characters one by one.*/
  //setbuf(stdout, NULL);

  // Initialize system
  if (system_init() != WEBSERVER_OK)
  {
    webserver_process_error();
  }


  // Initialize WiFi connection
  if (webserver_wifi_init() != WEBSERVER_OK)
  {
    webserver_process_error();
  }

  // Connect to available WiFi
  if (webserver_wifi_connect() != WEBSERVER_OK)
  {
    webserver_process_error();
  }

  general_init();

  // Start web server
  if (socket_server_start() != WEBSERVER_OK)
  {
    webserver_process_error();
  }

}
