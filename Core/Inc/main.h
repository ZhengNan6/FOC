/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32f1xx_hal.h"

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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define C_Pin GPIO_PIN_5
#define C_GPIO_Port GPIOA
#define B_Pin GPIO_PIN_6
#define B_GPIO_Port GPIOA
#define A_Pin GPIO_PIN_7
#define A_GPIO_Port GPIOA
#define C_L_Pin GPIO_PIN_13
#define C_L_GPIO_Port GPIOB
#define B_L_Pin GPIO_PIN_14
#define B_L_GPIO_Port GPIOB
#define A_L_Pin GPIO_PIN_15
#define A_L_GPIO_Port GPIOB
#define C_H_Pin GPIO_PIN_8
#define C_H_GPIO_Port GPIOA
#define B_H_Pin GPIO_PIN_9
#define B_H_GPIO_Port GPIOA
#define A_H_Pin GPIO_PIN_10
#define A_H_GPIO_Port GPIOA
#define GND5600_Pin GPIO_PIN_3
#define GND5600_GPIO_Port GPIOB
#define VCC5600_Pin GPIO_PIN_5
#define VCC5600_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
