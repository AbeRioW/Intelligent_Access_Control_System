#include "Matrix_Keyboard.h"

void Matrix_Keyboard_Init(void)
{
  /* 行列引脚已经在MX_GPIO_Init中初始化 */
  /* LINE1-LINE4为输出，ROW1-ROW4为输入 */
}

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
  HAL_GPIO_WritePin(LINE4_GPIO_Port, LINE4_Pin, GPIO_PIN_SET);
  
  return key_value;
}