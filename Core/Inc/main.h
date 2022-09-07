/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
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
#include "stm32g4xx_hal.h"

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
#define BTN_RVS_Pin GPIO_PIN_1
#define BTN_RVS_GPIO_Port GPIOF
#define BTN_RVS_EXTI_IRQn EXTI1_IRQn
#define L_L_IS_Pin GPIO_PIN_0
#define L_L_IS_GPIO_Port GPIOA
#define L_R_IS_Pin GPIO_PIN_1
#define L_R_IS_GPIO_Port GPIOA
#define R_L_IS_Pin GPIO_PIN_2
#define R_L_IS_GPIO_Port GPIOA
#define R_R_IS_Pin GPIO_PIN_3
#define R_R_IS_GPIO_Port GPIOA
#define L_RPWM_Pin GPIO_PIN_4
#define L_RPWM_GPIO_Port GPIOA
#define BTN_LEFT_Pin GPIO_PIN_5
#define BTN_LEFT_GPIO_Port GPIOA
#define BTN_LEFT_EXTI_IRQn EXTI9_5_IRQn
#define L_LPWM_Pin GPIO_PIN_6
#define L_LPWM_GPIO_Port GPIOA
#define L_L_EN_Pin GPIO_PIN_7
#define L_L_EN_GPIO_Port GPIOA
#define L_R_EN_Pin GPIO_PIN_0
#define L_R_EN_GPIO_Port GPIOB
#define L_HAL_Pin GPIO_PIN_8
#define L_HAL_GPIO_Port GPIOA
#define BTN_FWD_Pin GPIO_PIN_10
#define BTN_FWD_GPIO_Port GPIOA
#define BTN_FWD_EXTI_IRQn EXTI15_10_IRQn
#define R_HALL_Pin GPIO_PIN_15
#define R_HALL_GPIO_Port GPIOA
#define BTN_RIGHT_Pin GPIO_PIN_3
#define BTN_RIGHT_GPIO_Port GPIOB
#define BTN_RIGHT_EXTI_IRQn EXTI3_IRQn
#define R_R_EN_Pin GPIO_PIN_4
#define R_R_EN_GPIO_Port GPIOB
#define R_L_EN_Pin GPIO_PIN_5
#define R_L_EN_GPIO_Port GPIOB
#define R_LPWM_Pin GPIO_PIN_6
#define R_LPWM_GPIO_Port GPIOB
#define R_RPWM_Pin GPIO_PIN_7
#define R_RPWM_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
enum BTN_TYPE{
    BTN_FRWD,
    BTN_RVRS,
    BTN_LEFT,
    BTN_RIGHT,
};
enum MOTOR_EVENT {
    EVENT_NULL = 0,
    EVENT_STOP, // 	move stopped
    EVENT_CSS,  //	constant speed reached
    EVENT_CSE   //  constant speed end
};
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
