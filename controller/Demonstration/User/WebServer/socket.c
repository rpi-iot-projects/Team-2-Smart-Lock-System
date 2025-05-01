/**
  **********************************************************************************************************************
  * @file    webserver_http_response.c
  * @author  MCD Application Team
  * @brief   This file implements the web server http response services
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
#include "webserver_http_response.h"
#include "webserver_http_encoder.h"
#include "net_connect.h"
#include "net_interface.h"
#include "mx_wifi.h"

//#include "socket.h"
#include "main.h"
#include "lock_actions.h"
#include "aes.h"
#include "socket.h"

/* Private typedef ---------------------------------------------------------------------------------------------------*/
/* Private define ----------------------------------------------------------------------------------------------------*/
#define SERVER_PORT         (24928U)
#define SOCKET_RECEIVE_BUFFER_SIZE (1500U)
#define MAX_SOCKET_DATASIZE (1500U)

/* Private macro -----------------------------------------------------------------------------------------------------*/
/* Private variables -------------------------------------------------------------------------------------------------*/
extern uint8_t lock_password[];
extern uint8_t lock_temp_pwd[];
extern uint8_t lock_temp_pwd_status[];

int32_t sock = 0;

/* Private function prototypes ---------------------------------------------------------------------------------------*/


/* Functions prototypes ----------------------------------------------------------------------------------------------*/

/**
  * @brief  Start HTTP web server process
  * @param  None
  * @retval Web Server status
  */
WebServer_StatusTypeDef socket_server_start(void)
{
  struct net_sockaddr_in s_addr_in = {0};
  net_ip_addr_t ip_addr_in = {0};
  int32_t timeout = MX_WIFI_CMD_TIMEOUT;


  printf("\r\n*** Create TCP socket\r\n");
  if ((sock = net_socket(NET_AF_INET, NET_SOCK_STREAM, NET_IPPROTO_TCP)) < 0)
  {
    printf("*** Fail: Socket not created!\r\n");
    return SOCKET_ERROR;
  }
  printf("*** TCP socket created\r\n");

  net_setsockopt(sock, NET_SOL_SOCKET, NET_SO_BINDTODEVICE, Netif, sizeof(&Netif));
  net_setsockopt(sock, NET_SOL_SOCKET, NET_SO_RCVTIMEO, &timeout, sizeof(timeout));

  s_addr_in.sin_family = NET_AF_INET;
  s_addr_in.sin_len    = sizeof(s_addr_in);
  net_if_get_ip_address(Netif, &ip_addr_in);
  s_addr_in.sin_addr.s_addr = ip_addr_in.addr;
  net_set_port((struct net_sockaddr *)&s_addr_in, SERVER_PORT);

  if (net_bind(sock, (struct net_sockaddr *)&s_addr_in, sizeof(s_addr_in)) != 0U)
  {
    printf("*** Fail: Socket bind failed!\r\n");
    return SOCKET_ERROR;
  }
  printf("*** Socket bind successful\r\n");

  if (net_listen(sock, 1) != 0U)
  {
    printf("*** Fail: Listen failed!\r\n");
    return SOCKET_ERROR;
  }
  printf("*** Listening for raw TCP connections on %s:%" PRIu32 "\n",
         net_ntoa(&ip_addr_in), (uint32_t)NET_NTOHS(s_addr_in.sin_port));

  while (1)
  {
    struct net_sockaddr_in client_addr = {0};
    uint32_t client_len = sizeof(client_addr);
    int32_t conn = net_accept(sock, (struct net_sockaddr *)&client_addr, &client_len);

    if (conn > 0)
    {
      printf("New client connected from %s:%" PRIu32 "\n",
             net_ntoa((net_ip_addr_t *)&client_addr.sin_addr),
             (uint32_t)NET_NTOHS(client_addr.sin_port));

      handle_client(conn);  // <- raw socket handler

      net_closesocket(conn);
    }
    else
    {
      printf("*** Fail: Connection accept failed!\r\n");
    }
  }

  return WEBSERVER_OK;
}


/**
  * @brief  Start communication with client
  * @param  None
  * @retval Web Server status
*/
void handle_client(int32_t conn)
{
  uint8_t recv_buffer[16] = {0};

  while (1)
  {
    int32_t len = net_recv(conn, recv_buffer, sizeof(recv_buffer), 0);

    if (len <= 0)
    {
      printf("Client disconnected or recv error\n");
      break;
    }

    treat_command(recv_buffer);
    memset((void*)recv_buffer, 0, sizeof(recv_buffer));
  }
}


/**
  * @brief  Treat client request
  * @param  socket : connection socket
  * @retval Web Server status
  */
WebServer_StatusTypeDef treat_command(uint8_t *recv_buffer)
{
  uint8_t buffer[16];
  AES_Decrypt(recv_buffer, buffer);
  switch(buffer[2])
  {
    // open lock
    case ACTION_LOCK:
    	action_lock();
    	break;
    case ACTION_UNLOCK:
    	action_unlock(AUTH_CMD);
    	break;
    case ACTION_OPEN_CAM:
    	action_enable_camera();
		break;
    case ACTION_CLOSE_CAM:
    	action_disable_camera();
    	break;
    case ACTION_NEW_OTP:
    	action_new_temp_password(&buffer[3]);
    	break;
    case ACTION_DIS_OTP:
    	action_disable_temp_password();
    	break;
    case ACTION_NEW_PWD:
    	action_new_password(&buffer[3]);
    case ACTION_NEW_FP:
    	action_new_fp();
    	break;
    default:
    	break;
  }
  return WEBSERVER_OK;
}

/**
  * @brief  send data via socket
  * @param  socket      : connection socket
  * @param  frame       : pointer to frame to be sent
  * @param  frame_size  : size of frame to be sent
  * @retval Web Server status
  */
WebServer_StatusTypeDef socket_send(int32_t socket, uint8_t* buffer, uint32_t frame_size)
{
  uint8_t frame[16];
  AES_Encrypt(buffer, frame);

  /* Setup send information */
  uint32_t data_size = frame_size;
  uint32_t data_idx  = 0U;

  /* Check remaining data */
  while (data_size > 0U)
  {
    if (data_size >= MAX_SOCKET_DATASIZE)
    {
      /* Send data */
      if (net_send(socket, (uint8_t*)&frame[data_idx], MAX_SOCKET_DATASIZE, 0) <= 0U)
      {
        return SOCKET_ERROR;
      }

      /* Update send information */
      data_size -= MAX_SOCKET_DATASIZE;
      data_idx += MAX_SOCKET_DATASIZE;
    }
    else
    {
      /* Send data */
      if (net_send(socket, (uint8_t*)&frame[data_idx], data_size, 0) <= 0U)
      {
        return SOCKET_ERROR;
      }

      /* Update send information */
      data_size = 0U;
    }
  }

  return WEBSERVER_OK;
}
