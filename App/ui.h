#ifndef __UI_H__
#define __UI_H__

#include "main.h"
#include "oled.h"

void welcome(void);
uint8_t UI_CheckPIN(uint8_t* inputPin);
uint8_t UI_CheckPinSetting(uint8_t* inputPin);
uint8_t UI_SaveNewPIN(uint8_t* newPin, uint8_t* confirmPin);
void UI_ShowPINInput(void);
void UI_ShowPINSetting(void);
void UI_ShowPINConfirm(void);

#endif /* __UI_H__ */