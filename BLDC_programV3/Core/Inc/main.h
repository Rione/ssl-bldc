/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin GPIO_PIN_14
#define LED_GPIO_Port GPIOC
#define C_U_Pin GPIO_PIN_0
#define C_U_GPIO_Port GPIOA
#define C_W_Pin GPIO_PIN_1
#define C_W_GPIO_Port GPIOA
#define USBTX_Pin GPIO_PIN_2
#define USBTX_GPIO_Port GPIOA
#define USBRX_Pin GPIO_PIN_3
#define USBRX_GPIO_Port GPIOA
#define PWM_C_Pin GPIO_PIN_10
#define PWM_C_GPIO_Port GPIOB
#define BLUE_Pin GPIO_PIN_13
#define BLUE_GPIO_Port GPIOB
#define RED_Pin GPIO_PIN_14
#define RED_GPIO_Port GPIOB
#define GREEN_Pin GPIO_PIN_15
#define GREEN_GPIO_Port GPIOB
#define PWM_A_Pin GPIO_PIN_7
#define PWM_A_GPIO_Port GPIOC
#define PWM_B_Pin GPIO_PIN_4
#define PWM_B_GPIO_Port GPIOB
#define IDIN_B_Pin GPIO_PIN_5
#define IDIN_B_GPIO_Port GPIOB
#define SPI1_CS_Pin GPIO_PIN_6
#define SPI1_CS_GPIO_Port GPIOB
#define IDIN_A_Pin GPIO_PIN_7
#define IDIN_A_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
