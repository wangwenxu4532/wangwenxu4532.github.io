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
#include "i2c.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "dht11.h"
#include "oled.h"
#include "bsp_key.h"
#include "control.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static DHT11_Data dht11_data;       /* DHT11 温湿度数据 */
static uint32_t   last_read_tick;   /* 上次读取 DHT11 的 tick */
static uint32_t   last_display_tick;/* 上次刷新显示的 tick */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  HAL_Delay(100);  /* 等待外设稳定 */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  OLED_Init();                    /* 初始化 OLED */
  Control_Init();                 /* 初始化温控 + 启动 PWM */

  /* 显示开机画面 */
  OLED_ShowLine(0, "Mini Fridge");
  OLED_ShowLine(1, "Starting...");
  OLED_Refresh();
  HAL_Delay(1000);

  /* 初始化时间基准 */
  last_read_tick    = HAL_GetTick();
  last_display_tick = HAL_GetTick();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    uint32_t now = HAL_GetTick();

    /* ---- 按键扫描 (从队列中取完所有待处理事件) ---- */
    KeyEvent key_event;
    while ((key_event = Key_GetEvent()) != KEY_NONE) {
        if (key_event == KEY_UP_SHORT) {
            Control_SetTarget(Control_GetTarget() + 1);
        } else if (key_event == KEY_DOWN_SHORT) {
            Control_SetTarget(Control_GetTarget() - 1);
        }
    }

    /* ---- 每 1 秒: 读取 DHT11 + 温控更新 ---- */
    if (now - last_read_tick >= 1000) {
        last_read_tick = now;
        if (DHT11_Read(&dht11_data) == 0) {
            Control_Update((int8_t)dht11_data.temperature);
        }
    }

    /* ---- 每 300ms: 刷新 OLED 显示 ---- */
    if (now - last_display_tick >= 300) {
        last_display_tick = now;

        char buf[17];

        /* 写入缓冲区 (不刷新) */
        snprintf(buf, sizeof(buf), "Temp: %d C", dht11_data.temperature);
        OLED_ShowLine(0, buf);

        snprintf(buf, sizeof(buf), "Set : %d C", Control_GetTarget());
        OLED_ShowLine(1, buf);

        OLED_ShowLine(2, Control_IsCooling() ? "Cool: ON " : "Cool: OFF");

        uint8_t up_val = HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin);
        uint8_t down_val = HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin);
        snprintf(buf, sizeof(buf), "K1:%d K2:%d", up_val, down_val);
        OLED_ShowLine(3, buf);

        /* 一次性刷新全屏 */
        OLED_Refresh();
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
  HAL_StatusTypeDef hal_status;

  /* ---- 第一步: 尝试 HSE + PLL (72MHz) ---- */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  hal_status = HAL_RCC_OscConfig(&RCC_OscInitStruct);

  if (hal_status == HAL_OK) {
      /* HSE 起振成功, 用 PLL 72MHz */
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
  } else {
      /*
       * HSE 晶振不存在或不起振 (常见于低成本最小系统板)
       * 退回到 HSI 8MHz, 不用 PLL
       */
      RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
      RCC_OscInitStruct.HSEState = RCC_HSE_OFF;
      RCC_OscInitStruct.HSIState = RCC_HSI_ON;
      RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
      HAL_RCC_OscConfig(&RCC_OscInitStruct);  /* HSI 不会失败 */

      RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
      RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
      RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
      RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
      RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

      if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
      {
          Error_Handler();
      }
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
