#include "ui.h"
#include "flash.h"
#include "beep_lay.h"
#include "main.h"

#define MAX_PIN_RETRY 3
#define RELAY_ON_TIME 5000

void welcome(void)
{
	 OLED_ShowString(10, 0, (uint8_t*)"Welcome", 8, 1);
   OLED_ShowString(0, 32, (uint8_t*)"Press KEY16 for PIN", 8, 1);
   OLED_Refresh();
}

uint8_t UI_CheckPIN(uint8_t* inputPin)
{
    uint8_t storedPin[4];
    Flash_ReadPIN(storedPin); // 从FLASH读取存储的PIN码
    
    // 比较4位数组
    uint8_t pinMatch = 1;
    for(uint8_t i = 0; i < 4; i++)
    {
        if(inputPin[i] + '0' != storedPin[i])
        {
            pinMatch = 0;
            break;
        }
    }
    
    return pinMatch;
}

uint8_t UI_CheckPinSetting(uint8_t* inputPin)
{
    uint8_t storedPin[4];
    Flash_ReadPIN(storedPin); // 从FLASH读取存储的PIN码
    
    // 比较4位数组
    uint8_t pinMatch = 1;
    for(uint8_t i = 0; i < 4; i++)
    {
        if(inputPin[i] + '0' != storedPin[i])
        {
            pinMatch = 0;
            break;
        }
    }
    
    return pinMatch;
}

uint8_t UI_SaveNewPIN(uint8_t* newPin, uint8_t* confirmPin)
{
    // 比较4位数组
    uint8_t pinMatch = 1;
    for(uint8_t i = 0; i < 4; i++)
    {
        if(newPin[i] != confirmPin[i])
        {
            pinMatch = 0;
            break;
        }
    }
    
    if(pinMatch)
    {
        // 两次输入一致，保存新PIN码
        uint8_t asciiPin[4];
        for(uint8_t i = 0; i < 4; i++)
        {
            asciiPin[i] = newPin[i] + '0';
        }
        Flash_WritePIN(asciiPin);
    }
    
    return pinMatch;
}

void UI_ShowPINInput(void)
{
    OLED_ShowString(10, 0, (uint8_t*)"Welcome", 8, 1);
    OLED_ShowString(0, 16, (uint8_t*)"____", 8, 1);
    OLED_ShowString(0, 32, (uint8_t*)"Enter PIN", 8, 1);
    OLED_Refresh();
}

void UI_ShowPINSetting(void)
{
    OLED_ShowString(0, 0, (uint8_t*)"Set New PIN", 8, 1);
    OLED_ShowString(0, 8, (uint8_t*)"New PIN: ", 8, 1);
    OLED_ShowString(48, 8, (uint8_t*)"____", 8, 1);
    OLED_ShowString(0, 16, (uint8_t*)"Enter 4-digit PIN", 8, 1);
    OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
    OLED_Refresh();
}

void UI_ShowPINConfirm(void)
{
    OLED_ShowString(0, 0, (uint8_t*)"Confirm New PIN", 8, 1);
    OLED_ShowString(0, 8, (uint8_t*)"Confirm: ", 8, 1);
    OLED_ShowString(48, 8, (uint8_t*)"____", 8, 1);
    OLED_ShowString(0, 16, (uint8_t*)"Enter 4-digit PIN", 8, 1);
    OLED_ShowString(0, 24, (uint8_t*)"                ", 8, 1);
    OLED_Refresh();
}
