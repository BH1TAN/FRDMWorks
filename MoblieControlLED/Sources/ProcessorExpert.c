/* ###################################################################
**     Filename    : ProcessorExpert.c
**     Project     : ProcessorExpert
**     Processor   : MKL25Z128VLK4
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2015-05-03, 00:15, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file ProcessorExpert.c
** @version 01.01
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup ProcessorExpert_module ProcessorExpert module documentation
**  @{
*/         
/* MODULE ProcessorExpert */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "Green.h"
#include "BitIoLdd1.h"
#include "AD1.h"
#include "AdcLdd1.h"
#include "AS1.h"
#include "ASerialLdd1.h"
#include "AS2.h"
#include "ASerialLdd2.h"
#include "Red.h"
#include "BitIoLdd2.h"
#include "Blue.h"
#include "BitIoLdd3.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

/* User includes (#include below this line is not maintained by Processor Expert) */
word result[3],check[6];
byte ch;
unsigned short lednum=1;
/*lint -save  -e970 Disable MISRA rule (6.3) checking. */

void ShutDownLed(void)
{
	Green_SetVal();
	Red_SetVal();
	Blue_SetVal();
}

int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */
	unsigned int i;
	
  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  /* For example: for(;;) { } */
  while(1)
  {
	  AD1_Measure(TRUE);
	  AS2_RecvChar(&ch);
	  switch(ch)
	  {
	  case 'A':
		  AS1_SendChar(result[0]>>8);
		  AS1_SendChar(result[0]);
		  AS2_SendChar(result[0]>>8);
		  AS2_SendChar(result[0]);
		  check[0]=AS2_SendChar(result[0]);
		  ch = 0;
		  break;
		  
	  case 'H':
		  AS1_SendChar(result[1]>>8);
		  AS1_SendChar(result[1]);
		  AS2_SendChar(result[1]>>8);
		  check[1]=AS2_SendChar(result[1]);
		  ch = 0;
		  break;
		  
	  case 'T':
		  AS1_SendChar(result[2]>>8);
		  AS1_SendChar(result[2]);
		  AS2_SendChar(result[2]>>8);
		  AS2_SendChar(result[2]);
		  check[2]=AS2_SendChar(result[2]);
		  ch = 0;
		  break;
		  
	  case 'R':
		  ShutDownLed();
		  lednum=2;
		  ch=0;
		  break;
		  
	  case 'G':
		  ShutDownLed();
		  lednum=1;
		  ch=0;
		  break;
		  
	  case 'B':
		  ShutDownLed();
		  lednum=3;
		  ch=0;
		  break;
	  }
	  
	  if(lednum==1)
		  Green_NegVal();
	  else if(lednum==2)
		  Red_NegVal();
	  else if(lednum==3)
		  Blue_NegVal();
	  else ;
	  
	  for(i=0;i<100000;i++) ;
  }
  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END ProcessorExpert */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.3 [05.08]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
