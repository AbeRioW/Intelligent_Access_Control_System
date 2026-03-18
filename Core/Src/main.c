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
#include "ui.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum {
    MODE_MAIN,
    MODE_PIN,
    MODE_PIN_SETTING,
    MODE_PIN_CONFIRM
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
  OLED_Init();
  PCD_Init();
  Flash_Init();
  welcome();
   
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
                }
                break;
            case MODE_PIN:
                if(key >= 1 && key <= 16) {
                    uint8_t digit = KeyToDigit(key);
                    if(pinIndex < 4) {
                        currentPin[pinIndex] = digit;
                        // 显示输入的数字
                        char digitStr[2] = {digit + '0', '\0'};
                        OLED_ShowString(pinIndex * 8, 16, (uint8_t*)digitStr, 8, 1);
                        OLED_Refresh();
                        pinIndex++;
                        
                        // 4个数字输入完成
                        if(pinIndex == 4) {
                            if(isPinSettingMode) {
                                // 验证旧PIN码
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
                            } else {
                                // 验证PIN码
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
                    OLED_ShowString(0, 16, (uint8_t*)"____", 8, 1);
                    OLED_Refresh();
                }
                break;
            case MODE_PIN_SETTING:
                if(key >= 1 && key <= 16) {
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
                if(key >= 1 && key <= 10) {
                    // 数字按键 (1-10对应0-9)
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

#ifdef  USE_FULL_ASSERT
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
