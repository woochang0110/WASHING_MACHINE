/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software , it is provided AS-IS.
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
#define LED_GREEN_Pin GPIO_PIN_3
#define LED_GREEN_GPIO_Port GPIOE
#define LED_YELLOW_Pin GPIO_PIN_5
#define LED_YELLOW_GPIO_Port GPIOE
#define LED_RED_Pin GPIO_PIN_6
#define LED_RED_GPIO_Port GPIOE
#define BUTTON_4_Pin GPIO_PIN_0
#define BUTTON_4_GPIO_Port GPIOF
#define BUTTON_5_Pin GPIO_PIN_1
#define BUTTON_5_GPIO_Port GPIOF
#define BUTTON_6_Pin GPIO_PIN_2
#define BUTTON_6_GPIO_Port GPIOF
#define MCO_Pin GPIO_PIN_0
#define MCO_GPIO_Port GPIOH
#define RMII_MDC_Pin GPIO_PIN_1
#define RMII_MDC_GPIO_Port GPIOC
#define DHT11_Pin GPIO_PIN_0
#define DHT11_GPIO_Port GPIOA
#define RMII_REF_CLK_Pin GPIO_PIN_1
#define RMII_REF_CLK_GPIO_Port GPIOA
#define RMII_MDIO_Pin GPIO_PIN_2
#define RMII_MDIO_GPIO_Port GPIOA
#define TIM2_CH1_SERVO_Pin GPIO_PIN_5
#define TIM2_CH1_SERVO_GPIO_Port GPIOA
#define ULTRASONIC_TIM3_CH1_Pin GPIO_PIN_6
#define ULTRASONIC_TIM3_CH1_GPIO_Port GPIOA
#define RMII_CRS_DV_Pin GPIO_PIN_7
#define RMII_CRS_DV_GPIO_Port GPIOA
#define RMII_RXD0_Pin GPIO_PIN_4
#define RMII_RXD0_GPIO_Port GPIOC
#define RMII_RXD1_Pin GPIO_PIN_5
#define RMII_RXD1_GPIO_Port GPIOC
#define LD1_Pin GPIO_PIN_0
#define LD1_GPIO_Port GPIOB
#define FND_A_Pin GPIO_PIN_1
#define FND_A_GPIO_Port GPIOB
#define FND_B_Pin GPIO_PIN_2
#define FND_B_GPIO_Port GPIOB
#define ULTRASONIC_TRIG_Pin GPIO_PIN_12
#define ULTRASONIC_TRIG_GPIO_Port GPIOF
#define IN1_DCMOTOR_Pin GPIO_PIN_8
#define IN1_DCMOTOR_GPIO_Port GPIOE
#define IN1_DCMOTORE9_Pin GPIO_PIN_9
#define IN1_DCMOTORE9_GPIO_Port GPIOE
#define BUTTON_3_Pin GPIO_PIN_10
#define BUTTON_3_GPIO_Port GPIOE
#define BUTTON_2_Pin GPIO_PIN_12
#define BUTTON_2_GPIO_Port GPIOE
#define FAN_LED_Pin GPIO_PIN_13
#define FAN_LED_GPIO_Port GPIOE
#define BUTTON_1_Pin GPIO_PIN_14
#define BUTTON_1_GPIO_Port GPIOE
#define BUTTON_0_Pin GPIO_PIN_15
#define BUTTON_0_GPIO_Port GPIOE
#define FND_G_Pin GPIO_PIN_10
#define FND_G_GPIO_Port GPIOB
#define FND_DP_Pin GPIO_PIN_11
#define FND_DP_GPIO_Port GPIOB
#define RMII_TXD1_Pin GPIO_PIN_13
#define RMII_TXD1_GPIO_Port GPIOB
#define LD3_Pin GPIO_PIN_14
#define LD3_GPIO_Port GPIOB
#define STLK_RX_Pin GPIO_PIN_8
#define STLK_RX_GPIO_Port GPIOD
#define STLK_TX_Pin GPIO_PIN_9
#define STLK_TX_GPIO_Port GPIOD
#define USB_PowerSwitchOn_Pin GPIO_PIN_6
#define USB_PowerSwitchOn_GPIO_Port GPIOG
#define USB_OverCurrent_Pin GPIO_PIN_7
#define USB_OverCurrent_GPIO_Port GPIOG
#define FND_D1_Pin GPIO_PIN_8
#define FND_D1_GPIO_Port GPIOC
#define FND_D2_Pin GPIO_PIN_9
#define FND_D2_GPIO_Port GPIOC
#define USB_SOF_Pin GPIO_PIN_8
#define USB_SOF_GPIO_Port GPIOA
#define USB_VBUS_Pin GPIO_PIN_9
#define USB_VBUS_GPIO_Port GPIOA
#define USB_ID_Pin GPIO_PIN_10
#define USB_ID_GPIO_Port GPIOA
#define USB_DM_Pin GPIO_PIN_11
#define USB_DM_GPIO_Port GPIOA
#define USB_DP_Pin GPIO_PIN_12
#define USB_DP_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define FND_D3_Pin GPIO_PIN_10
#define FND_D3_GPIO_Port GPIOC
#define FND_D4_Pin GPIO_PIN_11
#define FND_D4_GPIO_Port GPIOC
#define RMII_TX_EN_Pin GPIO_PIN_11
#define RMII_TX_EN_GPIO_Port GPIOG
#define RMII_TXD0_Pin GPIO_PIN_13
#define RMII_TXD0_GPIO_Port GPIOG
#define FND_C_Pin GPIO_PIN_3
#define FND_C_GPIO_Port GPIOB
#define FND_D_Pin GPIO_PIN_4
#define FND_D_GPIO_Port GPIOB
#define FND_E_Pin GPIO_PIN_5
#define FND_E_GPIO_Port GPIOB
#define FND_F_Pin GPIO_PIN_6
#define FND_F_GPIO_Port GPIOB
#define LD2_Pin GPIO_PIN_7
#define LD2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
