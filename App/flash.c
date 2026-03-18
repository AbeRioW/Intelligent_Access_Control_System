#include "flash.h"

//判断是否初始化
void Flash_Init(void)
{
    // 检查FLASH是否未初始化
    uint32_t pinValue = *(uint32_t*)FLASH_PIN_ADDR;
    
    if(pinValue == 0xFFFFFFFF)
    {
        // 未初始化，写入默认值
        HAL_FLASH_Unlock();
        
        FLASH_EraseInitTypeDef erase_init;
        erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
        erase_init.PageAddress = FLASH_PIN_ADDR;
        erase_init.NbPages = 1;
        
        uint32_t page_error;
        HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&erase_init, &page_error);
        
        if(status == HAL_OK)
        {
            // 写入默认值
            status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_NFC_ID_ADDR, 0);
            if(status == HAL_OK)
            {
                // 写入ASCII形式的"0000"
                status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_PIN_ADDR, 0x30303030);
            }
        }
        
        HAL_FLASH_Lock();
    }
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

void Flash_ReadPIN(uint8_t* pin)
{
    uint32_t pinValue = *(uint32_t*)FLASH_PIN_ADDR;
    // 转换为4位数组
    pin[0] = (pinValue >> 24) & 0xFF;
    pin[1] = (pinValue >> 16) & 0xFF;
    pin[2] = (pinValue >> 8) & 0xFF;
    pin[3] = pinValue & 0xFF;
}

void Flash_WritePIN(uint8_t* pin)
{
    HAL_FLASH_Unlock();
    
    // 先读取当前的NFC ID
    uint32_t nfc_id = Flash_ReadNFCID();
    
    FLASH_EraseInitTypeDef erase_init;
    erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
    erase_init.PageAddress = FLASH_PIN_ADDR;
    erase_init.NbPages = 1;
    
    uint32_t page_error;
    HAL_FLASHEx_Erase(&erase_init, &page_error);
    
    // 转换为32位值
    uint32_t pinValue = (pin[0] << 24) | (pin[1] << 16) | (pin[2] << 8) | pin[3];
    
    // 先写入NFC ID，再写入PIN
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_NFC_ID_ADDR, nfc_id);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_PIN_ADDR, pinValue);
    
    HAL_FLASH_Lock();
}
