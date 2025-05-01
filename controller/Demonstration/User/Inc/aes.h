#ifndef AES_H
#define AES_H

#include "main.h"
#include "stm32U5xx_hal.h"
#include "stm32U5xx_hal_cryp.h"
#include "stm32U5xx_hal_rcc.h"

void AES_init(void);

void AES_Encrypt(uint8_t* plaintext, uint8_t* ciphertext);
void AES_Decrypt(uint8_t* ciphertext, uint8_t* plaintext);

#endif
