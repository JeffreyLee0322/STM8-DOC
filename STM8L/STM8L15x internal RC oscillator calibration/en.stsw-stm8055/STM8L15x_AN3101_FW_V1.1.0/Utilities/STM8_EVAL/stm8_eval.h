/**
  ******************************************************************************
  * @file    stm8_eval.h
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    09/24/2010
  * @brief   Header file for stm8l_eval.c module.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM8_EVAL_H
#define __STM8_EVAL_H

/* Includes ------------------------------------------------------------------*/

/** @addtogroup Utilities
  * @{
  */

/** @addtogroup STM8_EVAL
  * @{
  */

/** @defgroup STM8_EVAL_Abstraction_Layer
  * @{
  */

/** @defgroup STM8_EVAL_HARDWARE_RESOURCES
  * @{
  */

/**
@code
 The table below gives an overview of the hardware resources supported by each
 STM8 EVAL board.
     - LCD: Mono LCD (Glass and Serial (SPI))
     - sFLASH: serial SPI FLASH (M25Pxxx)
     - sEE: serial I2C EEPROM (M24C32, M24C64)
     - TSENSOR: Temperature Sensor (LM75)
     - SD: SD Card memory (SPI (SD Card MODE))
  =============================================================================================+
    STM8 EVAL     | LED | Buttons  | Com Ports |    LCD    | sFLASH | sEE | TSENSOR | SD (SPI) |
  -============================================================================================+
   STM8L1526-EVAL |  5  |    6     |     1     | YES (SPI) |  YES   | YES |   YES   |    YES   |
                  |     |          |           | & (Glass) |        |     |         |          |
  ----------------------------------------------------------------------------------------------
   STM8L1528-EVAL |  5  |    6     |     1     | YES (SPI) |  YES   | YES |   YES   |    YES   |
                  |     |          |           | & (Glass) |        |     |         |          |
  =============================================================================================+
@endcode
*/
/**
  * @}
  */


/** @defgroup STM8_EVAL_Exported_Types
  * @{
  */
typedef enum
{
  LED1 = 0,
  LED2 = 1,
  LED3 = 2,
  LED4 = 3,
  LED5 = 4  /* Bicolor Led */
} Led_TypeDef;

typedef enum
{
  BUTTON_KEY = 0,
  BUTTON_RIGHT = 1,
  BUTTON_LEFT = 2,
  BUTTON_UP = 3,
  BUTTON_DOWN = 4,
  BUTTON_SEL = 5,
  BUTTON_TAMPER = 6
} Button_TypeDef;

typedef enum
{
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1
} ButtonMode_TypeDef;

typedef enum
{
  JOY_NONE = 0,
  JOY_SEL = 1,
  JOY_DOWN = 2,
  JOY_LEFT = 3,
  JOY_RIGHT = 4,
  JOY_UP = 5
} JOYState_TypeDef;

typedef enum
{
  COM1 = 0
} COM_TypeDef;

/**
  * @}
  */

/** @defgroup STM8_EVAL_Exported_Constants
  * @{
  */
/**
  * @brief  Uncomment the line corresponding to the STMicroelectronics evaluation
  *   board used in your application.
  *
  *  Tip: To avoid modifying this file each time you need to switch between these
  *       boards, you can define the board in your toolchain compiler preprocessor.
  */
#if !defined (USE_STM8L1526_EVAL) && !defined (USE_STM8L1528_EVAL)
//#define USE_STM8L1526_EVAL
//#define USE_STM8L1528_EVAL
#endif

#ifdef USE_STM8L1526_EVAL
#include "stm8l15x.h"
#include "stm8l1526_eval/stm8l1526_eval.h"
#elif defined USE_STM8L1528_EVAL
#include "stm8l15x.h"
#include "stm8l1528_eval/stm8l1528_eval.h"
#else
#error "Please select first the STM8L15X_EVAL board to be used (in stm8_eval.h)"
#endif

/**
  * @}
  */

/** @defgroup STM8L_EVAL_Exported_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup STM8L_EVAL_Exported_Functions
  * @{
  */

#endif /* __STM8_EVAL_H */
/**
  * @}
  */


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
