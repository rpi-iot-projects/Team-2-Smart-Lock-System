#include "aes.h"

CRYP_HandleTypeDef hcryp;
CRYP_ConfigTypeDef config;


uint32_t pKeyAES[4] = {0x2b7e1516, 0x28aed2a6, 0xabf71588, 0x09cf4f3c};

void AES_init(void)
{
  __HAL_RCC_AES_CLK_ENABLE();
  hcryp.Instance = AES;
  hcryp.Init.DataType = CRYP_NO_SWAP;
  hcryp.Init.KeySize = CRYP_KEYSIZE_128B;
  hcryp.Init.pKey = (uint32_t *)pKeyAES;
  hcryp.Init.Algorithm = CRYP_AES_ECB;
  hcryp.Init.DataWidthUnit = CRYP_DATAWIDTHUNIT_WORD;
  hcryp.Init.HeaderWidthUnit = CRYP_HEADERWIDTHUNIT_WORD;
  hcryp.Init.KeyIVConfigSkip = CRYP_KEYIVCONFIG_ALWAYS;
  hcryp.Init.KeyMode = CRYP_KEYMODE_NORMAL;
  if (HAL_CRYP_Init(&hcryp) != HAL_OK)
  {
	  //Error_Handler();
  }

}


void AES_Encrypt(uint8_t* plaintext, uint8_t* ciphertext) {
  if (HAL_CRYP_Encrypt(&hcryp, (uint32_t *)plaintext, 16, (uint32_t *)ciphertext, HAL_MAX_DELAY) != HAL_OK) {
    // Encryption Error
    //Error_Handler();
  }
}

void AES_Decrypt(uint8_t* ciphertext, uint8_t* plaintext) {
  if (HAL_CRYP_Decrypt(&hcryp, (uint32_t *)ciphertext, 16, (uint32_t *)plaintext, HAL_MAX_DELAY) != HAL_OK) {
    // Decryption Error
    //Error_Handler();
  }
}
