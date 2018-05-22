/**
  ******************************************************************************
  * @file    Project\Internal_RC_Oscillators_Calibration\src\hsi_calibration.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    10/06/2010
  * @brief   This file provides all the HSI Calibration firmware functions.
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
#include "hsi_calibration.h"

/* Private variables--------------------------------------------------------- */
uint32_t HSICurrentPeriod = 0;
uint32_t HSIMeasuredFrequencyCumul = 0;
uint32_t HSIMeasuredFrequency = 0;
uint8_t HSIPeriodCounter = 0;
extern uint32_t HSIFrequencyDefaultCalib;
extern __IO uint16_t CaptureState;
extern __IO uint32_t Capture;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
extern void CLK_Configuration(void);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Restores clock configuration.
  * @param  None.
  * @retval None.
  */
void CLK_RestoreUserConfiguration(void)
{
  /* Call your default clock configuration here, for example: CLK_Configuration() */
  CLK_Configuration();
}

/**
  * @brief  Calibrates the HSI clock to the minimum calculated error.
  * @param  None.
  * @retval The optimum HSI frequency.
  */
uint32_t HSI_CalibrateMinError(void)
{
  uint32_t optimumfrequency = 0;
  int32_t frequencyerror = 0;
  uint32_t currentfrequencyerror = 0;
  uint32_t minfrequencyerror = 2000000; /* large value */
  uint8_t calibrationvalue = 0;
  uint8_t optimumcalibrationvalue = 0;
  uint8_t trimmingindex = 0, numberofsteps = 0;

#ifndef USE_REFERENCE_LSE
  /* Configure the GPIO ports for HSI calibration process */
  GPIO_ConfigForHSI();
#endif /* USE_REFERENCE_LSE */

  /* Configure clock for HSI calibration */
  CLK_ConfigForHSI();

#ifdef USE_REFERENCE_LSE
  /* Connect LSE clock to Timer 2 channel 1 */
  BEEP_LSClockToTIMConnectCmd(ENABLE);
#endif /* USE_REFERENCE_LSE */

  /* Configure TIM2 for HSI calibration process */
  TIM2_ConfigForHSI();

  /* compute the number of steps to calibrate HSI clock */
  numberofsteps = LOWER_THRESHOLD + UPPER_THRESHOLD + 1;
  /* Get default calibration value (before calibration) substructed by lower threshold (12) */
  calibrationvalue = (uint8_t) (CLK->HSICALR - LOWER_THRESHOLD);

  /* HSI Frequency measurement for numberofsteps values,
  from HSICALR-LOWER_THRESHOLD to HSICALR+UPPER_THRESHOLD */
  for (trimmingindex = 0; trimmingindex < numberofsteps; trimmingindex++)
  {
    /* Set the HSITRIMR register to calibrationvalue to be ready for measurement */
    CLK_AdjustHSICalibrationValue(calibrationvalue);
    /* Start Frequency measurement for current HSITRIM value */
    HSIMeasuredFrequencyCumul = 0;
    HSIPeriodCounter = 0;
    /**************************** START of HSI Measurement ********************/
    while (HSIPeriodCounter <= HSI_PERIOD_NUMBERS)
    {
      CaptureState = 1;
      /* Generate update */
      TIM2_GenerateEvent(TIM2_EventSource_Update);
      /* Clear all TM2 flags */
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
      if (HSIPeriodCounter != 0)
      {
        /* Compute the frequency (the Timer prescaler isn't included) */
        HSICurrentPeriod = (uint32_t) (REFERENCE_FREQUENCY * Capture);
        /* Add the current frequency to previous cumulation */
        HSIMeasuredFrequencyCumul = HSIMeasuredFrequencyCumul + HSICurrentPeriod;
      }
      HSIPeriodCounter++;
    }
    /**************************** END of Measurement **************************/

    /* Compute the HSI frequency average of the the current HSITRIM configuration */
    HSIMeasuredFrequency = (uint32_t)((1 << TIM2_GetPrescaler()) * (HSIMeasuredFrequencyCumul / HSI_PERIOD_NUMBERS));

    /* Compute current frequency error corresponding to the current HSITRIM value */
    frequencyerror = (HSIMeasuredFrequency - HSI_VALUE);
    currentfrequencyerror = ABS_RETURN(frequencyerror);

    if (trimmingindex == LOWER_THRESHOLD)
    {
      HSIFrequencyDefaultCalib = HSIMeasuredFrequency;
    }
    /* Get the nearest frequency value to 16 MHz */
    if (minfrequencyerror > currentfrequencyerror)
    {
      minfrequencyerror = currentfrequencyerror;
      optimumcalibrationvalue = calibrationvalue;
      optimumfrequency = HSIMeasuredFrequency;
    }
    /* Increment calibration value */
    calibrationvalue++;
  }

  /* Set HSITRIM bits corresponding to the nearest frequency */
  CLK_AdjustHSICalibrationValue(optimumcalibrationvalue);

  /* Restore user clock configuration */
  CLK_RestoreUserConfiguration();
  /* Return the HSI frequency after calibration */
  return (optimumfrequency);
}

/**
  * @brief  Calibrates the HSI clock with the maximum allowed error value fixed by the user.
  *   If this value was not found, this function sets the HSI frequency to default value.
  * @param  MaxAllowedError: maximum absolute value allowed of the HSI frequency
                            error given in Hz.
  * @param  Freq: pointer to an uint32_t variable that will contain the value of
                 the HSI frequency after calibration.
  * @retval ErrorStatus:
  *             - SUCCESS: a frequency error =< MaxAllowedError was found.
  *             - ERROR: a frequency error =< MaxAllowedError was not found.
  */
ErrorStatus HSI_CalibrateFixedError(uint32_t MaxAllowedError, uint32_t* Freq)
{
  int32_t frequencyerror = 0;
  uint32_t currentfrequencyerror = 0;
  uint8_t trimmingindex = 0;
  int8_t calibrationtab[21] = {0, -1, 1, -2, 2, -3, 3, -4, 4, -5, 5, -6, 6, -7, 7, -8, 8, -9, 9, -10, 10};
  uint8_t calibrationvalue = 0;
  uint8_t mediumcalibrationvalue = 0, numberofsteps = 0;
  ErrorStatus calibrationstatus = ERROR;

#ifndef USE_REFERENCE_LSE
  /* Configure the GPIO ports for HSI calibration process */
  GPIO_ConfigForHSI();
#endif /* USE_REFERENCE_LSE */

  /* Configure clock for HSI calibration */
  CLK_ConfigForHSI();

#ifdef USE_REFERENCE_LSE
  /* Connect LSE clock to Timer 2 channel 1 */
  BEEP->CSR1 |= BEEP_CSR1_MSR;
#endif /* USE_REFERENCE_LSE */

  /* Configure TIM2 for HSI calibration process */
  TIM2_ConfigForHSI();

  /* compute the number of steps to calibrate HSI clock */
  numberofsteps = (uint8_t) (LOWER_THRESHOLD + UPPER_THRESHOLD + 1);

  /* Initialize the medium calibration value */
  mediumcalibrationvalue = (uint8_t )(CLK->HSICALR + ((UPPER_THRESHOLD - LOWER_THRESHOLD) / 2));

  /* RC Frequency measurement for different values HSICALR - LOWER_THRESHOLD to HSICALR + UPPER_THRESHOLD */
  for (trimmingindex = 0; trimmingindex < numberofsteps; trimmingindex++)
  {
    /* Set the calibration value */
    calibrationvalue =  (uint8_t) (mediumcalibrationvalue + calibrationtab[trimmingindex]);
    /* Set the HSITRIMR register to calibrationvalue to be ready for measurement */
    CLK_AdjustHSICalibrationValue(calibrationvalue);

    HSIMeasuredFrequencyCumul = 0;
    HSIPeriodCounter = 0;
    /**************************** START of HSI Measurement ********************/
    while (HSIPeriodCounter <= HSI_PERIOD_NUMBERS)
    {
      CaptureState = 1;
      /* Generate update */
      TIM2_GenerateEvent(TIM2_EventSource_Update);
      /* Clear all TM2 flags */
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
      if (HSIPeriodCounter != 0)
      {
        /* Compute the frequency (the Timer prescaler isn't included) */
        HSICurrentPeriod = (uint32_t) (REFERENCE_FREQUENCY * Capture);
        /* Add the current frequency to previous cumulation */
        HSIMeasuredFrequencyCumul = HSIMeasuredFrequencyCumul + HSICurrentPeriod;
      }
      HSIPeriodCounter++;
    }
    /**************************** END of Measurement **************************/

    /* Compute the HSI frequency average of the the current HSITRIM configuration */
    HSIMeasuredFrequency = (uint32_t)((1 << TIM2_GetPrescaler()) * (HSIMeasuredFrequencyCumul / HSI_PERIOD_NUMBERS));

    /* Compute current frequency Error corresponding to the current HSITRIM value */
    frequencyerror = (HSIMeasuredFrequency - HSI_VALUE);
    currentfrequencyerror = ABS_RETURN(frequencyerror);

    /* Check if frequency error is less or equal to value set by the user */
    if (currentfrequencyerror <= MaxAllowedError)
    {
      calibrationstatus = SUCCESS; /* The calibration has succeeded */
      break; /* stop searching and measurments */
    }
  }
  /* Save the HSI measured value */
  *Freq = HSIMeasuredFrequency;

  /* Check if the frequency error set by the user was not found */
  if (calibrationstatus == ERROR)
  {
    /* Set the HSITRIMR register to default value */
    CLK_AdjustHSICalibrationValue(CLK->HSICALR);
  }

  /* Restore user clock configuration */
  CLK_RestoreUserConfiguration();

  /* Return the calibration status: failed or succeed */
  return (calibrationstatus);
}

/**
  * @brief  Configures the TIM2 in input capture to measure HSI frequency.
  * @param  None.
  * @retval None.
  */
void TIM2_ConfigForHSI(void)
{
  uint8_t icfilter = 0;

  /* DeInt TIM2 registers */
  TIM2_DeInit();
  /* Configure TIM2 presclaer, The TIM2 counter clock is HSI/prescaler */
  TIM2_PrescalerConfig(TIM2_COUNTER_PRESCALER, TIM2_PSCReloadMode_Immediate);
  /* TIM2 configuration: Input Capture mode */
  /* Configure TIM2 channel 1 in input capture mode */
  /* The signal in input is divided and not filtered */
  /* The capture occurs when a rising edge is detected on TIM2 channel 1 */
  TIM2_ICInit(TIM2_Channel_1, TIM2_ICPolarity_Rising, TIM2_ICSelection_DirectTI,
              TIM2_IC_DIVIDER, icfilter);
}

/**
  * @brief  Configures the clock: set HSI clock, HSI divider to 1, enable TIM2 and BEEP clocks
  * @param  None.
  * @retval None.
  */
void CLK_ConfigForHSI(void)
{
#ifdef USE_REFERENCE_LSE
  uint32_t counter = 0, waitforstab = 0x3C702;
#endif /* USE_REFERENCE_LSE */

  /* High Speed Internal clock divider: 1 */
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);

  /* Select HSI as system clock source  */
  CLK_SYSCLKSourceSwitchCmd(ENABLE);
  CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);
  while (CLK_GetSYSCLKSource() != CLK_SYSCLKSource_HSI)
  {}
  /* Wait for HSIRDY flag to be reset */
  while (CLK_GetFlagStatus(CLK_FLAG_HSIRDY) == RESET);

#ifdef USE_REFERENCE_LSE
  /* Enable LSE clock */
  CLK_LSEConfig(CLK_LSE_ON);
  /* Wait for LSERDY flag to be reset */
  while (CLK_GetFlagStatus(CLK_FLAG_LSERDY) == RESET);

  /* Wait for BEEP switch busy flag to be reset */
  while (CLK_GetFlagStatus(CLK_FLAG_BEEPSWBSY) == SET);
  /* Select LSE clock as source for BEEP */
  CLK_BEEPClockConfig(CLK_BEEPCLKSource_LSE);
  /* Enable BEEP clock to get write access for BEEP registers */
  CLK_PeripheralClockConfig(CLK_Peripheral_BEEP, ENABLE);

  /* wait for stabilzation of LSE clock */
  while (counter < waitforstab)
  {
    counter++;
  }
#endif /* USE_REFERENCE_LSE */

  /* Enable TIM2 clock */
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);
}

#ifndef USE_REFERENCE_LSE
/**
  * @brief  Configures the GPIO: configure each used I/O in the suitable setting
  * @param  None.
  * @retval None.
  */
void GPIO_ConfigForHSI(void)
{
  /* GPIOB configuration: TIM2 channel 1: PB0 */
  GPIO_Init(GPIOB, GPIO_Pin_0, GPIO_Mode_In_FL_No_IT);
}
#endif /* USE_REFERENCE_LSE */
/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
