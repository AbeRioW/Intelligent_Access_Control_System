/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
#include "Matrix_Keyboard.h"
#include "RC522.h"
#include "flash.h"
#include "ui.h"
#include <string.h>
#include "beep_lay.h"
#include "AS608.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum {
    MODE_MAIN,
    MODE_PIN,
    MODE_PIN_SETTING,
    MODE_PIN_CONFIRM,
    MODE_NFC_REGISTER,
    MODE_NFC_UNREGISTER,
    MODE_FINGER_REGISTER,
    MODE_FINGER_UNREGISTER
} SystemMode;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MAX_NFC_RETRY 3
#define MAX_PIN_RETRY 3
#define RELAY_ON_TIME 5000
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
SystemMode currentMode = MODE_MAIN;
uint8_t nfcRetryCount = 0;
uint8_t pinRetryCount = 0;
uint8_t oldPinRetryCount = 0;
uint8_t currentPin[4];
uint8_t pinIndex = 0;
uint8_t newPin[4];
uint8_t confirmPin[4];
uint8_t isPinSettingMode = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void ClearPIN(void);
void CheckPIN(void);
void CheckPinSetting(void);
void SaveNewPIN(void);
uint8_t KeyToDigit(uint8_t key);
uint32_t ReadNFCID(void);
void RegisterNFC(void);
void UnregisterNFC(void);
void RegisterFingerprint(void);
void UnregisterFingerprint(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void ClearPIN(void)
{
    memset(currentPin, 0, sizeof(currentPin));
    memset(newPin, 0, sizeof(newPin));
    memset(confirmPin, 0, sizeof(confirmPin));
    pinIndex = 0;
    OLED_ShowString(32, 8, (uint8_t*)"    ", 8, 1);
    OLED_Refresh();
}

void welcome_with_pin(uint8_t* pin) {
    OLED_ShowString(10, 0, (uint8_t*)"Welcome", 8, 1);
    char pinStr[5];
    sprintf(pinStr, "%d%d%d%d", pin[0], pin[1], pin[2], pin[3]);
    OLED_ShowString(0, 16, (uint8_t*)pinStr, 8, 1);
    OLED_ShowString(0, 32, (uint8_t*)"Press KEY16 for PIN", 8, 1);
    OLED_Refresh();
}

// 读取NFC卡片ID
// 返回值：卡片ID，0表示没有卡片靠近
uint32_t ReadNFCID(void)
{
    uint8_t cardType[2];
    uint8_t cardID[4];
    char status;
    
    // 请求卡片
    status = PCD_Request(PICC_REQIDL, cardType);
    if(status != PCD_OK)
    {
        return 0; // 没有卡片靠近
    }
    
    // 防碰撞，读取卡片ID
    status = PCD_Anticoll(cardID);
    if(status != PCD_OK)
    {
        return 0; // 读取卡片ID失败
    }
    
    // 将4字节的卡片ID转换为32位整数
    uint32_t nfcId = (cardID[0] << 24) | (cardID[1] << 16) | (cardID[2] << 8) | cardID[3];
    
    return nfcId;
}

// 注册NFC卡片
void RegisterNFC(void)
{
    uint32_t nfcId = 0;
    uint32_t startTime = HAL_GetTick();
    uint32_t timeout = 10000; // 10秒超时
    
    // 等待用户放置卡片，最多等待10秒
    while((HAL_GetTick() - startTime) < timeout)
    {
        nfcId = ReadNFCID();
        if(nfcId != 0)
        {
            break; // 读取到卡片ID，退出循环
        }
        HAL_Delay(100); // 短暂延迟，避免频繁读取
    }
    
    if(nfcId == 0)
    {
        // 超时，没有卡片靠近
        OLED_ShowString(0, 24, (uint8_t*)"Timeout!", 8, 1);
        OLED_Refresh();
        HAL_Delay(1000);
    } else {
        // 尝试添加到FLASH
        uint8_t result = Flash_AddNFCID(nfcId);
        
        if(result == 0)
        {
            // 注册成功
            OLED_ShowString(0, 24, (uint8_t*)"Registered!", 8, 1);
            OLED_Refresh();
            HAL_Delay(1000);
        } else if(result == 1)
        {
            // 已满
            OLED_ShowString(0, 24, (uint8_t*)"Full!", 8, 1);
            OLED_Refresh();
            HAL_Delay(1000);
        } else if(result == 2)
        {
            // 已存在
            OLED_ShowString(0, 24, (uint8_t*)"Exists!", 8, 1);
            OLED_Refresh();
            HAL_Delay(1000);
        }
    }
    
    // 完全清除OLED屏幕
    OLED_Clear();
    OLED_Refresh();
    
    // 重新绘制主页面内容（welcome页面）
    welcome();
    
    // 返回主页面
    currentMode = MODE_MAIN;
}

// 注销NFC卡片
void UnregisterNFC(void)
{
    uint32_t nfcId = 0;
    uint32_t startTime = HAL_GetTick();
    uint32_t timeout = 10000; // 10秒超时
    
    // 等待用户放置卡片，最多等待10秒
    while((HAL_GetTick() - startTime) < timeout)
    {
        nfcId = ReadNFCID();
        if(nfcId != 0)
        {
            break; // 读取到卡片ID，退出循环
        }
        HAL_Delay(100); // 短暂延迟，避免频繁读取
    }
    
    if(nfcId == 0)
    {
        // 超时，没有卡片靠近
        OLED_ShowString(0, 24, (uint8_t*)"Timeout!", 8, 1);
        OLED_Refresh();
        HAL_Delay(1000);
    } else {
        // 尝试从FLASH中删除
        uint8_t result = Flash_RemoveNFCID(nfcId);
        
        if(result == 0)
        {
            // 注销成功
            OLED_ShowString(0, 24, (uint8_t*)"Unregistered!", 8, 1);
            OLED_Refresh();
            HAL_Delay(1000);
        } else if(result == 1)
        {
            // 未找到
            OLED_ShowString(0, 24, (uint8_t*)"Not found!", 8, 1);
            OLED_Refresh();
            HAL_Delay(1000);
        }
    }
    
    // 完全清除OLED屏幕
    OLED_Clear();
    OLED_Refresh();
    
    // 重新绘制主页面内容（welcome页面）
    welcome();
    
    // 返回主页面
    currentMode = MODE_MAIN;
}

// 注册指纹
void RegisterFingerprint(void)
{
    uint8_t result;
    uint8_t processnum = 0;
    uint8_t i = 0;
    uint16_t pageID = 0;
    uint8_t found = 0;
    uint16_t score = 0;
    uint16_t tempPageID = 0;
    
    while(1)
    {
        switch(processnum)
        {
            case 0:
                // 清除显示
                OLED_ShowString(0, 0, (uint8_t*)"                ", 8, 1);
                OLED_ShowString(0, 8, (uint8_t*)"                ", 8, 1);
                OLED_ShowString(0, 16, (uint8_t*)"                ", 8, 1);
                OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                OLED_Refresh();
                HAL_Delay(500); // 增加延迟时间
                
                i++;
                OLED_ShowString(0, 0, (uint8_t*)"Place finger", 8, 1);
                OLED_ShowString(0, 8, (uint8_t*)"to register", 8, 1);
                OLED_ShowString(0, 16, (uint8_t*)"First scan", 8, 1);
                OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                OLED_Refresh();
                HAL_Delay(1000); // 增加延迟时间
                
                // 采集指纹图像
                result = AS608_GetImage();
                if(result == AS608_ACK_OK)
                {
                    // 生成特征
                    result = AS608_GenChar(AS608_BUFFER_CHAR1);
                    if(result == AS608_ACK_OK)
                    {
                        OLED_ShowString(0, 16, (uint8_t*)"First ok!        ", 8, 1);
                        OLED_Refresh();
                        HAL_Delay(1500); // 增加延迟时间
                        i = 0;
                        processnum = 1; // 进入第二步
                    }
                    else
                    {
                        OLED_ShowString(0, 24, (uint8_t*)"Gen failed!      ", 8, 1);
                        OLED_Refresh();
                        HAL_Delay(2000); // 增加延迟时间
                    }
                }
                else
                {
                    if(i % 5 == 0)
                    {
                        OLED_ShowString(0, 24, (uint8_t*)"No finger!       ", 8, 1);
                        OLED_Refresh();
                        HAL_Delay(1000); // 增加延迟时间
                        OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                        OLED_Refresh();
                    }
                }
                break;
                
            case 1:
                i++;
                OLED_ShowString(0, 16, (uint8_t*)"Place again      ", 8, 1);
                OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                OLED_Refresh();
                HAL_Delay(1000); // 增加延迟时间
                
                // 第二次采集指纹
                result = AS608_GetImage();
                if(result == AS608_ACK_OK)
                {
                    // 生成第二个特征
                    result = AS608_GenChar(AS608_BUFFER_CHAR2);
                    if(result == AS608_ACK_OK)
                    {
                        OLED_ShowString(0, 16, (uint8_t*)"Second ok!       ", 8, 1);
                        OLED_Refresh();
                        HAL_Delay(1500); // 增加延迟时间
                        i = 0;
                        processnum = 2; // 进入第三步
                    }
                    else
                    {
                        OLED_ShowString(0, 24, (uint8_t*)"Gen failed!      ", 8, 1);
                        OLED_Refresh();
                        HAL_Delay(2000); // 增加延迟时间
                    }
                }
                else
                {
                    if(i % 5 == 0)
                    {
                        OLED_ShowString(0, 24, (uint8_t*)"No finger!       ", 8, 1);
                        OLED_Refresh();
                        HAL_Delay(1000); // 增加延迟时间
                        OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                        OLED_Refresh();
                    }
                }
                break;
                
            case 2:
                // 比较两个特征
                OLED_ShowString(0, 16, (uint8_t*)"Matching...      ", 8, 1);
                OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                OLED_Refresh();
                HAL_Delay(1000); // 增加延迟时间
                
                result = AS608_Match(&score);
                if(result == AS608_ACK_OK)
                {
                    OLED_ShowString(0, 16, (uint8_t*)"Matched!         ", 8, 1);
                    OLED_Refresh();
                    HAL_Delay(1500); // 增加延迟时间
                    processnum = 3; // 进入第四步
                }
                else
                {
                    OLED_ShowString(0, 24, (uint8_t*)"Not same!        ", 8, 1);
                    OLED_Refresh();
                    HAL_Delay(2000); // 增加延迟时间
                    processnum = 0; // 重新开始
                }
                break;
                
            case 3:
                // 合并特征
                OLED_ShowString(0, 16, (uint8_t*)"Processing...    ", 8, 1);
                OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                OLED_Refresh();
                HAL_Delay(1000); // 增加延迟时间
                
                result = AS608_RegModel();
                if(result == AS608_ACK_OK)
                {
                    OLED_ShowString(0, 16, (uint8_t*)"Merged!          ", 8, 1);
                    OLED_Refresh();
                    HAL_Delay(1500); // 增加延迟时间
                    processnum = 4; // 进入第五步
                }
                else
                {
                    OLED_ShowString(0, 24, (uint8_t*)"Merge failed!    ", 8, 1);
                    OLED_Refresh();
                    HAL_Delay(2000); // 增加延迟时间
                    processnum = 0; // 重新开始
                }
                break;
                
            case 4:
                // 查找可用的ID
                OLED_ShowString(0, 16, (uint8_t*)"Finding ID...    ", 8, 1);
                OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                OLED_Refresh();
                HAL_Delay(1000); // 增加延迟时间
                
                // 逐个查找可用ID，从0开始
                pageID = 0;
                found = 0;
                
                // 先检查指纹库是否为空
                uint16_t score = 0;
                uint16_t tempID = 0;
                result = AS608_Search(AS608_BUFFER_CHAR1, 0, AS608_MAX_FINGER_NUM, &tempID, &score);
                
                if(result == AS608_ACK_NO_FOUND)
                {
                    // 指纹库为空，直接使用ID 0
                    pageID = 0;
                    found = 1;
                }
                else
                {
                    // 指纹库不为空，逐个查找可用ID
                    while(pageID < AS608_MAX_FINGER_NUM)
                    {
                        // 尝试加载指定ID的指纹
                        result = AS608_LoadChar(AS608_BUFFER_CHAR1, pageID);
                        if(result != AS608_ACK_OK)
                        {
                            // 该ID不存在，可用
                            found = 1;
                            break;
                        }
                        pageID++;
                    }
                    
                    if(!found)
                    {
                        // 如果没找到可用ID，从0开始重新查找
                        pageID = 0;
                        found = 1;
                    }
                }
                
                // 显示找到的ID
                char id_str[16];
                sprintf(id_str, "ID:%d            ", pageID);
                OLED_ShowString(0, 24, (uint8_t*)id_str, 8, 1);
                OLED_Refresh();
                HAL_Delay(1000); // 增加延迟时间
                
                processnum = 5; // 进入第六步
                break;
                
            case 5:
                // 存储指纹
                OLED_ShowString(0, 16, (uint8_t*)"Storing...       ", 8, 1);
                OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                OLED_Refresh();
                HAL_Delay(1000); // 增加延迟时间
                
                result = AS608_StoreChar(AS608_BUFFER_CHAR2, pageID);
                if(result == AS608_ACK_OK)
                {
                    // 清除旧内容
                    OLED_ShowString(0, 16, (uint8_t*)"                ", 8, 1);
                    OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                    OLED_Refresh();
                    
                    // 显示注册成功信息
                    OLED_ShowString(0, 0, (uint8_t*)"Registered!      ", 8, 1);
                    char id_str[16];
                    sprintf(id_str, "ID:%d            ", pageID);
                    OLED_ShowString(0, 8, (uint8_t*)id_str, 8, 1);
                    OLED_ShowString(0, 16, (uint8_t*)"Success!         ", 8, 1);
                    OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                    OLED_Refresh();
                    HAL_Delay(2500); // 增加延迟时间，确保用户能看到
                    goto cleanup;
                }
                else
                {
                    OLED_ShowString(0, 24, (uint8_t*)"Store failed!    ", 8, 1);
                    OLED_Refresh();
                    HAL_Delay(2000); // 增加延迟时间
                    processnum = 0; // 重新开始
                }
                break;
        }
        HAL_Delay(100);
    }

cleanup:
    // 完全清除OLED屏幕
    OLED_Clear();
    OLED_Refresh();
    
    // 重新绘制主页面内容（welcome页面）
    welcome();
    
    // 返回主页面
    currentMode = MODE_MAIN;
}

void DeleteFingerprint(void)
{
    uint8_t result;
    uint8_t processnum = 0;
    uint8_t i = 0;
    uint16_t fingerID = 0;
    uint16_t score = 0;
    
    while(1)
    {
        switch(processnum)
        {
            case 0:
                // 清除显示
                OLED_ShowString(0, 0, (uint8_t*)"                ", 8, 1);
                OLED_ShowString(0, 8, (uint8_t*)"                ", 8, 1);
                OLED_ShowString(0, 16, (uint8_t*)"                ", 8, 1);
                OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                OLED_Refresh();
                HAL_Delay(500); // 增加延迟时间
                
                i++;
                OLED_ShowString(0, 0, (uint8_t*)"Place finger", 8, 1);
                OLED_ShowString(0, 8, (uint8_t*)"to delete", 8, 1);
                OLED_ShowString(0, 16, (uint8_t*)"Scan finger", 8, 1);
                OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                OLED_Refresh();
                HAL_Delay(1000); // 增加延迟时间
                
                // 采集指纹图像
                result = AS608_GetImage();
                if(result == AS608_ACK_OK)
                {
                    // 生成特征
                    result = AS608_GenChar(AS608_BUFFER_CHAR1);
                    if(result == AS608_ACK_OK)
                    {
                        OLED_ShowString(0, 16, (uint8_t*)"Finger scanned!  ", 8, 1);
                        OLED_Refresh();
                        HAL_Delay(1500); // 增加延迟时间
                        i = 0;
                        processnum = 1; // 进入第二步
                    }
                    else
                    {
                        OLED_ShowString(0, 24, (uint8_t*)"Gen failed!      ", 8, 1);
                        OLED_Refresh();
                        HAL_Delay(2000); // 增加延迟时间
                    }
                }
                else
                {
                    if(i % 5 == 0)
                    {
                        OLED_ShowString(0, 24, (uint8_t*)"No finger!       ", 8, 1);
                        OLED_Refresh();
                        HAL_Delay(1000); // 增加延迟时间
                        OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                        OLED_Refresh();
                    }
                }
                break;
                
            case 1:
                // 搜索指纹
                OLED_ShowString(0, 16, (uint8_t*)"Searching...     ", 8, 1);
                OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                OLED_Refresh();
                HAL_Delay(1000); // 增加延迟时间
                
                result = AS608_Search(AS608_BUFFER_CHAR1, 0, AS608_MAX_FINGER_NUM, &fingerID, &score);
                if(result == AS608_ACK_OK)
                {
                    // 找到指纹，显示ID
                    char id_str[16];
                    sprintf(id_str, "Found ID:%d       ", fingerID);
                    OLED_ShowString(0, 16, (uint8_t*)id_str, 8, 1);
                    OLED_Refresh();
                    HAL_Delay(1500); // 增加延迟时间
                    processnum = 2; // 进入第三步
                }
                else if(result == AS608_ACK_NO_FOUND)
                {
                    OLED_ShowString(0, 24, (uint8_t*)"Not found!       ", 8, 1);
                    OLED_Refresh();
                    HAL_Delay(2000); // 增加延迟时间
                    goto cleanup;
                }
                else
                {
                    OLED_ShowString(0, 24, (uint8_t*)"Search failed!   ", 8, 1);
                    OLED_Refresh();
                    HAL_Delay(2000); // 增加延迟时间
                    goto cleanup;
                }
                break;
                
            case 2:
                // 删除指纹
                OLED_ShowString(0, 16, (uint8_t*)"Deleting...      ", 8, 1);
                OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                OLED_Refresh();
                HAL_Delay(1000); // 增加延迟时间
                
                result = AS608_DeleteChar(fingerID, 1);
                if(result == AS608_ACK_OK)
                {
                    // 清除旧内容
                    OLED_ShowString(0, 16, (uint8_t*)"                ", 8, 1);
                    OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                    OLED_Refresh();
                    
                    // 显示删除成功信息
                    OLED_ShowString(0, 0, (uint8_t*)"Deleted!         ", 8, 1);
                    char id_str[16];
                    sprintf(id_str, "ID:%d            ", fingerID);
                    OLED_ShowString(0, 8, (uint8_t*)id_str, 8, 1);
                    OLED_ShowString(0, 16, (uint8_t*)"Success!         ", 8, 1);
                    OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                    OLED_Refresh();
                    HAL_Delay(1000); // 增加延迟时间，确保用户能看到
                    
                    // 再次搜索指纹，确认删除成功
                    OLED_ShowString(0, 16, (uint8_t*)"Verifying...     ", 8, 1);
                    OLED_Refresh();
                    HAL_Delay(1000);
                    
                    // 重新采集指纹图像
                    result = AS608_GetImage();
                    if(result == AS608_ACK_OK)
                    {
                        // 生成特征
                        result = AS608_GenChar(AS608_BUFFER_CHAR1);
                        if(result == AS608_ACK_OK)
                        {
                            // 搜索指纹
                            uint16_t verifyID = 0;
                            uint16_t verifyScore = 0;
                            result = AS608_Search(AS608_BUFFER_CHAR1, 0, AS608_MAX_FINGER_NUM, &verifyID, &verifyScore);
                            if(result == AS608_ACK_NO_FOUND)
                            {
                                OLED_ShowString(0, 16, (uint8_t*)"Delete confirmed!", 8, 1);
                                OLED_Refresh();
                                HAL_Delay(1500);
                            }
                            else
                            {
                                OLED_ShowString(0, 16, (uint8_t*)"Delete failed!   ", 8, 1);
                                OLED_Refresh();
                                HAL_Delay(1500);
                            }
                        }
                    }
                    
                    goto cleanup;
                }
                else
                {
                    OLED_ShowString(0, 24, (uint8_t*)"Delete failed!   ", 8, 1);
                    OLED_Refresh();
                    HAL_Delay(2000); // 增加延迟时间
                    goto cleanup;
                }
                break;
        }
        HAL_Delay(100);
    }

cleanup:
    // 完全清除OLED屏幕
    OLED_Clear();
    OLED_Refresh();
    
    // 重新绘制主页面内容（welcome页面）
    welcome();
    
    // 返回主页面
    currentMode = MODE_MAIN;
}

// 注销指纹
void UnregisterFingerprint(void)
{
    OLED_ShowString(0, 0, (uint8_t*)"Place finger", 8, 1);
    OLED_ShowString(0, 8, (uint8_t*)"to unregister", 8, 1);
    OLED_ShowString(0, 16, (uint8_t*)"                ", 8, 1);
    OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
    OLED_Refresh();
    
    // 显示正在验证指纹
    OLED_ShowString(0, 16, (uint8_t*)"Verifying...", 8, 1);
    OLED_Refresh();
    
    // 等待用户放置指纹
    uint16_t pageID = 0;
    uint16_t score = 0;
    uint8_t result = AS608_VerifyFinger(&pageID, &score);
    
    // 清除验证状态显示
    OLED_ShowString(0, 16, (uint8_t*)"                ", 8, 1);
    OLED_Refresh();
    
    if(result == AS608_ACK_OK)
    {
        // 显示正在删除
        OLED_ShowString(0, 16, (uint8_t*)"Deleting...", 8, 1);
        OLED_Refresh();
        
        // 找到指纹，删除它
        result = AS608_DeleteChar(pageID, 1);
        
        // 清除删除状态显示
        OLED_ShowString(0, 16, (uint8_t*)"                ", 8, 1);
        OLED_Refresh();
        
        if(result == AS608_ACK_OK)
        {
            // 注销成功
            OLED_ShowString(0, 24, (uint8_t*)"Unregistered!", 8, 1);
            OLED_Refresh();
            HAL_Delay(1000);
        } else {
            // 注销失败
            OLED_ShowString(0, 24, (uint8_t*)"Failed!", 8, 1);
            OLED_Refresh();
            HAL_Delay(1000);
        }
    } else {
        // 未找到指纹
        OLED_ShowString(0, 24, (uint8_t*)"Not found!", 8, 1);
        OLED_Refresh();
        HAL_Delay(1000);
    }
    
    // 完全清除OLED屏幕
    OLED_Clear();
    OLED_Refresh();
    
    // 重新绘制主页面内容（welcome页面）
    welcome();
    
    // 返回主页面
    currentMode = MODE_MAIN;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART3_UART_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
	Relay_Off();
  OLED_Init();
  PCD_Init();
  Flash_Init();
  AS608_Init();
  
  // 检测AS608握手
  OLED_ShowString(0, 0, (uint8_t*)"Checking AS608...", 8, 1);
  OLED_Refresh();
  
  // 尝试握手
  uint32_t as608_addr = 0xffffffff;
  uint8_t handshake_result = AS608_HandShake(&as608_addr);
  
  if(handshake_result == AS608_ACK_OK) {
      OLED_ShowString(0, 16, (uint8_t*)"Handshake OK!", 8, 1);
      // 显示设备地址
      char addr_str[17];
      sprintf(addr_str, "Addr: %08X", as608_addr);
      OLED_ShowString(0, 24, (uint8_t*)addr_str, 8, 1);
  } else {
      OLED_ShowString(0, 16, (uint8_t*)"Handshake Fail!", 8, 1);
      // 显示错误码
      char error_str[16];
      sprintf(error_str, "Error: %02X", handshake_result);
      OLED_ShowString(0, 24, (uint8_t*)error_str, 8, 1);
  }
  OLED_Refresh();
  HAL_Delay(2000);
	OLED_Clear();
  
  welcome();
   
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    // 主页面NFC卡片检测
    if(currentMode == MODE_MAIN) {
        uint32_t nfcId = ReadNFCID();
        if(nfcId != 0) {
            // 读取FLASH中存储的NFC ID
            uint32_t nfcIds[MAX_NFC_IDS];
            uint8_t count = Flash_ReadNFCIDs(nfcIds);
            
            // 检查卡片ID是否在FLASH中
            uint8_t found = 0;
            for(uint8_t i = 0; i < count; i++) {
                if(nfcIds[i] == nfcId) {
                    found = 1;
                    break;
                }
            }
            
            if(found) {
                // 卡片已注册，拉高LAY
                Relay_On();
                OLED_ShowString(0, 24, (uint8_t*)"Success!", 8, 1);
                OLED_Refresh();
                HAL_Delay(RELAY_ON_TIME);
                // 关闭LAY
                Relay_Off();
                OLED_ShowString(0, 24, (uint8_t*)"        ", 8, 1);
                OLED_Refresh();
            }
        }
        
        // 主页面指纹检测（不使用PS_STA引脚）
        static uint32_t lastFingerCheckTime = 0;
        uint32_t currentTime = HAL_GetTick();
        
        // 每500ms检测一次指纹，避免频繁检测影响性能
        if(currentTime - lastFingerCheckTime >= 500) {
            lastFingerCheckTime = currentTime;
            
            uint16_t fingerID = 0;
            uint16_t score = 0;
            uint8_t fingerResult = AS608_VerifyFinger(&fingerID, &score);
            
            // 只处理成功的情况，忽略其他情况（如无指纹）
            if(fingerResult == AS608_ACK_OK) {
                // 指纹已注册，拉高LAY
                Relay_On();
                OLED_ShowString(0, 24, (uint8_t*)"Success!", 8, 1);
                OLED_Refresh();
                HAL_Delay(RELAY_ON_TIME);
                // 关闭LAY
                Relay_Off();
                OLED_ShowString(0, 24, (uint8_t*)"        ", 8, 1);
                OLED_Refresh();
            } else if(fingerResult == AS608_ACK_NO_FINGER) {
                // 无指纹，不显示任何信息
            } else if(fingerResult == AS608_ACK_NO_FOUND) {
                // 指纹未找到，不显示任何信息
            } else {
                // 其他错误，不显示任何信息
            }
        }
    }
    
    uint8_t key = Matrix_Keyboard_Scan(); 
    if(key != 0) {
        switch(currentMode) {
            case MODE_MAIN:
                if(key == 16) {
                    // 进入PIN输入模式
                    currentMode = MODE_PIN;
                    pinIndex = 0;
                    memset(currentPin, 0, sizeof(currentPin));
                    UI_ShowPINInput();
                } else if(key == 8) {
                    // 进入PIN设置模式
                    isPinSettingMode = 1;
                    currentMode = MODE_PIN;
                    pinIndex = 0;
                    oldPinRetryCount = 0;
                    memset(currentPin, 0, sizeof(currentPin));
                    OLED_ShowString(0, 0, (uint8_t*)"Enter Old PIN", 8, 1);
                    OLED_ShowString(0, 8, (uint8_t*)"Old PIN: ", 8, 1);
                    OLED_ShowString(48, 8, (uint8_t*)"____", 8, 1);
                    OLED_ShowString(0, 16, (uint8_t*)"Enter 4-digit PIN", 8, 1);
                    OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                    OLED_Refresh();
                } else if(key == 13) {
                    // 进入NFC注册模式
                    isPinSettingMode = 2; // 标记为NFC注册模式
                    currentMode = MODE_PIN;
                    pinIndex = 0;
                    oldPinRetryCount = 0;
                    memset(currentPin, 0, sizeof(currentPin));
                    OLED_ShowString(0, 0, (uint8_t*)"Enter PIN to Register", 8, 1);
                    OLED_ShowString(0, 8, (uint8_t*)"PIN: ", 8, 1);
                    OLED_ShowString(48, 8, (uint8_t*)"____", 8, 1);
                    OLED_ShowString(0, 16, (uint8_t*)"Enter 4-digit PIN", 8, 1);
                    OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                    OLED_Refresh();
                } else if(key == 15) {
                    // 进入NFC注销模式
                    isPinSettingMode = 3; // 标记为NFC注销模式
                    currentMode = MODE_PIN;
                    pinIndex = 0;
                    oldPinRetryCount = 0;
                    memset(currentPin, 0, sizeof(currentPin));
                    OLED_ShowString(0, 0, (uint8_t*)"Enter PIN to Unregister", 8, 1);
                    OLED_ShowString(0, 8, (uint8_t*)"PIN: ", 8, 1);
                    OLED_ShowString(48, 8, (uint8_t*)"____", 8, 1);
                    OLED_ShowString(0, 16, (uint8_t*)"Enter 4-digit PIN", 8, 1);
                    OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                    OLED_Refresh();
                } else if(key == 4) {
                    // 进入指纹注册模式
                    isPinSettingMode = 4; // 标记为指纹注册模式
                    currentMode = MODE_PIN;
                    pinIndex = 0;
                    oldPinRetryCount = 0;
                    memset(currentPin, 0, sizeof(currentPin));
                    OLED_ShowString(0, 0, (uint8_t*)"Enter PIN to Register", 8, 1);
                    OLED_ShowString(0, 8, (uint8_t*)"Fingerprint", 8, 1);
                    OLED_ShowString(0, 16, (uint8_t*)"PIN: ", 8, 1);
                    OLED_ShowString(48, 16, (uint8_t*)"____", 8, 1);
                    OLED_ShowString(0, 24, (uint8_t*)"Enter 4-digit PIN", 8, 1);
                    OLED_Refresh();

                } else if(key == 12) {
                    // 进入指纹删除模式
                    isPinSettingMode = 5; // 标记为指纹删除模式
                    currentMode = MODE_PIN;
                    pinIndex = 0;
                    oldPinRetryCount = 0;
                    memset(currentPin, 0, sizeof(currentPin));
                    OLED_ShowString(0, 0, (uint8_t*)"Enter PIN to Delete", 8, 1);
                    OLED_ShowString(0, 8, (uint8_t*)"Fingerprint", 8, 1);
                    OLED_ShowString(0, 16, (uint8_t*)"PIN: ", 8, 1);
                    OLED_ShowString(48, 16, (uint8_t*)"____", 8, 1);
                    OLED_ShowString(0, 24, (uint8_t*)"Enter 4-digit PIN", 8, 1);
                    OLED_Refresh();
                }
                break;
            case MODE_PIN:
                if(key >= 1 && key <= 15) {
                    uint8_t digit = KeyToDigit(key);
                    if(pinIndex < 4) {
                        currentPin[pinIndex] = digit;
                        // 显示输入的数字
                        char digitStr[2] = {digit + '0', '\0'};
                        if(isPinSettingMode) {
                            // 旧PIN码输入，显示在第2行
                            OLED_ShowString(48 + pinIndex * 8, 8, (uint8_t*)digitStr, 8, 1);
                        } else {
                            // 普通PIN码输入，显示在第3行
                            OLED_ShowString(pinIndex * 8, 16, (uint8_t*)digitStr, 8, 1);
                        }
                        OLED_Refresh();
                        pinIndex++;
                        
                        // 4个数字输入完成
                        if(pinIndex == 4) {
                            if(isPinSettingMode == 1) {
                                // 验证旧PIN码（用于修改PIN）
                                if(UI_CheckPinSetting(currentPin)) {
                                    // 验证成功，进入修改PIN码界面
                                    currentMode = MODE_PIN_SETTING;
                                    memset(newPin, 0, sizeof(newPin));
                                    pinIndex = 0;
                                    isPinSettingMode = 0;
                                    UI_ShowPINSetting();
                                } else {
                                    // 验证失败
                                    oldPinRetryCount++;
                                    if(oldPinRetryCount >= MAX_PIN_RETRY) {
                                        OLED_ShowString(0, 24, (uint8_t*)"Error!", 8, 1);
                                        OLED_Refresh();
                                        HAL_Delay(1000);
                                        OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                                        OLED_Refresh();
                                        currentMode = MODE_MAIN;
                                        isPinSettingMode = 0;
                                        oldPinRetryCount = 0;
                                        ClearPIN();
                                    } else {
                                        OLED_ShowString(0, 24, (uint8_t*)"Error!", 8, 1);
                                        OLED_Refresh();
                                        HAL_Delay(1000);
                                        OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                                        OLED_Refresh();
                                        pinIndex = 0;
                                        memset(currentPin, 0, sizeof(currentPin));
                                        OLED_ShowString(48, 8, (uint8_t*)"____", 8, 1);
                                        OLED_Refresh();
                                    }
                                }
                            } else if(isPinSettingMode == 2) {
                                // 验证PIN码（用于NFC注册）
                                if(UI_CheckPinSetting(currentPin)) {
                                    // 验证成功，进入NFC注册模式
                                    currentMode = MODE_NFC_REGISTER;
                                    isPinSettingMode = 0;
                                    OLED_ShowString(0, 0, (uint8_t*)"card to register", 8, 1);
                                    OLED_ShowString(0, 8, (uint8_t*)"                    ", 8, 1);
                                    OLED_ShowString(0, 16, (uint8_t*)"                    ", 8, 1);
                                    OLED_ShowString(0, 24, (uint8_t*)"                    ", 8, 1);
                                    OLED_Refresh();
                                    RegisterNFC();
                                } else {
                                    // 验证失败
                                    oldPinRetryCount++;
                                    if(oldPinRetryCount >= MAX_PIN_RETRY) {
                                        OLED_ShowString(0, 24, (uint8_t*)"Error!", 8, 1);
                                        OLED_Refresh();
                                        HAL_Delay(1000);
                                        OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                                        OLED_Refresh();
                                        currentMode = MODE_MAIN;
                                        isPinSettingMode = 0;
                                        oldPinRetryCount = 0;
                                        ClearPIN();
                                    } else {
                                        OLED_ShowString(0, 24, (uint8_t*)"Error!", 8, 1);
                                        OLED_Refresh();
                                        HAL_Delay(1000);
                                        OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                                        OLED_Refresh();
                                        pinIndex = 0;
                                        memset(currentPin, 0, sizeof(currentPin));
                                        OLED_ShowString(48, 8, (uint8_t*)"____", 8, 1);
                                        OLED_Refresh();
                                    }
                                }
                            } else if(isPinSettingMode == 3) {
                                // 验证PIN码（用于NFC注销）
                                if(UI_CheckPinSetting(currentPin)) {
                                    // 验证成功，进入NFC注销模式
                                    currentMode = MODE_NFC_UNREGISTER;
                                    isPinSettingMode = 0;
                                    OLED_ShowString(0, 0, (uint8_t*)"card to unregister", 8, 1);
                                    OLED_ShowString(0, 8, (uint8_t*)"                    ", 8, 1);
                                    OLED_ShowString(0, 16, (uint8_t*)"                    ", 8, 1);
                                    OLED_ShowString(0, 24, (uint8_t*)"                    ", 8, 1);
                                    OLED_Refresh();
                                    UnregisterNFC();
                                } else {
                                    // 验证失败
                                    oldPinRetryCount++;
                                    if(oldPinRetryCount >= MAX_PIN_RETRY) {
                                        OLED_ShowString(0, 24, (uint8_t*)"Error!", 8, 1);
                                        OLED_Refresh();
                                        HAL_Delay(1000);
                                        OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                                        OLED_Refresh();
                                        currentMode = MODE_MAIN;
                                        isPinSettingMode = 0;
                                        oldPinRetryCount = 0;
                                        ClearPIN();
                                    } else {
                                        OLED_ShowString(0, 24, (uint8_t*)"Error!", 8, 1);
                                        OLED_Refresh();
                                        HAL_Delay(1000);
                                        OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                                        OLED_Refresh();
                                        pinIndex = 0;
                                        memset(currentPin, 0, sizeof(currentPin));
                                        OLED_ShowString(48, 8, (uint8_t*)"____", 8, 1);
                                        OLED_Refresh();
                                    }
                                }
                            } else if(isPinSettingMode == 4) {
                                // 验证PIN码（用于指纹注册）
                                if(UI_CheckPinSetting(currentPin)) {
                                    // 验证成功，进入指纹注册模式
                                    currentMode = MODE_FINGER_REGISTER;
                                    isPinSettingMode = 0;
                                    // 完全清除OLED屏幕
                                    OLED_Clear();
                                    OLED_Refresh();
                                    OLED_ShowString(0, 0, (uint8_t*)"Fingerprint", 8, 1);
                                    OLED_ShowString(0, 8, (uint8_t*)"                ", 8, 1);
                                    OLED_ShowString(0, 16, (uint8_t*)"                ", 8, 1);
                                    OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                                    OLED_Refresh();
                                    RegisterFingerprint();
                                } else {
                                    // 验证失败
                                    oldPinRetryCount++;
                                    if(oldPinRetryCount >= MAX_PIN_RETRY) {
                                        OLED_ShowString(0, 24, (uint8_t*)"Error!", 8, 1);
                                        OLED_Refresh();
                                        HAL_Delay(1000);
                                        OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                                        OLED_Refresh();
                                        currentMode = MODE_MAIN;
                                        isPinSettingMode = 0;
                                        oldPinRetryCount = 0;
                                        ClearPIN();
                                    } else {
                                        OLED_ShowString(0, 24, (uint8_t*)"Error!", 8, 1);
                                        OLED_Refresh();
                                        HAL_Delay(1000);
                                        OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                                        OLED_Refresh();
                                        pinIndex = 0;
                                        memset(currentPin, 0, sizeof(currentPin));
                                        OLED_ShowString(48, 16, (uint8_t*)"____", 8, 1);
                                        OLED_Refresh();
                                    }
                                }
                            } else if(isPinSettingMode == 5) {
                                // 验证PIN码（用于指纹删除）
                                if(UI_CheckPinSetting(currentPin)) {
                                    // 验证成功，进入指纹删除模式
                                    currentMode = MODE_FINGER_UNREGISTER;
                                    isPinSettingMode = 0;
                                    // 完全清除OLED屏幕
                                    OLED_Clear();
                                    OLED_Refresh();
                                    OLED_ShowString(0, 0, (uint8_t*)"Delete Fingerprint", 8, 1);
                                    OLED_ShowString(0, 8, (uint8_t*)"                ", 8, 1);
                                    OLED_ShowString(0, 16, (uint8_t*)"                ", 8, 1);
                                    OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                                    OLED_Refresh();
                                    DeleteFingerprint();
                                } else {
                                    // 验证失败
                                    oldPinRetryCount++;
                                    if(oldPinRetryCount >= MAX_PIN_RETRY) {
                                        OLED_ShowString(0, 24, (uint8_t*)"Error!", 8, 1);
                                        OLED_Refresh();
                                        HAL_Delay(1000);
                                        OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                                        OLED_Refresh();
                                        currentMode = MODE_MAIN;
                                        isPinSettingMode = 0;
                                        oldPinRetryCount = 0;
                                        ClearPIN();
                                    } else {
                                        OLED_ShowString(0, 24, (uint8_t*)"Error!", 8, 1);
                                        OLED_Refresh();
                                        HAL_Delay(1000);
                                        OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                                        OLED_Refresh();
                                        pinIndex = 0;
                                        memset(currentPin, 0, sizeof(currentPin));
                                        OLED_ShowString(48, 16, (uint8_t*)"____", 8, 1);
                                        OLED_Refresh();
                                    }
                                }

                            } else {
                                // 验证普通PIN码
                                if(UI_CheckPIN(currentPin)) {
                                    Relay_On();
                                    OLED_ShowString(0, 24, (uint8_t*)"Success!", 8, 1);
                                    OLED_Refresh();
                                    HAL_Delay(RELAY_ON_TIME);
                                    // 清除success的显示和PIN码的显示
                                    OLED_ShowString(0, 24, (uint8_t*)"        ", 8, 1);
                                    OLED_ShowString(0, 16, (uint8_t*)"      ", 8, 1);
                                    OLED_Refresh();
                                    Relay_Off();
                                    currentMode = MODE_MAIN;
                                    isPinSettingMode = 0;
                                    pinRetryCount = 0;
                                } else {
                                    pinRetryCount++;
                                    if(pinRetryCount >= MAX_PIN_RETRY) {
                                        Beep_On();
                                        OLED_ShowString(0, 24, (uint8_t*)"Alarm!", 8, 1);
                                        OLED_Refresh();
                                        HAL_Delay(3000);
                                        Beep_Off();
                                        OLED_ShowString(0, 24, (uint8_t*)"        ", 8, 1);
                                        OLED_Refresh();
                                        currentMode = MODE_MAIN;
                                        isPinSettingMode = 0;
                                        pinRetryCount = 0;
                                    } else {
                                        OLED_ShowString(0, 24, (uint8_t*)"Error!", 8, 1);
                                        OLED_Refresh();
                                        HAL_Delay(1000);
                                        OLED_ShowString(0, 24, (uint8_t*)"        ", 8, 1);
                                        OLED_Refresh();
                                    }
                                    ClearPIN();
                                }
                            }
                        }
                    }
                } else if(key == 16) {
                    // 重新输入
                    pinIndex = 0;
                    memset(currentPin, 0, sizeof(currentPin));
                    if(isPinSettingMode == 1 || isPinSettingMode == 2 || isPinSettingMode == 3) {
                        // 旧PIN码输入或NFC操作，清除第2行
                        OLED_ShowString(48, 8, (uint8_t*)"____", 8, 1);
                    } else if(isPinSettingMode == 4 || isPinSettingMode == 5) {
                        // 指纹操作，清除第3行
                        OLED_ShowString(48, 16, (uint8_t*)"____", 8, 1);
                    } else {
                        // 普通PIN码输入，清除第3行
                        OLED_ShowString(0, 16, (uint8_t*)"____", 8, 1);
                    }
                    OLED_Refresh();
                }
                break;
            case MODE_PIN_SETTING:
                if(key >= 1 && key <= 15) {
                    uint8_t digit = KeyToDigit(key);
                    if(pinIndex < 4) {
                        newPin[pinIndex] = digit;
                        // 显示输入的数字
                        char digitStr[2] = {digit + '0', '\0'};
                        OLED_ShowString(48 + pinIndex * 8, 8, (uint8_t*)digitStr, 8, 1);
                        OLED_Refresh();
                        pinIndex++;
                        
                        // 4个数字输入完成
                        if(pinIndex == 4) {
                            // 进入确认模式
                            currentMode = MODE_PIN_CONFIRM;
                            pinIndex = 0;
                            memset(confirmPin, 0, sizeof(confirmPin));
                            UI_ShowPINConfirm();
                        }
                    }
                } else if(key == 16) {
                    // 重新输入
                    pinIndex = 0;
                    memset(newPin, 0, sizeof(newPin));
                    OLED_ShowString(48, 8, (uint8_t*)"____", 8, 1);
                    OLED_Refresh();
                }
                break;
            case MODE_PIN_CONFIRM:
                if(key >= 1 && key <= 15) {
                    uint8_t digit = KeyToDigit(key);
                    if(pinIndex < 4) {
                        confirmPin[pinIndex] = digit;
                        // 显示输入的数字
                        char digitStr[2] = {digit + '0', '\0'};
                        OLED_ShowString(48 + pinIndex * 8, 8, (uint8_t*)digitStr, 8, 1);
                        OLED_Refresh();
                        pinIndex++;
                        
                        // 4个数字输入完成
                        if(pinIndex == 4) {
                            // 保存新PIN码
                            if(UI_SaveNewPIN(newPin, confirmPin)) {
                                OLED_ShowString(0, 24, (uint8_t*)"Saved!", 8, 1);
                                OLED_Refresh();
                                HAL_Delay(1000);
                            } else {
                                OLED_ShowString(0, 24, (uint8_t*)"Mismatch!", 8, 1);
                                OLED_Refresh();
                                HAL_Delay(1000);
                            }
                            
                            OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
                            OLED_Refresh();
                            currentMode = MODE_MAIN;
                            isPinSettingMode = 0;
                            ClearPIN();
                        }
                    }
                } else if(key == 16) {
                    // 重新输入
                    pinIndex = 0;
                    memset(confirmPin, 0, sizeof(confirmPin));
                    OLED_ShowString(48, 8, (uint8_t*)"____", 8, 1);
                    OLED_Refresh();
                }
                break;
        }
    }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number, */
  /* ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
