/* ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename    : AS2.c
**     Project     : ProcessorExpert
**     Processor   : MKL25Z128VLK4
**     Component   : AsynchroSerial
**     Version     : Component 02.602, Driver 01.01, CPU db: 3.00.000
**     Compiler    : GNU C Compiler
**     Date/Time   : 2015-05-10, 10:35, # CodeGen: 11
**     Abstract    :
**         This component "AsynchroSerial" implements an asynchronous serial
**         communication. The component supports different settings of
**         parity, word width, stop-bit and communication speed,
**         user can select interrupt or polling handler.
**         Communication speed can be changed also in runtime.
**         The component requires one on-chip asynchronous serial channel.
**     Settings    :
**         Serial channel              : UART2
**
**         Protocol
**             Width                   : 8 bits
**             Stop bits               : 1
**             Parity                  : none
**             Breaks                  : Disabled
**             Input buffer size       : 0
**             Output buffer size      : 0
**
**         Registers
**
**
**
**         Used pins:
**         ----------------------------------------------------------
**           Function | On package           |    Name
**         ----------------------------------------------------------
**            Input   |     16               |  ADC0_DM3/ADC0_SE7a/PTE23/TPM2_CH1/UART2_RX
**            Output  |     15               |  ADC0_DP3/ADC0_SE3/PTE22/TPM2_CH0/UART2_TX
**         ----------------------------------------------------------
**
**
**
**     Contents    :
**         RecvChar        - byte AS2_RecvChar(AS2_TComData *Chr);
**         SendChar        - byte AS2_SendChar(AS2_TComData Chr);
**         GetCharsInRxBuf - word AS2_GetCharsInRxBuf(void);
**         GetCharsInTxBuf - word AS2_GetCharsInTxBuf(void);
**
**     Copyright : 1997 - 2013 Freescale Semiconductor, Inc. All Rights Reserved.
**     SOURCE DISTRIBUTION PERMISSIBLE as directed in End User License Agreement.
**     
**     http      : www.freescale.com
**     mail      : support@freescale.com
** ###################################################################*/
/*!
** @file AS2.c
** @version 01.01
** @brief
**         This component "AsynchroSerial" implements an asynchronous serial
**         communication. The component supports different settings of
**         parity, word width, stop-bit and communication speed,
**         user can select interrupt or polling handler.
**         Communication speed can be changed also in runtime.
**         The component requires one on-chip asynchronous serial channel.
*/         
/*!
**  @addtogroup AS2_module AS2 module documentation
**  @{
*/         

/* MODULE AS2. */

#include "AS2.h"

#ifdef __cplusplus
extern "C" {
#endif 


#define OVERRUN_ERR      0x01U         /* Overrun error flag bit    */
#define FRAMING_ERR      0x02U         /* Framing error flag bit    */
#define PARITY_ERR       0x04U         /* Parity error flag bit     */
#define CHAR_IN_RX       0x08U         /* Char is in RX buffer      */
#define FULL_TX          0x10U         /* Full transmit buffer      */
#define RUNINT_FROM_TX   0x20U         /* Interrupt is in progress  */
#define FULL_RX          0x40U         /* Full receive buffer       */
#define NOISE_ERR        0x80U         /* Noise error flag bit      */
#define IDLE_ERR         0x0100U       /* Idle character flag bit   */
#define BREAK_ERR        0x0200U       /* Break detect              */

LDD_TDeviceData *ASerialLdd2_DeviceDataPtr; /* Device data pointer */
static word SerFlag;                   /* Flags for serial communication */
                                       /* Bits: 0 - OverRun error */
                                       /*       1 - Framing error */
                                       /*       2 - Parity error */
                                       /*       3 - Char in RX buffer */
                                       /*       4 - Full TX buffer */
                                       /*       5 - Unused */
                                       /*       6 - Full RX buffer */
                                       /*       7 - Noise error */
                                       /*       8 - Idle character  */
                                       /*       9 - Break detected  */
                                       /*      10 - Unused */
                                       /*      11 - Unused */
static AS2_TComData BufferRead;        /* Input char for SCI communication */
static AS2_TComData OutBuffer;         /* Output char for SCI communication */

/*
** ===================================================================
**     Method      :  HWEnDi (component AsynchroSerial)
**
**     Description :
**         Enables or disables the peripheral(s) associated with the bean.
**         The method is called automatically as a part of the Enable and 
**         Disable methods and several internal methods.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void HWEnDi(void)
{
  (void)ASerialLdd2_ReceiveBlock(ASerialLdd2_DeviceDataPtr, &BufferRead, 1U); /* Receive one data byte */
}

/*
** ===================================================================
**     Method      :  AS2_RecvChar (component AsynchroSerial)
**     Description :
**         If any data is received, this method returns one character,
**         otherwise it returns an error code (it does not wait for
**         data). This method is enabled only if the receiver property
**         is enabled.
**         [Note:] Because the preferred method to handle error and
**         break exception in the interrupt mode is to use events
**         <OnError> and <OnBreak> the return value ERR_RXEMPTY has
**         higher priority than other error codes. As a consequence the
**         information about an exception in interrupt mode is returned
**         only if there is a valid character ready to be read.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * Chr             - Pointer to a received character
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_RXEMPTY - No data in receiver
**                           ERR_BREAK - Break character is detected
**                           (only when the <Interrupt service> property
**                           is disabled and the <Break signal> property
**                           is enabled)
**                           ERR_COMMON - common error occurred (the
**                           <GetError> method can be used for error
**                           specification)
** ===================================================================
*/
byte AS2_RecvChar(AS2_TComData *Chr)
{
  byte Result = ERR_OK;                /* Return error code */
  LDD_SERIAL_TError SerialErrorMask;   /* Serial error mask variable */

  ASerialLdd2_Main(ASerialLdd2_DeviceDataPtr);
  if (ASerialLdd2_GetError(ASerialLdd2_DeviceDataPtr, &SerialErrorMask) == ERR_OK) { /* Get error state */
    if (SerialErrorMask != 0U) {
      Result = ERR_COMMON;             /* If yes then set common error value */
    } else {
      if (ASerialLdd2_GetReceivedDataNum(ASerialLdd2_DeviceDataPtr) == 0U) { /* Is not received char? */
        return ERR_RXEMPTY;            /* If yes then error is returned */
      }
    }
  }
  *Chr = BufferRead;                   /* Read the char */
  (void)ASerialLdd2_ReceiveBlock(ASerialLdd2_DeviceDataPtr, &BufferRead, 1U); /* Receive one data byte */
  ASerialLdd2_Main(ASerialLdd2_DeviceDataPtr);
  return Result;                       /* Return error code */
}

/*
** ===================================================================
**     Method      :  AS2_SendChar (component AsynchroSerial)
**     Description :
**         Sends one character to the channel. If the component is
**         temporarily disabled (Disable method) SendChar method only
**         stores data into an output buffer. In case of a zero output
**         buffer size, only one character can be stored. Enabling the
**         component (Enable method) starts the transmission of the
**         stored data. This method is available only if the
**         transmitter property is enabled.
**     Parameters  :
**         NAME            - DESCRIPTION
**         Chr             - Character to send
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_TXFULL - Transmitter is full
** ===================================================================
*/
byte AS2_SendChar(AS2_TComData Chr)
{
  AS2_TComData TmpChr = OutBuffer;     /* Save OutBuffer value */

  ASerialLdd2_Main(ASerialLdd2_DeviceDataPtr);
  OutBuffer = Chr;                     /* Save character */
  if (ASerialLdd2_SendBlock(ASerialLdd2_DeviceDataPtr, (LDD_TData *)&OutBuffer, 1U) == ERR_BUSY) { /* Send one data byte */
    OutBuffer = TmpChr;                /* If is device busy, restore OutBuffer value */
    return ERR_TXFULL;
  }
  ASerialLdd2_Main(ASerialLdd2_DeviceDataPtr);
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  AS2_GetCharsInRxBuf (component AsynchroSerial)
**     Description :
**         Returns the number of characters in the input buffer. This
**         method is available only if the receiver property is enabled.
**     Parameters  : None
**     Returns     :
**         ---             - The number of characters in the input
**                           buffer.
** ===================================================================
*/
word AS2_GetCharsInRxBuf(void)
{
  ASerialLdd2_Main(ASerialLdd2_DeviceDataPtr);
  return (word)ASerialLdd2_GetReceivedDataNum(ASerialLdd2_DeviceDataPtr); /* Return number of chars in the receive buffer */
}

/*
** ===================================================================
**     Method      :  AS2_GetCharsInTxBuf (component AsynchroSerial)
**     Description :
**         Returns the number of characters in the output buffer. This
**         method is available only if the transmitter property is
**         enabled.
**     Parameters  : None
**     Returns     :
**         ---             - The number of characters in the output
**                           buffer.
** ===================================================================
*/
word AS2_GetCharsInTxBuf(void)
{
  ASerialLdd2_Main(ASerialLdd2_DeviceDataPtr);
  return ((word)(ASerialLdd2_GetSentDataNum(ASerialLdd2_DeviceDataPtr) != 0x00U) ? 0U:1U); /* Return number of chars in the transmit buffer */
}

/*
** ===================================================================
**     Method      :  AS2_Init (component AsynchroSerial)
**
**     Description :
**         Initializes the associated peripheral(s) and the bean internal 
**         variables. The method is called automatically as a part of the 
**         application initialization code.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void AS2_Init(void)
{
  SerFlag = 0x00U;                     /* Reset flags */
  ASerialLdd2_DeviceDataPtr = ASerialLdd2_Init(NULL); /* Calling init method of the inherited component */
  HWEnDi();                            /* Enable/disable device according to status flags */
}

/*
** ===================================================================
**     Method      :  AS2_ASerialLdd2_OnBreak (component AsynchroSerial)
**
**     Description :
**         This event is called when a break occurs on the input channel.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void ASerialLdd2_OnBreak(LDD_TUserData *UserDataPtr)
{
  (void)UserDataPtr;                   /* Parameter is not used, suppress unused argument warning */
  SerFlag |= FRAMING_ERR;              /* Set framing error flag */
}


/* END AS2. */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

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