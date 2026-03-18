#include "Matrix_Keyboard.h"


uint8_t Matrix_Keyboard_Scan(void)
{
  uint8_t key_value = KEY_NONE;
  
  /* 扫描LINE1 */
  HAL_GPIO_WritePin(LINE1_GPIO_Port, LINE1_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LINE2_GPIO_Port, LINE2_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LINE3_GPIO_Port, LINE3_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LINE4_GPIO_Port, LINE4_Pin, GPIO_PIN_SET);
  
  if(HAL_GPIO_ReadPin(ROW1_GPIO_Port, ROW1_Pin) == GPIO_PIN_RESET)
  {
    HAL_Delay(10); /* 消抖 */
    if(HAL_GPIO_ReadPin(ROW1_GPIO_Port, ROW1_Pin) == GPIO_PIN_RESET)
    {
      key_value = 1;
      while(HAL_GPIO_ReadPin(ROW1_GPIO_Port, ROW1_Pin) == GPIO_PIN_RESET);
    }
  }
  else if(HAL_GPIO_ReadPin(ROW2_GPIO_Port, ROW2_Pin) == GPIO_PIN_RESET)
  {
    HAL_Delay(10);
    if(HAL_GPIO_ReadPin(ROW2_GPIO_Port, ROW2_Pin) == GPIO_PIN_RESET)
    {
      key_value = 2;
      while(HAL_GPIO_ReadPin(ROW2_GPIO_Port, ROW2_Pin) == GPIO_PIN_RESET);
    }
  }
  else if(HAL_GPIO_ReadPin(ROW3_GPIO_Port, ROW3_Pin) == GPIO_PIN_RESET)
  {
    HAL_Delay(10);
    if(HAL_GPIO_ReadPin(ROW3_GPIO_Port, ROW3_Pin) == GPIO_PIN_RESET)
    {
      key_value = 3;
      while(HAL_GPIO_ReadPin(ROW3_GPIO_Port, ROW3_Pin) == GPIO_PIN_RESET);
    }
  }
  else if(HAL_GPIO_ReadPin(ROW4_GPIO_Port, ROW4_Pin) == GPIO_PIN_RESET)
  {
    HAL_Delay(10);
    if(HAL_GPIO_ReadPin(ROW4_GPIO_Port, ROW4_Pin) == GPIO_PIN_RESET)
    {
      key_value = 4;
      while(HAL_GPIO_ReadPin(ROW4_GPIO_Port, ROW4_Pin) == GPIO_PIN_RESET);
    }
  }
  
  /* 扫描LINE2 */
  HAL_GPIO_WritePin(LINE1_GPIO_Port, LINE1_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LINE2_GPIO_Port, LINE2_Pin, GPIO_PIN_RESET);
  
  if(HAL_GPIO_ReadPin(ROW1_GPIO_Port, ROW1_Pin) == GPIO_PIN_RESET)
  {
    HAL_Delay(10);
    if(HAL_GPIO_ReadPin(ROW1_GPIO_Port, ROW1_Pin) == GPIO_PIN_RESET)
    {
      key_value = 5;
      while(HAL_GPIO_ReadPin(ROW1_GPIO_Port, ROW1_Pin) == GPIO_PIN_RESET);
    }
  }
  else if(HAL_GPIO_ReadPin(ROW2_GPIO_Port, ROW2_Pin) == GPIO_PIN_RESET)
  {
    HAL_Delay(10);
    if(HAL_GPIO_ReadPin(ROW2_GPIO_Port, ROW2_Pin) == GPIO_PIN_RESET)
    {
      key_value = 6;
      while(HAL_GPIO_ReadPin(ROW2_GPIO_Port, ROW2_Pin) == GPIO_PIN_RESET);
    }
  }
  else if(HAL_GPIO_ReadPin(ROW3_GPIO_Port, ROW3_Pin) == GPIO_PIN_RESET)
  {
    HAL_Delay(10);
    if(HAL_GPIO_ReadPin(ROW3_GPIO_Port, ROW3_Pin) == GPIO_PIN_RESET)
    {
      key_value = 7;
      while(HAL_GPIO_ReadPin(ROW3_GPIO_Port, ROW3_Pin) == GPIO_PIN_RESET);
    }
  }
  else if(HAL_GPIO_ReadPin(ROW4_GPIO_Port, ROW4_Pin) == GPIO_PIN_RESET)
  {
    HAL_Delay(10);
    if(HAL_GPIO_ReadPin(ROW4_GPIO_Port, ROW4_Pin) == GPIO_PIN_RESET)
    {
      key_value = 8;
      while(HAL_GPIO_ReadPin(ROW4_GPIO_Port, ROW4_Pin) == GPIO_PIN_RESET);
    }
  }
  
  /* 扫描LINE3 */
  HAL_GPIO_WritePin(LINE2_GPIO_Port, LINE2_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LINE3_GPIO_Port, LINE3_Pin, GPIO_PIN_RESET);
  
  if(HAL_GPIO_ReadPin(ROW1_GPIO_Port, ROW1_Pin) == GPIO_PIN_RESET)
  {
    HAL_Delay(10);
    if(HAL_GPIO_ReadPin(ROW1_GPIO_Port, ROW1_Pin) == GPIO_PIN_RESET)
    {
      key_value = 9;
      while(HAL_GPIO_ReadPin(ROW1_GPIO_Port, ROW1_Pin) == GPIO_PIN_RESET);
    }
  }
  else if(HAL_GPIO_ReadPin(ROW2_GPIO_Port, ROW2_Pin) == GPIO_PIN_RESET)
  {
    HAL_Delay(10);
    if(HAL_GPIO_ReadPin(ROW2_GPIO_Port, ROW2_Pin) == GPIO_PIN_RESET)
    {
      key_value = 10;
      while(HAL_GPIO_ReadPin(ROW2_GPIO_Port, ROW2_Pin) == GPIO_PIN_RESET);
    }
  }
  else if(HAL_GPIO_ReadPin(ROW3_GPIO_Port, ROW3_Pin) == GPIO_PIN_RESET)
  {
    HAL_Delay(10);
    if(HAL_GPIO_ReadPin(ROW3_GPIO_Port, ROW3_Pin) == GPIO_PIN_RESET)
    {
      key_value = 11;
      while(HAL_GPIO_ReadPin(ROW3_GPIO_Port, ROW3_Pin) == GPIO_PIN_RESET);
    }
  }
  else if(HAL_GPIO_ReadPin(ROW4_GPIO_Port, ROW4_Pin) == GPIO_PIN_RESET)
  {
    HAL_Delay(10);
    if(HAL_GPIO_ReadPin(ROW4_GPIO_Port, ROW4_Pin) == GPIO_PIN_RESET)
    {
      key_value = 12;
      while(HAL_GPIO_ReadPin(ROW4_GPIO_Port, ROW4_Pin) == GPIO_PIN_RESET);
    }
  }
  
  /* 扫描LINE4 */
  HAL_GPIO_WritePin(LINE3_GPIO_Port, LINE3_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LINE4_GPIO_Port, LINE4_Pin, GPIO_PIN_RESET);
  
  if(HAL_GPIO_ReadPin(ROW1_GPIO_Port, ROW1_Pin) == GPIO_PIN_RESET)
  {
    HAL_Delay(10);
    if(HAL_GPIO_ReadPin(ROW1_GPIO_Port, ROW1_Pin) == GPIO_PIN_RESET)
    {
      key_value = 13;
      while(HAL_GPIO_ReadPin(ROW1_GPIO_Port, ROW1_Pin) == GPIO_PIN_RESET);
    }
  }
  else if(HAL_GPIO_ReadPin(ROW2_GPIO_Port, ROW2_Pin) == GPIO_PIN_RESET)
  {
    HAL_Delay(10);
    if(HAL_GPIO_ReadPin(ROW2_GPIO_Port, ROW2_Pin) == GPIO_PIN_RESET)
    {
      key_value = 14;
      while(HAL_GPIO_ReadPin(ROW2_GPIO_Port, ROW2_Pin) == GPIO_PIN_RESET);
    }
  }
  else if(HAL_GPIO_ReadPin(ROW3_GPIO_Port, ROW3_Pin) == GPIO_PIN_RESET)
  {
    HAL_Delay(10);
    if(HAL_GPIO_ReadPin(ROW3_GPIO_Port, ROW3_Pin) == GPIO_PIN_RESET)
    {
      key_value = 15;
      while(HAL_GPIO_ReadPin(ROW3_GPIO_Port, ROW3_Pin) == GPIO_PIN_RESET);
    }
  }
  else if(HAL_GPIO_ReadPin(ROW4_GPIO_Port, ROW4_Pin) == GPIO_PIN_RESET)
  {
    HAL_Delay(10);
    if(HAL_GPIO_ReadPin(ROW4_GPIO_Port, ROW4_Pin) == GPIO_PIN_RESET)
    {
      key_value = 16;
      while(HAL_GPIO_ReadPin(ROW4_GPIO_Port, ROW4_Pin) == GPIO_PIN_RESET);
    }
  }
  
  /* 恢复所有列为高电平 */
  HAL_GPIO_WritePin(LINE1_GPIO_Port, LINE1_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LINE2_GPIO_Port, LINE2_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LINE3_GPIO_Port, LINE3_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LINE4_GPIO_Port, LINE4_Pin, GPIO_PIN_SET);
  
  return key_value;
}


uint8_t KeyToDigit(uint8_t key)
{
    switch(key)
    {
        case 1: return 1;
        case 2: return 2;
        case 3: return 3;
        case 5: return 4;
        case 6: return 5;
        case 7: return 6;
        case 9: return 7;
        case 10: return 8;
        case 11: return 9;
        case 14: return 0;
        default: return 0xFF; // 无效按键
    }
}