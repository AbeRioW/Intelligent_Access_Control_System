#ifndef __FLASH_H__
#define __FLASH_H__

#include "main.h"

#define FLASH_START_ADDR     0x0800FC00  // FLASH扇区末尾地址
#define FLASH_NFC_ID_ADDR    FLASH_START_ADDR
#define FLASH_PIN_ADDR       FLASH_START_ADDR + 4

#define DEFAULT_PIN          0

typedef struct {
    uint32_t nfc_id;
    uint16_t pin;
} FlashData;

void Flash_Init(void);
uint32_t Flash_ReadNFCID(void);
void Flash_WriteNFCID(uint32_t id);
uint16_t Flash_ReadPIN(void);
void Flash_WritePIN(uint16_t pin);

#endif /* __FLASH_H__ */
