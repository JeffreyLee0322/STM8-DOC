/** @page Internal RC oscillators calibration application note

  @par Firmware description

   This application note describes a firmware that:
     1- calibrates the High-Speed Internal RC oscillator (HSI) using an accurate 
        reference frequency.
     2- measures the Low-Speed Internal RC oscillator (LSI)

  By default the firmware calibrates the HSI clock with minimum error method.
  Once the calibration has finished, the HSI values before and after calibration
  are displayed on LCD and the three leds LD1, LD2 and LD3 are on.
  When the fixed error method is selected and if the calibration routine 
  succeeded finding the reqquired error, a success message is displayed on LCD
  and the green led LD1 is on. Otherwise, a fail message  is displayed on LCD 
  and the red led LD3 is on.
  When the LSI measurement routine is slected, the Independent Watchdog time out 
  can be checked by measuring the time during which LED4_GPIO_PIN (PA7 for 
  STM8L1526-EVAL and PH3 for STM8L1528-EVAL) is in low state.

  @par Directory contents

  - Project\\Internal RC oscillators calibration\\inc
        - hsi_calibration.h              HSI calibration configuration file
        - lsi_measurement.h              LSI measurement configuration file
        - stm8l15x_conf.h                Library Configuration file
        - stm8l15x_it.h                  Interrupt routines declaration

  - Project\\Internal RC oscillators calibration\\src
        - main.c                         Main program
        - hsi_calibration.c              HSI calibration routine
        - lsi_measurement.c              LSI measurement routine 
        - stm8l15x_it.c                  Interrupt routines source
        - stm8_interrupt_vector.c        Interrupt handlers
        
  - Project\\Internal RC oscillators calibration\\RIDE
        - stm8l1526-eval.rapp            Contains the Ride workspace for medium 
                                         density devices
        - stm8l1528-eval.rapp            Contains the Ride workspace for high 
                                         density devices
        - project.rprj                   Contains the Ride project

  - Project\\Internal RC oscillators calibration\\STVD\\Cosmic
          - project.stw                  Contains the STVD workspace
          - stm8l1526-eval.stp           Contains the STVD project for medium 
                                         density devices
          - stm8l1528-eval.stp           Contains the STVD project for high
                                         density devices
          - stm8_interrupt_vector.c      Interrupt handlers
          
  - Project\\Internal RC oscillators calibration\\STVD\\Raisonance
          - project.stw                  Contains the STVD workspace
          - stm8l1526-eval.stp           Contains the STVD project for medium 
                                         density devices
          - stm8l1528-eval.stp           Contains the STVD project for high
                                         density devices

  - Project\\Internal RC oscillators calibration\\EWSTM8
          - stm8l1526-eval.ewd           Contains project settings for debugger(C-SPY)
                                         for medium density devices
          - stm8l1526-eval.ewp           Contains IAR Embedded Workbench project
                                         for medium density devices
          - stm8l1528-eval.ewd           Contains project settings for debugger(C-SPY)
                                         for high density devices
          - stm8l1528-eval.ewp           Contains IAR Embedded Workbench project
                                         for high density devices
          - project.eww                  Contains EWSTM8 Workspace


  @par Hardware and Software environment

  - This application runs on STM8L15x High-Density and Medium-Density Devices.

  - This application has been tested with STMicroelectronics STM8L1528-EVAL 
    (STM8L15x High-Density devices) and STM8L1526-EVAL (STM8L15x Medium-Density 
    devices) evaluation boards and can be easily tailored to any other

  - STM8L1526-EVAL and STM8L1528-EVAL Set-up
     - Make sure that the LCD glass daughter board is mounted in IO position.
     - Dot matrix LCD
     - Leds: LD1, LD2, LD3 and LD4
     - By default, the LSE method is used so no hardware connection is required
       (the 32.768 KHz quart is already connected in STM8L152X-EVAL).
       In case of using other reference frequency, connect the source to PB0
       (The source voltage levels have to correspond to levels allowed for this pin).

 @par How to use it with STVD ?

  - Open the STVD workspace
  - Rebuild all files: Project->Rebuild all
  - Load project image: Debug->Start/Stop Debug Session
  - Run program: Debug->Run (F5)

 @par How to use it with RIDE ?

  - Open the RIDE workspace (project.rprj)
  - Choose the desired project related to the used product from the list: 
  in the "view Project Explorer" --> Right click on the desired project 
  and select "Set as StartUp application" 
  - Rebuild all files: : in the "view Project Explorer" Right click on 
  the active application and select "Build" 
  - Load project image: Debug->Load(Ctrl+L)
  - Run program: Debug->Start (Ctrl+D)
  
 @par How to use it with EWSTM8 ?

  - Open the EWSTM8 workspace
  - Rebuild all files: Project-> Rebuild all. 
  - Load project image: Project-> Download and Debug
  - Run program: Debug->Go (Ctrl+F5)
 
 @note
  - Medium-Density devices are STM8L15x microcontrollers where the 
    Flash memory density ranges between 16 and 32 Kbytes.
  - Medium density Plus devices are STM8L151R6, STM8L152R6 microcontrollers 
    where the Flash memory density is fixed and equal to 32 Kbytes and with 
    wider range of peripheral and features than the medium density devices. 
  - High-Density devices are STM8L15x microcontrollers where the 
    Flash memory density is 64 Kbytes and with the same peripheral set than 
    Medium Density Plus devices.
    
  */

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
