/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "icm42688.h"
#include "sensor.h"
#include "shell.h"
#include <stdio.h>
#include <math.h>
#include "task.h"
#include "menu.h"
#include "lcd.h"
#include "pid.h"
#include "IMU.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */


#define REMOTE_CONTROL 0	// 遥控模式


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

//	这里有一个 extern int CrsfChannels[CRSF_NUM_CHANNELS]; 移到 crsf.h 中去了,未检查可行性

#if REMOTE_CONTROL
int16_t speed = 0;
int16_t error = 0;
#endif

/* --------------------LCD-------------------- */
static uint16_t line_buffer[320];

lcd_io lcd_io_desc = {
    .spi = &hspi1,
    .rst = {LCD_RST_GPIO_Port, LCD_RST_Pin, 0},
    .bl  = {LCD_PWR_GPIO_Port, LCD_PWR_Pin, 1},
    .cs  = {LCD_CS_GPIO_Port , LCD_CS_Pin , 0},
    .dc  = {LCD_DC_GPIO_Port , LCD_DC_Pin , 0},
};

lcd lcd_desc = {
    .io = &lcd_io_desc,
    .line_buffer = line_buffer,
};
/* ------------------------------------------- */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
//	static float x0 = 0;
//	static float y0 = 0;
	
	if(htim->Instance == TIM1) {			// 10ms

		IMU_getYawPitchRoll(imu_angle);		// 更新陀螺仪角度
		
#if REMOTE_CONTROL
		speed = (CrsfChannels[1] - 1000) / 20;
		error = (CrsfChannels[0] - 1000) / 40;
		PID_Speed(speed+error,speed-error);
#endif
//		x0 = x0 + 1 * sin(imu_angle[0] / 57.296f);
//		y0 = y0 + 1 * cos(imu_angle[0] / 57.296f);
//		printf("%.2f,%.2f\n",x0,y0);
	}
	if(htim->Instance == TIM5) {	// 100ns
		imuUpdata();				// IMU心跳
	}
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
	
#if REMOTE_CONTROL
  Crc_init(0xD5);		// crsf接收机初始化
#endif
	
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_SPI1_Init();
  MX_ADC1_Init();
  MX_TIM5_Init();
  MX_SPI2_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  
  /* ----------用户初始化函数---------- */
  ShellInit();												// 初始化命令系统
  SchedulerInit();											// 初始化任务管理系统
  lcd_init_dev(&lcd_desc, LCD_1_47_INCH, LCD_ROTATE_90);	// 初始化显示屏
  MenuInit();												// 初始化菜单
  lcd_clear(&lcd_desc, BLACK);
  IMU_init();												// 初始化陀螺仪
  HAL_Delay(100);
  
  /* ----------系统初始化函数---------- */
#if REMOTE_CONTROL
  HAL_UART_Receive_IT(&huart2, (uint8_t *)RxBuffer,LENGTH);	// 开启接收机
#endif
  HAL_TIM_Base_Start_IT(&htim1);
  HAL_TIM_Base_Start_IT(&htim5);
  HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);		// 启动左编码器
  HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);		// 启动右编码器
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
  
  /* ----------初始程序---------- */
  
  lcd_print(&lcd_desc, 0, 40, "> IPS LCD 1.47inch 320x172");
  lcd_print(&lcd_desc, 0, 60, "> STM32F407VGT6");
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//	printf("%d,%d,%d,%d,%d,%d,%d,%d\n",arr[0],arr[1],arr[2],arr[3],arr[4],arr[5],arr[6],arr[7]);		//灰度传感器状态
	  
//	printf("ch1:%d\r\n",CrsfChannels[0]);
//	printf("ch2:%d\r\n",CrsfChannels[1]);
//	printf("ch3:%d\r\n",CrsfChannels[2]);
//	printf("ch4:%d\r\n",CrsfChannels[3]);
//	printf("ch5:%d\r\n",CrsfChannels[4]);
//	printf("ch6:%d\r\n",CrsfChannels[5]);
//	printf("ch7:%d\r\n",CrsfChannels[6]);
//	printf("ch8:%d\r\n",CrsfChannels[7]);
//	printf("ch9:%d\r\n",CrsfChannels[8]);
//	printf("ch10:%d\r\n",CrsfChannels[9]);
//	printf("ch11:%d\r\n",CrsfChannels[10]);
//	printf("ch12:%d\r\n",CrsfChannels[11]);
//	HAL_Delay(100);
	  
//	read_gyro_corrected(&Yaw,&Pitch,&Roll);
//	printf("%.2f %.2f %.2f\n",Yaw,Pitch,Roll);
//	printf("%.2f %.2f %.2f\n",imu_angle[0],imu_angle[1],imu_angle[2]);
	  
	SchedulerRun();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
