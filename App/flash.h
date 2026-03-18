#ifndef __FLASH_H__
#define __FLASH_H__

#include "main.h"

// STM32F103 128KB FLASH, 1KB per page, total 128 pages
#define FLASH_PAGE_SIZE      0x400  // 1KB per page
#define FLASH_TOTAL_PAGES    128    // 128 pages for 128KB FLASH
#define FLASH_4TH_LAST_PAGE  (FLASH_TOTAL_PAGES - 4)  // 倒数第4页
#define FLASH_PIN_ADDR       (0x08000000 + FLASH_4TH_LAST_PAGE * FLASH_PAGE_SIZE)
#define FLASH_NFC_ID_ADDR    FLASH_PIN_ADDR + 4

#define DEFAULT_PIN          0 // 存储为数字0，但读取时转换为"0000"字符串

typedef struct {
    uint32_t nfc_id;
    uint16_t pin;
} FlashData;

void Flash_Init(void);
uint32_t Flash_ReadNFCID(void);
void Flash_WriteNFCID(uint32_t id);
void Flash_ReadPIN(uint8_t* pin);
void Flash_WritePIN(uint8_t* pin);

#endif /* __FLASH_H__ */
