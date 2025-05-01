#ifndef LOCK_ACTIONS_H
#define LOCK_ACTIONS_H

#include "main.h"

#define ACTION_POLL		 0x00
#define ACTION_LOCK 	 0x02
#define ACTION_UNLOCK 	 0x01
#define ACTION_OPEN_CAM  0x03
#define ACTION_CLOSE_CAM 0x04
#define ACTION_NEW_OTP 	 0x06
#define ACTION_DIS_OTP 	 0x07
#define ACTION_NEW_PWD   0x05
#define ACTION_NEW_FP    0x08


// authorization type
#define AUTH_DENY       0x00
#define AUTH_CMD 		0x04
#define AUTH_PWD 		0x01
#define AUTH_TEMP_PWD 	0x02
#define AUTH_FP  		0x03


void action_lock(void);
void action_unlock(uint8_t auth);
void action_deny(uint8_t auth);
void action_new_password(uint8_t* pwd);
void action_new_temp_password(uint8_t* pwd);
void action_disable_temp_password();
void action_new_fp();
void action_enable_camera();
void action_disable_camera();

int compare_pwd(uint8_t *user_pwd);


#endif
