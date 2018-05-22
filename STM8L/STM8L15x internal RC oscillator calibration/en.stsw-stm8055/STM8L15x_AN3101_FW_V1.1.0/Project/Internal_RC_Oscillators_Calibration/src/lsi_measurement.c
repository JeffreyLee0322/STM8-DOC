/**
  ******************************************************************************
  * @file    Project\Internal_RC_Oscillators_Calibration\src\lsi_measurement.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    10/06/2010
  * @brief   This file provides all the LSI Measurement firmware functions.
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

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"
#include "lsi_measurement.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t LSICurrentPeriod = 0;
uint32_t LSIMeasuredFrequencyCumul = 0;
uint16_t LSIMeasuredFrequency = 0;
uint8_t LSIPeriodCounter = 0;
extern __IO uint16_t CaptureState;
extern __IO uint32_t Capture;

/* Private function prototypes -----------------------------------------------*/
extern void CLK_Configuration(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Measures the LSI clock frequency.
  * @param  None.
  * @retval The LSI frequency value.
  */
uint16_t LSI_FreqMeasure(uint32_t HSIFrequency)
{
  /* Configure clock for LSI measurement */
  CLK_ConfigForLSI();

  /* Connect LSI clock to Timer 2 channel 1 */
  BEEP_LSClockToTIMConnectCmd(ENABLE);

  /* Configure TIM2 for LSI measurement process */
  TIM2_ConfigForLSI();

  LSIPeriodCounter = 0;
  /**************************** START of LSI Measurement **********************/
  while (LSIPeriodCounter <= LSI_PERIOD_NUMBERS)
  {
    CaptureState = 1;
    /* Clear all TM2 flags */
    TIM2_GenerateEvent(TIM2_EventSource_Update);
    TIM2->SR1 = 0;
    TIM2->SR2 = 0;
    /* Enable capture 1 interrupt */
    TIM2_ITConfig(TIM2_IT_CC1, ENABLE);
    /* Enable TIM2 */
    TIM2_Cmd(ENABLE);
    /* Enable global interrupts */
    enableInterrupts();
    while (CaptureState != 255);
    /* Disable global interrupts */
    disableInterrupts();
    if (LSIPeriodCounter != 0)
    {
      /* Compute the frequency value */
      LSICurrentPeriod = (uint32_t) 8 * (HSIFrequency / Capture);
      /* Add the current frequency to previous cumulation */
      LSIMeasuredFrequencyCumul = LSIMeasuredFrequencyCumul + LSICurrentPeriod;
    }
    LSIPeriodCounter++;
  }
  /**************************** END of LSI Measurement ************************/

  /* Compute the average of LSI frequency value */
  LSIMeasuredFrequency = (uint16_t) (LSIMeasuredFrequencyCumul / LSI_PERIOD_NUMBERS);

  /* Restore user clock configuration */
  CLK_Configuration();

  /* Return the LSI frequency */
  return (uint16_t)(LSIMeasuredFrequency);
}

/**
  * @brief  Configures the TIM 2 channel 1 in input capture to measure the LSI 
  *         clock frequency.
  * @param  None.
  * @retval None.
  */
void TIM2_ConfigForLSI(void)
{
  uint8_t icfilter = 0;

  /* DeInt TIM2 registers */
  TIM2_DeInit();

  /* TIM2 configuration: Input Capture mode */
  /* Configure TIM2 channel 1 in input capture mode */
  /* The signal in input is divided and not filtered */
  /* The capture occurs when a rising edge is detected on TIM2 channel 1 */
  TIM2_ICInit(TIM2_Channel_1, TIM2_ICPolarity_Rising, TIM2_ICSelection_DirectTI,
              TIM2_ICPSC_DIV8, icfilter);
}

/**
  * @brief  Configures the clock: set HSI clock, HSI divider to 1, enable TIM2 clock
  * @param  None.
  * @retval None.
  */
void CLK_ConfigForLSI(void)
{
  /* DeInit clock configuration */
  CLK_DeInit();

  /* High Speed Internal clock divider: 1 */
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);

  /* Select HSI as system clock source */
  CLK_SYSCLKSourceSwitchCmd(ENABLE);
  CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);
  while (CLK_GetSYSCLKSource() != CLK_SYSCLKSource_HSI)
  {}
  /* Wait for HSIRDY flag to be reset */
  while (CLK_GetFlagStatus(CLK_FLAG_HSIRDY) == RESET);
  /* Enable LSI clock */
  CLK_LSICmd(ENABLE);
  /* Wait for LSI clock to be ready */
  while (CLK_GetFlagStatus(CLK_FLAG_LSIRDY) == RESET);

  /* Wait for BEEP switch busy flag to be reset */
  while (CLK_GetFlagStatus(CLK_FLAG_BEEPSWBSY) == SET);
  /* Select LSI clock as source for BEEP */
  CLK_BEEPClockConfig(CLK_BEEPCLKSource_LSI);

  /* Enable TIM2 clock */
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);
  /* Enable BEEP clock to get write access for BEEP registers */
  CLK_PeripheralClockConfig(CLK_Peripheral_BEEP, ENABLE);
}

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
