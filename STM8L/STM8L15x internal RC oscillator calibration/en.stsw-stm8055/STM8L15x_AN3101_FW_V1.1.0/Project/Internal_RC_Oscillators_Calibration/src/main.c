/**
  ******************************************************************************
  * @file    Project\Internal_RC_Oscillators_Calibration\src\main.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    10/06/2010
  * @brief   Main program body
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
#include "stm8_eval.h"
#include "stm8_eval_lcd.h"
#include "hsi_calibration.h"
#include "lsi_measurement.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Uncomment the line below to run calibration with fixed error
   Comment it to run calibration with the minimum fixed error  */
/* #define USE_HSI_FIXED_ERROR */

/* Uncomment the line below to output HSI on CCO pin: PC4 */
/* #define OUTPUT_HSI_ON_CCO_FOR_DEBUG */

#define TIME_OUT          (uint32_t)20000 /* expressed in µs */
#define ASCII_NUM_0       (uint8_t)48

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t CurrentHSIFrequency = 0;
/* Init HSIFrequencyAfterCalib at the typical value of HSI: 16 MHz */
uint32_t HSIFrequencyAfterCalib = HSI_VALUE;
uint32_t HSIFrequencyDefaultCalib = 0;
ErrorStatus CalibStatus = ERROR;
uint8_t LCDString1[16] = "Before:         ";
uint8_t LCDString2[16] = "After:          ";
/* Init LSIFrequency at the typical value of LSI: 38 KHz */
uint16_t LSIFrequency = LSI_VALUE;

/* Private function prototypes -----------------------------------------------*/
void HSI_Calibration(void);
void LSI_Measurement(void);
ErrorStatus TimeOutConfig(uint32_t timeout, uint8_t* reload, uint8_t* prescaler);
void CLK_Configuration(void);
void GPIO_Configuration(void);
void DisaplyFrequencies(void);

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
  /* Configure clock */
  CLK_Configuration();
  
  /* GPIO configuration */
  GPIO_Configuration();

  /* Run the HSI calibration routine */
  HSI_Calibration();

  /* Run the LSI measurement routine */
  /* Uncomment the line below to run the LSI measurement routine 
     Make sure to comment the above HSI_Calibration(); routine */
  /* LSI_Measurement(); */

  /* Infinite loop */
  while (1)
  {}
}

/**
  * @brief  Calibrates the HSI clock.
  * @param  None.
  * @retval None.
  */
void HSI_Calibration(void)
{
#ifdef USE_HSI_FIXED_ERROR
  /* Calibrate HSI clock with the maximum allowed error in Hz */
  /* Set the maximum value of the error frequency at +/- 60000 Hz -> 0.375 % */
  CalibStatus = HSI_CalibrateFixedError(60000, &CurrentHSIFrequency);
  /* Init the Eval board LCD */
  STM8_EVAL_LCD_Init();
  /* Clear LCD */
  LCD_Clear();
  LCD_SetCursorPos(LCD_LINE1, 0);
  LCD_Print("HSI Calibration");

  if (CalibStatus != ERROR) /* fixed Error */
  {
    /* Turn on LD1 if the calibration has succeeded */
    STM_EVAL_LEDOn(LED1);
    LCD_SetCursorPos(LCD_LINE2, 0);
    LCD_Print("    SUCCESS    ");
  }
  else
  {
    /* Turn on LD3 if the Calibration has failed */
    STM_EVAL_LEDOn(LED3);
    LCD_SetCursorPos(LCD_LINE2, 0);
    LCD_Print("      FAIL     ");
  }

#else /* USE_HSI_FIXED_ERROR isn't defined */
  /* Calibrate HSI clock and return its value (Hz) after calibration  */
  HSIFrequencyAfterCalib = HSI_CalibrateMinError();
  /* Init the Eval board LCD */
  STM8_EVAL_LCD_Init();
  /* Clear LCD*/
  LCD_Clear();
  /* Display HSI frequencies values: before and after calibration */
  DisaplyFrequencies();

  /* Turn on LD1, LD2 and LD3 */
  STM_EVAL_LEDOn(LED1);
  STM_EVAL_LEDOn(LED2);
  STM_EVAL_LEDOn(LED3);
#endif /* USE_HSI_FIXED_ERROR */
}

/**
  * @brief  Configures the IWDG according to measured LSI value.
  * @param  None.
  * @retval None.
  */
void LSI_Measurement(void)
{
  uint8_t reloadvalue = 0;
  uint8_t prescalervalue = 0;

  /* IWDG configuration: IWDG is clocked by LSI, typical value is 38 KHz */
  /* The objective is to get IWDG time out equal to TIME_OUT µs */

  /* Get the measured LSI frequency value */
  /* LSIFrequency is initialized at 38 KHz so if the line below is commented
     the time out will occur a little bit before/after the forecasted time 
     Otherwise the LSI clock is measured and LSIFrequency is updated with the
     current LSI value avoiding error on time out.
     If more accuracy is required the HSI calibration routine should be already 
     run to update HSIFrequencyAfterCalib with a more accurate value of HSI frequency.
  */
  /* Comment then uncomment the following line to notice the difference
     when the LSI clock is already measured 
  */
  LSIFrequency = LSI_FreqMeasure(HSIFrequencyAfterCalib);

  /* Look for IWDG configuration that ensures getting required time out */
  if (TimeOutConfig(TIME_OUT, &reloadvalue, &prescalervalue) != ERROR)
  {
    /* Enable the IWDG */
    IWDG_Enable();
    /* Enable write access to IWDG_PR and IWDG_RLR registers */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    /* Set IWDG prescaler */
    IWDG->PR = prescalervalue;
    IWDG_SetReload(reloadvalue);
    /* Refresh IWDG counter */
    IWDG_ReloadCounter();
  }
  /* Turn on LD4: set at low level LED4_GPIO_PIN (PA7/PH3) pin */
  /* The time during which the (PA7/PH3) pin is at low level represents the time out */
  STM_EVAL_LEDOn(LED4);
}

/**
  * @brief  Computes the IWDG configuration (prescaler and reload registers)
  *    regarding the required time out.
  * @param  timeout: value of time out required (µs).
  * @param  reload: a pointer to the value to be programmed in the reload register PR.
  * @param  prescaler: a pointer to the value to be programmed in the prescaler register PR.
  * @retval ErrorStatus (ERROR or SUCCESS).
  */
ErrorStatus TimeOutConfig(uint32_t timeout, uint8_t* reload, uint8_t* prescaler)
{
  float tmptimeout = 0;
  uint32_t mintimeout = 0;
  uint32_t maxtimeout = 0;
  uint16_t prescalertab[7] = {4, 8, 16, 32, 64, 128, 256};
  uint8_t prescalerindex = 0;
  ErrorStatus status = ERROR;

  while ((prescalerindex < 7) && (status == ERROR))
  {
    mintimeout = (uint32_t) ((uint32_t) 1000000 * ((float) prescalertab[prescalerindex] / LSIFrequency));
    maxtimeout = (uint32_t) ((uint32_t) 1000000 * ((float) 256 * prescalertab[prescalerindex] / LSIFrequency));

    if ((timeout <= maxtimeout) && (timeout > mintimeout))
    {
      *prescaler = prescalerindex;
      tmptimeout = (float) timeout / ((uint32_t) prescalertab[prescalerindex] * 1000000);
      *reload = (uint8_t) ((LSIFrequency * tmptimeout) - 1);
      status = SUCCESS;
    }
    else
    {
      mintimeout = 0;
      maxtimeout = 0;
    }
    prescalerindex++;
  }
  return(status);
}

/**
  * @brief  Configures clock.
  * @param  None.
  * @retval None.
  */
void CLK_Configuration(void)
{
  /* High speed internal clock prescaler: 1 */
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);

#ifdef  OUTPUT_HSI_ON_CCO_FOR_DEBUG
  /* CCO configuration: HSI clock output */
  CLK_CCOConfig(CLK_CCOSource_HSI, CLK_CCODiv_1);
#endif /* OUTPUT_HSI_ON_CCO_FOR_DEBUG */

  /* Enable peripherals clock here... */

}

/**
  * @brief  Configures GPIO.
  * @param  None.
  * @retval None.
  */
void GPIO_Configuration(void)
{
  /* Initialize LEDs mounted on the eval board */
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  STM_EVAL_LEDInit(LED5);

  /* Add user GPIO configuration here... */

#ifdef OUTPUT_HSI_ON_CCO_FOR_DEBUG
  /* Configure CCO Pin: PC4 */
  GPIO_Init(GPIOC, GPIO_Pin_4, GPIO_Mode_Out_PP_Low_Fast);
#endif /* OUTPUT_HSI_ON_CCO_FOR_DEBUG */
}

/**
  * @brief  Displays on LCD HSI values before and after calibration.
  * @param  None.
  * @retval None.
  */
void DisaplyFrequencies(void)
{
  uint8_t frequency8 = 0;
  uint8_t frequency7 = 0;
  uint8_t frequency6 = 0;
  uint8_t frequency5 = 0;
  uint8_t frequency4 = 0;
  uint8_t frequency3 = 0;
  uint8_t frequency2 = 0;
  uint8_t frequency1 = 0;

  frequency8 = (uint8_t) (HSIFrequencyDefaultCalib / 10000000);
  frequency7 = (uint8_t) (HSIFrequencyDefaultCalib % 10000000 / 1000000);
  frequency6 = (uint8_t) (HSIFrequencyDefaultCalib % 1000000 / 100000);
  frequency5 = (uint8_t) (HSIFrequencyDefaultCalib % 100000 / 10000);
  frequency4 = (uint8_t) (HSIFrequencyDefaultCalib % 10000 / 1000);
  frequency3 = (uint8_t) (HSIFrequencyDefaultCalib % 1000 / 100);
  frequency2 = (uint8_t) (HSIFrequencyDefaultCalib % 100 / 10);
  frequency1 = (uint8_t) (HSIFrequencyDefaultCalib % 10);

  LCDString1[7]  = (uint8_t)((uint8_t)(frequency8) + ASCII_NUM_0);
  LCDString1[8]  = (uint8_t)((uint8_t)(frequency7) + ASCII_NUM_0);
  LCDString1[9]  = (uint8_t)((uint8_t)(frequency6) + ASCII_NUM_0);
  LCDString1[10] = (uint8_t)((uint8_t)(frequency5) + ASCII_NUM_0);
  LCDString1[11] = (uint8_t)((uint8_t)(frequency4) + ASCII_NUM_0);
  LCDString1[12] = (uint8_t)((uint8_t)(frequency3) + ASCII_NUM_0);
  LCDString1[13] = (uint8_t)((uint8_t)(frequency2) + ASCII_NUM_0);
  LCDString1[14] = (uint8_t)((uint8_t)(frequency1) + ASCII_NUM_0);

  /* Display default HSI frequency value before calibration on LCD*/
  LCD_SetCursorPos(LCD_LINE1, 0);
  LCD_Print(LCDString1);

  frequency8 = (uint8_t) (HSIFrequencyAfterCalib / 10000000);
  frequency7 = (uint8_t) (HSIFrequencyAfterCalib % 10000000 / 1000000);
  frequency6 = (uint8_t) (HSIFrequencyAfterCalib % 1000000 / 100000);
  frequency5 = (uint8_t) (HSIFrequencyAfterCalib % 100000 / 10000);
  frequency4 = (uint8_t) (HSIFrequencyAfterCalib % 10000 / 1000);
  frequency3 = (uint8_t) (HSIFrequencyAfterCalib % 1000 / 100);
  frequency2 = (uint8_t) (HSIFrequencyAfterCalib % 100 / 10);
  frequency1 = (uint8_t) (HSIFrequencyAfterCalib % 10);

  LCDString2[7]  = (uint8_t)((uint8_t)(frequency8) + ASCII_NUM_0);
  LCDString2[8]  = (uint8_t)((uint8_t)(frequency7) + ASCII_NUM_0);
  LCDString2[9]  = (uint8_t)((uint8_t)(frequency6) + ASCII_NUM_0);
  LCDString2[10] = (uint8_t)((uint8_t)(frequency5) + ASCII_NUM_0);
  LCDString2[11] = (uint8_t)((uint8_t)(frequency4) + ASCII_NUM_0);
  LCDString2[12] = (uint8_t)((uint8_t)(frequency3) + ASCII_NUM_0);
  LCDString2[13] = (uint8_t)((uint8_t)(frequency2) + ASCII_NUM_0);
  LCDString2[14] = (uint8_t)((uint8_t)(frequency1) + ASCII_NUM_0);

  /* Display the HSI frequency value after calibration on LCD */
  LCD_SetCursorPos(LCD_LINE2, 0);
  LCD_Print(LCDString2);
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/**
  * @}
  */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
