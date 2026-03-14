#include "flash.h"

void Flash_Init(void)
{
    // 强制初始化FLASH，写入默认值
    HAL_FLASH_Unlock();
    
    FLASH_EraseInitTypeDef erase_init;
    erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
    erase_init.PageAddress = FLASH_NFC_ID_ADDR;
    erase_init.NbPages = 1;
    
    uint32_t page_error;
    HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&erase_init, &page_error);
    
    if(status == HAL_OK)
    {
        // 写入默认值
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_NFC_ID_ADDR, 0);
        if(status == HAL_OK)
        {
            status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_PIN_ADDR, (uint32_t)DEFAULT_PIN);
        }
    }
    
    HAL_FLASH_Lock();
}

uint32_t Flash_ReadNFCID(void)
{
    return *(uint32_t*)FLASH_NFC_ID_ADDR;
}

void Flash_WriteNFCID(uint32_t id)
{
    HAL_FLASH_Unlock();
    
    FLASH_EraseInitTypeDef erase_init;
    erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
    erase_init.PageAddress = FLASH_NFC_ID_ADDR;
    erase_init.NbPages = 1;
    
    uint32_t page_error;
    HAL_FLASHEx_Erase(&erase_init, &page_error);
    
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_NFC_ID_ADDR, id);
    
    HAL_FLASH_Lock();
}

uint16_t Flash_ReadPIN(void)
{
    return (uint16_t)*(uint32_t*)FLASH_PIN_ADDR;
}

void Flash_WritePIN(uint16_t pin)
{
    HAL_FLASH_Unlock();
    
    // 先读取当前的NFC ID
    uint32_t nfc_id = Flash_ReadNFCID();
    
    FLASH_EraseInitTypeDef erase_init;
    erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
    erase_init.PageAddress = FLASH_NFC_ID_ADDR;
    erase_init.NbPages = 1;
    
    uint32_t page_error;
    HAL_FLASHEx_Erase(&erase_init, &page_error);
    
    // 先写入NFC ID，再写入PIN
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_NFC_ID_ADDR, nfc_id);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_PIN_ADDR, (uint32_t)pin);
    
    HAL_FLASH_Lock();
}
