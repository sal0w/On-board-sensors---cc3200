//*****************************************************************************
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

//*****************************************************************************
//
// Application Name     - Timer Count Capture
// Application Overview - This application showcases Timer's count capture 
//                        feature to measure frequency of an external signal.
// Application Details  -
// http://processors.wiki.ti.com/index.php/CC32xx_Timer_Count_Capture_Application
// or
// docs\examples\CC32xx_Timer_Count_Capture_Application.pdf
//
//*****************************************************************************

// Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "interrupt.h"
#include "prcm.h"
#include "gpio.h"
#include "utils.h"
#include "timer.h"
#include "rom.h"
#include "rom_map.h"
#include "pin.h"

// Common interface includes
#include "uart_if.h"
#include "pinmux.h"
#include "i2c_if.h"
#include "common.h"
#include "tmp006drv.h"
#include "bma222drv.h"


#define APPLICATION_VERSION     "1.1.1"
#define APP_NAME        "Timer Count Capture"
#define TIMER_FREQ      80000000

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************


#if defined(ccs) || defined(gcc)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif
//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************
signed char cAccXT1,cAccYT1,cAccZT1;


//*****************************************************************************
//
//! Timer interrupt handler
//
//*****************************************************************************



//*****************************************************************************
//
//! Application startup display on UART
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void
DisplayBanner(char * AppName)
{

    Report("\n\n\n\r");
    Report("\t\t *************************************************\n\r");
    Report("\t\t\t  CC3200 %s Application       \n\r", AppName);
    Report("\t\t *************************************************\n\r");
    Report("\n\n\n\r");
}

//*****************************************************************************
//
//! Board Initialization & Configuration
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
void
BoardInit(void)
{
/* In case of TI-RTOS vector table is initialize by OS itself */
#ifndef USE_TIRTOS
  //
  // Set vector table base
  //
#if defined(ccs) || defined(gcc)
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
    MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif
    //
    // Enable Processor
    //
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}


//*****************************************************************************
//
//! Main  Function
//
//*****************************************************************************
int main()
{

	long   lRetVal = -1;
    //
    // Initialize Board configurations
    //
    BoardInit();

    //
    // Pinmux for UART
    //
    PinMuxConfig();

    //
    // Configuring UART
    //
    InitTerm();

    //
    // Display Application Banner
    //
    DisplayBanner(APP_NAME);
   lRetVal = I2C_IF_Open(I2C_MASTER_MODE_FST);
      if(lRetVal < 0)
      {
          ERR_PRINT(lRetVal);
          LOOP_FOREVER();
      }

      //Init Temprature Sensor
      lRetVal = TMP006DrvOpen();
      if(lRetVal < 0)
      {
          ERR_PRINT(lRetVal);
          LOOP_FOREVER();
      }
      //Init Accelerometer Sensor
      lRetVal = BMA222Open();
      if(lRetVal < 0)
      {
          ERR_PRINT(lRetVal);
          LOOP_FOREVER();
      }







    while(1)
    {
        //
        // Report the calculate temperature
        //
    	float fCurrentTemp;
    	TMP006DrvGetTemp(&fCurrentTemp);
    	BMA222ReadNew(&cAccXT1, &cAccYT1, &cAccZT1);
    	//char cTemp = (char)fCurrentTemp;

    	int dtemp =(int)fCurrentTemp;
    	dtemp -= 32;
    	dtemp /= 9;
    	dtemp *= 5;

    	Report("Axis X: %d Y: %d Z: %d \n\n\r",cAccXT1,cAccYT1,cAccZT1);

    	Report("Temperature: %f F\n\n\r",fCurrentTemp);

    	Report("Temperature: %d C (Celsius)\n\n\r",dtemp);



        //
        // Delay loop
        //
        MAP_UtilsDelay(80000000/5);
    }
}
