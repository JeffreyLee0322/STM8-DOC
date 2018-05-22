/**
  ******************************************************************************
  * @file    Project\Internal_RC_Oscillators_Calibration\inc\hsi_calibration.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    10/06/2010
  * @brief   Header for hsi_calibration.h 
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
#ifndef __HSI_CALIBRATION_H
 #define __HSI_CALIBRATION_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Comment this line if the LSE clock is not used as the reference frequency */
#define USE_REFERENCE_LSE

#ifdef USE_REFERENCE_LSE
 /* The TIM2 counter is clocked by HSI/prescaler = 16MHz/1 = 16 MHz. 
	 So the TIM2 minimum frequency is 16MHz/65535 ~ 244 Hz */
 #define TIM2_COUNTER_PRESCALER  TIM2_Prescaler_1
 /* The signal in input capture is divided by 8 */
 #define TIM2_IC_DIVIDER         TIM2_ICPSC_DIV8
 /* The LSE is divided by 8 => LSE/8 = 32768/8 = 4096 */
 #define REFERENCE_FREQUENCY     (uint32_t) 4096 /* The reference frequency value in Hz */
#else
 /* If reference frequency is not the LSE and less than 250 Hz (The TIM2 min frequency),
   the TIM2 counter prescaler should be updated. 
	 Otherwise (the reference frequency is an external clock and greater than 250 Hz),
	 the TIM2 counter prescaler should be TIM2_Prescaler_1 */
 /* For example, If reference frequency is 50 Hz, the counter prescaler should be 8. 
	 In that case the minimum frequency is 16MHz/(8*65535) ~ 31 Hz */
 #define TIM2_COUNTER_PRESCALER  TIM2_Prescaler_8
 /* The signal in input capture is divided by 1 */
 #define TIM2_IC_DIVIDER         TIM2_ICPSC_DIV1
 /* The reference frequency is 50 Hz */
 #define REFERENCE_FREQUENCY     (uint32_t)50 /* The reference frequency value in Hz */
/* If the reference frequency is lower than 31Hz, the counter prescaler should be
   updated regarding the reference frequency in input */
#endif /* USE_REFERENCE_LSE */

#define HSI_PERIOD_NUMBERS    10

/* Exported macro ------------------------------------------------------------*/
#define ABS_RETURN(x)         (x < 0) ? (-x) : x
/* HSI calibration is performed between CLK->HSICALR + LOWER_THRESHOLD and 
   CLK->HSICALR + UPPER_THRESHOLD */
#define LOWER_THRESHOLD      12
#define UPPER_THRESHOLD       8

/* Exported functions ------------------------------------------------------- */
uint32_t HSI_CalibrateMinError(void);
ErrorStatus HSI_CalibrateFixedError(uint32_t MaxAllowedError, uint32_t* Freq);
void CLK_ConfigForHSI(void);
void TIM2_ConfigForHSI(void);
#ifndef USE_REFERENCE_LSE
void GPIO_ConfigForHSI(void);
#endif /* USE_REFERENCE_LSE */
void CLK_RestoreUserConfiguration(void);

#endif /* __HSI_CALIBRATION_H */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
