/**************************************************************************************************
Filename:       SampleApp.h
Revised:        $Date: 2007-10-27 17:22:23 -0700 (Sat, 27 Oct 2007) $
Revision:       $Revision: 15795 $

Description:    This file contains the Sample Application definitions.


Copyright 2007 Texas Instruments Incorporated. All rights reserved.

IMPORTANT: Your use of this Software is limited to those specific rights
granted under the terms of a software license agreement between the user
who downloaded the software, his/her employer (which must be your employer)
and Texas Instruments Incorporated (the "License").  You may not use this
Software unless you agree to abide by the terms of the License. The License
limits your use, and you acknowledge, that the Software may not be modified,
copied or distributed unless embedded on a Texas Instruments microcontroller
or used solely and exclusively in conjunction with a Texas Instruments radio
frequency transceiver, which is integrated into your product.  Other than for
the foregoing purpose, you may not use, reproduce, copy, prepare derivative
works of, modify, distribute, perform, display or sell this Software and/or
its documentation for any purpose.

YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
PROVIDED 揂S IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, 
INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE, 
NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

Should you have any questions regarding your right to use this Software,
contact Texas Instruments Incorporated at www.TI.com. 
**************************************************************************************************/

#ifndef SAMPLEAPP_H
#define SAMPLEAPP_H

#ifdef __cplusplus
extern "C"
{
#endif
  
  /*********************************************************************
  * INCLUDES
  */
#include "ZComDef.h"
#include "hal_adc.h"
  
  /*********************************************************************
  * CONSTANTS
  */
  
  // These constants are only for example and should be changed to the
  // device's needs
#define SAMPLEAPP_ENDPOINT           20
  
#define SAMPLEAPP_PROFID             0x0F08
#define SAMPLEAPP_DEVICEID           0x0001
#define SAMPLEAPP_DEVICE_VERSION     0
#define SAMPLEAPP_FLAGS              0
  
#define SAMPLEAPP_MAX_CLUSTERS       4
#define SAMPLEAPP_PERIODIC_CLUSTERID 1
#define SAMPLEAPP_FLASH_CLUSTERID     2
#define SAMPLEAPP_P2P_CLUSTERID    3       //添加标志号需要在.c文件的结构体添加
#define  SAMPLEAPP_COM_CLUSTERID  4     //透传
  // Send Message Timeout
#define SAMPLEAPP_SEND_PERIODIC_MSG_TIMEOUT   10000     // Every 5 seconds
  
  // Application Events (OSAL) - These are bit weighted definitions.
#define SAMPLEAPP_SEND_PERIODIC_MSG_EVT       0x0001    //事件号，一个任务可有多个事件
  //#define SAMPLEAPP_SEND_PERIODIC_MSG_EVT2      0x0002
  
  // Group ID for Flash Command
#define SAMPLEAPP_FLASH_GROUP                  0x0001           //组ID
  
  // Flash Command Duration - in milliseconds
#define SAMPLEAPP_FLASH_DURATION               1000
  
  /*
  定义人体红外热释电传感器IO  P0.4
  */
#define PEOPLE P0_4 
  //气体检测
#define HAL_ADC_DEC_064     0x00    /* Decimate by 64 : 8-bit resolution */
#define HAL_ADC_CHANNEL_6          0x06
  
  
  /*********************************************************************
  * MACROS
  */
  
  /*********************************************************************
  * FUNCTIONS
  */
  
  /*
  * Task Initialization for the Generic Application
  */
  extern void SampleApp_Init( uint8 task_id );
  
  /*
  * Task Event Processor for the Generic Application
  */
  extern UINT16 SampleApp_ProcessEvent( uint8 task_id, uint16 events );
  
  /*********************************************************************
  *********************************************************************/
  
#ifdef __cplusplus
}
#endif

#endif /* SAMPLEAPP_H */
