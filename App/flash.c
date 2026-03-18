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
        
        // 擦除倒数第4页（存储PIN码和NFC ID）
        FLASH_EraseInitTypeDef erase_init;
        erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
        erase_init.PageAddress = FLASH_PIN_ADDR;
        erase_init.NbPages = 1;
        
        uint32_t page_error;
        HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&erase_init, &page_error);
        
        if(status == HAL_OK)
        {
            // 写入ASCII形式的"0000"
            status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_PIN_ADDR, 0x30303030);
        }
        
        // 擦除倒数第3页（存储NFC卡片ID列表）
        erase_init.PageAddress = FLASH_NFC_IDS_ADDR;
        status = HAL_FLASHEx_Erase(&erase_init, &page_error);
        
        HAL_FLASH_Lock();
    }
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
    
    FLASH_EraseInitTypeDef erase_init;
    erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
    erase_init.PageAddress = FLASH_PIN_ADDR;
    erase_init.NbPages = 1;
    
    uint32_t page_error;
    HAL_FLASHEx_Erase(&erase_init, &page_error);
    
    // 转换为32位值
    uint32_t pinValue = (pin[0] << 24) | (pin[1] << 16) | (pin[2] << 8) | pin[3];
    
    // 写入PIN
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_PIN_ADDR, pinValue);
    
    HAL_FLASH_Lock();
}

// 读取所有存储的NFC卡片ID
// 返回值：实际存储的ID数量
uint8_t Flash_ReadNFCIDs(uint32_t* ids)
{
    uint8_t count = 0;
    for(uint8_t i = 0; i < MAX_NFC_IDS; i++)
    {
        uint32_t id = *(uint32_t*)(FLASH_NFC_IDS_ADDR + i * 4);
        if(id != 0xFFFFFFFF)
        {
            ids[count] = id;
            count++;
        }
    }
    return count;
}

// 添加新的NFC卡片ID
// 返回值：0-成功，1-已满，2-已存在，3-无效ID
uint8_t Flash_AddNFCID(uint32_t id)
{
    // 检查是否为无效ID
    if(id == 0)
    {
        return 3; // 无效ID
    }
    
    // 检查是否已存在
    uint32_t existingIds[MAX_NFC_IDS];
    uint8_t count = Flash_ReadNFCIDs(existingIds);
    
    for(uint8_t i = 0; i < count; i++)
    {
        if(existingIds[i] == id)
        {
            return 2; // 已存在
        }
    }
    
    if(count >= MAX_NFC_IDS)
    {
        return 1; // 已满
    }
    
    // 找到第一个空位置
    uint8_t emptyIndex = count;
    
    HAL_FLASH_Unlock();
    
    // 写入新ID
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_NFC_IDS_ADDR + emptyIndex * 4, id);
    
    HAL_FLASH_Lock();
    
    return 0; // 成功
}

// 删除指定的NFC卡片ID
// 返回值：0-成功，1-未找到，2-无效ID
uint8_t Flash_RemoveNFCID(uint32_t id)
{
    // 检查是否为无效ID
    if(id == 0)
    {
        return 2; // 无效ID
    }
    
    // 读取所有ID
    uint32_t ids[MAX_NFC_IDS];
    uint8_t count = Flash_ReadNFCIDs(ids);
    
    // 查找要删除的ID
    uint8_t foundIndex = MAX_NFC_IDS;
    for(uint8_t i = 0; i < count; i++)
    {
        if(ids[i] == id)
        {
            foundIndex = i;
            break;
        }
    }
    
    if(foundIndex == MAX_NFC_IDS)
    {
        return 1; // 未找到
    }
    
    // 重排ID列表
    for(uint8_t i = foundIndex; i < count - 1; i++)
    {
        ids[i] = ids[i + 1];
    }
    
    HAL_FLASH_Unlock();
    
    // 擦除整个页面
    FLASH_EraseInitTypeDef erase_init;
    erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
    erase_init.PageAddress = FLASH_NFC_IDS_ADDR;
    erase_init.NbPages = 1;
    
    uint32_t page_error;
    HAL_FLASHEx_Erase(&erase_init, &page_error);
    
    // 重新写入ID列表
    for(uint8_t i = 0; i < count - 1; i++)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_NFC_IDS_ADDR + i * 4, ids[i]);
    }
    
    HAL_FLASH_Lock();
    
    return 0; // 成功
}
