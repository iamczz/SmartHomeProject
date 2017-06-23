/**************************************************************************************************
Filename:       SampleApp.c
Revised:        $Date: 2009-03-18 15:56:27 -0700 (Wed, 18 Mar 2009) $
Revision:       $Revision: 19453 $

Description:    Sample Application (no Profile).


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
PROVIDED �AS IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
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

/*********************************************************************
This application isn't intended to do anything useful, it is
intended to be a simple example of an application's structure.

This application sends it's messages either as broadcast or
broadcast filtered group messages.  The other (more normal)
message addressing is unicast.  Most of the other sample
applications are written to support the unicast message model.

Key control:
SW1:  Sends a flash command to all devices in Group 1.
SW2:  Adds/Removes (toggles) this device in and out
of Group 1.  This will enable and disable the
reception of the flash command.
*********************************************************************/

/*********************************************************************
* INCLUDES
*/
#include "OSAL.h"
#include "ZGlobals.h"
#include "AF.h"
#include "aps_groups.h"
#include "ZDApp.h"

#include "SampleApp.h"
#include "SampleAppHw.h"

#include "OnBoard.h"

/* HAL */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "MT_UART.h"
#include "MT_APP.h"
#include "MT.h"

#include "ds18b20.h"
#include "LCD.h"
# include "stdlib.h"
#include "string.h"
#include "DHT11.h"

/*********************************************************************
* MACROS
*/

/*********************************************************************
* CONSTANTS
*/

/*********************************************************************
* TYPEDEFS
*/


/*********************************************************************
* GLOBAL VARIABLES
*/
uint8 AppTitle[] = "ALD2530 UART"; //Ӧ�ó�������
uint8 static LedState = 0; // ���浱ǰLed ��״̬
uint8 device=0x03;         //�ն���������    1:��̨ 2������  3������
        




// This list should be filled with Application specific Cluster IDs.
const cId_t SampleApp_ClusterList[SAMPLEAPP_MAX_CLUSTERS] =
{
  SAMPLEAPP_PERIODIC_CLUSTERID,
  SAMPLEAPP_FLASH_CLUSTERID  ,
  SAMPLEAPP_P2P_CLUSTERID  ,
  SAMPLEAPP_COM_CLUSTERID
   
};

const SimpleDescriptionFormat_t SampleApp_SimpleDesc =
{
  SAMPLEAPP_ENDPOINT,              //  int Endpoint;
  SAMPLEAPP_PROFID,                //  uint16 AppProfId[2];
  SAMPLEAPP_DEVICEID,              //  uint16 AppDeviceId[2];
  SAMPLEAPP_DEVICE_VERSION,        //  int   AppDevVer:4;
  SAMPLEAPP_FLAGS,                 //  int   AppFlags:4;
  SAMPLEAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)SampleApp_ClusterList,  //  uint8 *pAppInClusterList;
  SAMPLEAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)SampleApp_ClusterList   //  uint8 *pAppInClusterList;
};
 
endPointDesc_t SampleApp_epDesc;

/*********************************************************************
* EXTERNAL VARIABLES
*/

/*********************************************************************
* EXTERNAL FUNCTIONS
*/

/*********************************************************************
* LOCAL VARIABLES
*/

uint8 SampleApp_TaskID;   // Task ID for internal task/event processing
// This variable will be received when
// SampleApp_Init() is called.
devStates_t SampleApp_NwkState;

uint8 SampleApp_TransID;  // This is the unique message ID (counter)

afAddrType_t SampleApp_Periodic_DstAddr;    //�㲥
afAddrType_t SampleApp_Flash_DstAddr;      //�鲥
afAddrType_t SampleApp_P2P_DstAddr;         //�㲥


aps_Group_t SampleApp_Group;

uint8 SampleAppPeriodicCounter = 0;
uint8 SampleAppFlashCounter = 0;

/*********************************************************************
* LOCAL FUNCTIONS
*/
void SampleApp_HandleKeys( uint8 shift, uint8 keys );
void SampleApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );
void SampleApp_SendPeriodicMessage(byte data[8]);
void SampleApp_SendFlashMessage( uint16 flashTime );
void SampleApp_Send_P2P_Message( void );

//���պ���
void SampleApp_P2P_ReciveMessage(void);
//����͸��
void SampleApp_SerialCMD(mtOSALSerialData_t *cmdMsg);
//��ʼ������ OLED
void OLED_Init();
//���ڷ��������
uint8 wifiSendCmd(char* cmd,char*result,int timeOut);
//��ȡ��ȼ����    ģ��������
uint16 ReadGasData(void);



/*********************************************************************
* NETWORK LAYER CALLBACKS
*/

/*********************************************************************
* PUBLIC FUNCTIONS
*/

/*********************************************************************
* @fn      SampleApp_Init
*
* @brief   Initialization function for the Generic App Task.
*          This is called during initialization and should contain
*          any application specific initialization (ie. hardware
*          initialization/setup, table initialization, power up
*          notificaiton ... ).
*
* @param   task_id - the ID assigned by OSAL.  This ID should be
*                    used to send messages and set timers.
*
* @return  none
*/
void SampleApp_Init( uint8 task_id )
{ 
  SampleApp_TaskID = task_id;
  SampleApp_NwkState = DEV_INIT;
  SampleApp_TransID = 0;
  
  //------------------------���ô���---------------------------------
  MT_UartInit();                    //���ڳ�ʼ��
  MT_UartRegisterTaskID(task_id);   //ע�ᴮ������
//  HalUARTWrite(0,"UartInit OK\n", sizeof("UartInit OK\n"));
  //------------OLED��ʼ��--------------------------------------------
  OLED_Init();
   //HalLetdIni ();
 //--------------��ʼ������-----�������P0.4--�¶�P0.7--��ʪ��P0.5----����P0.6-------------------------------------
  P0SEL &=0x0f;     //��ʼ��P0.4~P0.7 
  P0DIR &=0x0f;  //P0.4~P0.7����Ϊ�����  

#if defined ( BUILD_ALL_DEVICES )
  if ( readCoordinatorJumper() )
    zgDeviceLogicalType = ZG_DEVICETYPE_COORDINATOR;
  else
    zgDeviceLogicalType = ZG_DEVICETYPE_ROUTER;
#endif // BUILD_ALL_DEVICES
  
#if defined ( HOLD_AUTO_START )
  // HOLD_AUTO_START is a compile option that will surpress ZDApp
  //  from starting the device and wait for the application to
  //  start the device.
  ZDOInitDevice(0);
#endif
  
  // Setup for the periodic message's destination address
  // Broadcast to everyone
  SampleApp_Periodic_DstAddr.addrMode = (afAddrMode_t)AddrBroadcast;
  SampleApp_Periodic_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_Periodic_DstAddr.addr.shortAddr = 0xFFFF;
  
  // Setup for the flash command's destination address - Group 1
  SampleApp_Flash_DstAddr.addrMode = (afAddrMode_t)afAddrGroup;
  SampleApp_Flash_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_Flash_DstAddr.addr.shortAddr = SAMPLEAPP_FLASH_GROUP;
  
  // P2P
  SampleApp_P2P_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
  SampleApp_P2P_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_P2P_DstAddr.addr.shortAddr = 0x0000;//Э������ַ

  
  // Fill out the endpoint description.
  SampleApp_epDesc.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_epDesc.task_id = &SampleApp_TaskID;
  SampleApp_epDesc.simpleDesc
    = (SimpleDescriptionFormat_t *)&SampleApp_SimpleDesc;
  SampleApp_epDesc.latencyReq = noLatencyReqs;
  
  // Register the endpoint description with the AF
  afRegister( &SampleApp_epDesc );
  
  // Register for all key events - This app will handle all key events
  RegisterForKeys( SampleApp_TaskID );
  
  // By default, all devices start out in Group 1
  SampleApp_Group.ID = 0x0001;
  osal_memcpy( SampleApp_Group.name, "Group 1", 7  );
  aps_AddGroup( SAMPLEAPP_ENDPOINT, &SampleApp_Group );
  
#if defined ( LCD_SUPPORTED )
  HalLcdWriteString( "SampleApp", HAL_LCD_LINE_1 );
#endif
}


/*********************************************************************
* @fn      SampleApp_ProcessEvent
*
* @brief   Generic Application Task event processor.  This function
*          is called to process all events for the task.  Events
*          include timers, messages and any other user defined events.
*
* @param   task_id  - The OSAL assigned task ID.
* @param   events - events to process.  This is a bit map and can
*                   contain more than one event.
*
* @return  none
*/
uint16 SampleApp_ProcessEvent( uint8 task_id, uint16 events )
{
  afIncomingMSGPacket_t *MSGpkt;
  (void)task_id;  // Intentionally unreferenced parameter
  
  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( SampleApp_TaskID );
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
      {
        
       case CMD_SERIAL_MSG: //�����յ����ݺ���MT_UART�㴫�ݹ��������ݣ������䷽�����գ�����ʱ������ MT ������ݣ�
             SampleApp_SerialCMD((mtOSALSerialData_t *)MSGpkt);
            break;
        
      case KEY_CHANGE:
        SampleApp_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
        break;
        
        // Received when a messages is received (OTA) for this endpoint
      case AF_INCOMING_MSG_CMD:
        SampleApp_MessageMSGCB( MSGpkt );
        break;
        
        // Received whenever the device changes state in the network
      case ZDO_STATE_CHANGE:
        SampleApp_NwkState = (devStates_t)(MSGpkt->hdr.status);
        if ( 
             (SampleApp_NwkState == DEV_ZB_COORD) ||
            (SampleApp_NwkState == DEV_ROUTER)
              || (SampleApp_NwkState == DEV_END_DEVICE) )
        {            
          
          // Start sending the periodic message in a regular interval.
          osal_start_timerEx( SampleApp_TaskID,
                             SAMPLEAPP_SEND_PERIODIC_MSG_EVT,
                             SAMPLEAPP_SEND_PERIODIC_MSG_TIMEOUT );
          
        }
        else
        {
          // Device is no longer in the network
        }       
        
        break;
        
      default:
        break;
      }
      
      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );
      
      // Next - if one is available
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( SampleApp_TaskID );
    }
    
    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }
  
  // Send a message out - This event is generated by a timer
  //  (setup in SampleApp_Init()).
  if ( events & SAMPLEAPP_SEND_PERIODIC_MSG_EVT )
  {
    // Send the periodic message
    //    SampleApp_SendPeriodicMessage();
    if (SampleApp_NwkState == DEV_END_DEVICE) 
    SampleApp_Send_P2P_Message();
    
    // Setup to send message again in normal period (+ a little jitter)
    osal_start_timerEx( SampleApp_TaskID, SAMPLEAPP_SEND_PERIODIC_MSG_EVT,
                       (SAMPLEAPP_SEND_PERIODIC_MSG_TIMEOUT + (osal_rand() & 0x00FF)) );
    return (events ^ SAMPLEAPP_SEND_PERIODIC_MSG_EVT);
  }
  
  
  // Discard unknown events
  return 0;
}

void OLED_Init(void){
    LCD_Init();                      //oled ��ʼ��  
    LCD_Fill(0xff);                  //��ȫ�� 
    LCD_CLS();   
    
    LCD_P6x8Str(28,0,"temp");  
    LCD_P6x8Str(55,0,"parm");
    LCD_P6x8Str(84,0,"LED");
//    LCD_P6x8Str(10,0,"Welcom!") ;
    
    //��ʼ������
    LCD_P6x8Str(32,2,"27C");  
    LCD_P6x8Str(55,2,"66H");
    LCD_P6x8Str(84,2,"off");
    
    LCD_P6x8Str(32,4,"28C");  
    LCD_P6x8Str(55,4,"alam");
    LCD_P6x8Str(84,4,"off");
    
     LCD_P6x8Str(32,6,"28C");  
    LCD_P6x8Str(55,6,"alam");
    LCD_P6x8Str(84,6,"off");
    
    LCD_P6x8Str(0,2,"Flow:");      
    LCD_P6x8Str(0,4,"Door:"); 
    LCD_P6x8Str(0,6,"Keic:")  ;
   
//    LCD_P6x8Str(0,7,"Alarm") ;   
//    while(1);
    

  }



void SampleApp_HandleKeys( uint8 shift, uint8 keys )
{
  (void)shift;  // Intentionally unreferenced parameter
  
  if ( keys & HAL_KEY_SW_6 )         //S1��
  {
    //    SampleApp_SendFlashMessage( SAMPLEAPP_FLASH_DURATION );
    
#if defined(ZDO_COORDINATOR) // Э�������͹㲥
 /* ��һλ��ѡ���նˡ��ڶ�λ������led(1Ϊon ��0Ϊoff).����λ����չ��  
    **************/
    byte data[8];
    char  *str="31000000";
    for(int i=0;i<8;i++){
      data[i]=str[i]-'0';    //char ����ת����int����   
    }
     SampleApp_SendPeriodicMessage(data); //�㲥
#else // ·�������ն˲ŷ�������
    SampleApp_SendFlashMessage(0); // ���鲥��ʽ������
#endif
    
  }
  
  if ( keys & HAL_KEY_SW_1 )         //S2��
  {
    /* The Flashr Command is sent to Group 1.
    * This key toggles this device in and out of group 1.
    * If this device doesn't belong to group 1, this application
    * will not receive the Flash command sent to group 1.
    */
    aps_Group_t *grp;
    grp = aps_FindGroup( SAMPLEAPP_ENDPOINT, SAMPLEAPP_FLASH_GROUP );
    if ( grp )
    {
      // Remove from the group
//      HalUARTWrite(0, "RemoveGroud:", 12); // ��ʾ��Ϣ
      aps_RemoveGroup( SAMPLEAPP_ENDPOINT, SAMPLEAPP_FLASH_GROUP );
    }
    else
    {
      // Add to the flash group
//      HalUARTWrite(0, "AddGroud:", 9); // ��ʾ��Ϣ
      aps_AddGroup( SAMPLEAPP_ENDPOINT, &SampleApp_Group );
      
    }
  }
}

/*********************************************************************
* LOCAL FUNCTIONS
*/

/*********************************************************************
* @fn      SampleApp_MessageMSGCB
*
* @brief   Data message processor callback.  This function processes
*          any incoming data - probably from other devices.  So, based
*          on cluster ID, perform the intended action.
*
* @param   none
*
* @return  none
*/
void SampleApp_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
  /*16����תASCII��*/
  uint8 asc_16[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'},i;
  uint8 data;
  
  unsigned char *tem ;   //�¶�
 
  switch ( pkt->clusterId )
  {  
    
  case SAMPLEAPP_PERIODIC_CLUSTERID:     //�㲥
     HalUARTWrite(0,"recive brocart\n",16);
     HalUARTWrite(0, pkt->cmd.Data, pkt->cmd.DataLength);     //����ڵ���յ��㲥��Ϣ 
   
    //�ı��ն�1 LED1
    if(device==0x01&&pkt->cmd.Data[0]==1 ){
      if( pkt->cmd.Data[1]==1){  
        HalLedSet(HAL_LED_1, HAL_LED_MODE_ON);
         LedState =1;
      }
      else{
        HalLedSet(HAL_LED_1, HAL_LED_MODE_OFF);
        LedState =0;
      }
    }
    
    //�ı��ն�2��led
    if(device==0x02&&pkt->cmd.Data[0]==2){ 
     if( pkt->cmd.Data[1]==1){  
        HalLedSet(HAL_LED_1, HAL_LED_MODE_ON);
         LedState =1;
      }
      else{
        HalLedSet(HAL_LED_1, HAL_LED_MODE_OFF);
        LedState =0;
      }
    }
    
    //�ı��ն�3��led  keichen
    if(device==0x03&&pkt->cmd.Data[0]==3){ 
      if( pkt->cmd.Data[1]==1){  
        HalLedSet(HAL_LED_1, HAL_LED_MODE_OFF);
         LedState =1;
      }
      else{
        HalLedSet(HAL_LED_1, HAL_LED_MODE_ON);
        LedState =0;
      }
    }
    
    break;
    
  case SAMPLEAPP_FLASH_CLUSTERID:      //�鲥
//      HalUARTWrite(0, "123456", 6); // ��ʾ��Ϣ   
    
//    data = (uint8)pkt->cmd.Data[0]; 
////    HalUARTWrite(0, &data, 1);
//    if(LedState == 0)
//      HalLedSet(HAL_LED_1, HAL_LED_MODE_OFF);
//    else
//      HalLedSet(HAL_LED_1, HAL_LED_MODE_ON);
    break;
    
    
  case SAMPLEAPP_P2P_CLUSTERID:
    // temp=pkt->srcAddr.addr.shortAddr;  //�������ݰ���16λ�̵�ַ
    //   HalUARTWrite(0,"Hello",5);
    //   HalUARTWrite(0,"\n",1) ;  
     HalUARTWrite(0, pkt->cmd.Data, pkt->cmd.DataLength);     //����ڵ���Ϣ ͸����wifiģ��
    
     //OLED��ʾ����ʾ
#if 0
    switch (pkt->cmd.Data[0]){ 
       /******�ն�1Flower** *****/
//      LCD_Fill(0xff);                  //��ȫ�� 
//      LCD_CLS();    
    case '1':  
      if(pkt->cmd.Data[1]==1) 
        LCD_P6x8Str(84,2,"on ");
      else   LCD_P6x8Str(84,2,"off");
      /*********DH11��ʪ��************/
      tem[0]=pkt->cmd.Data[4]; 
      tem[1]=pkt->cmd.Data[5];
      LCD_P6x8Str(32,2,tem+'C'); 
      /***ʪ��****/
      tem[0]=pkt->cmd.Data[6]; 
      tem[1]=pkt->cmd.Data[7];
      LCD_P6x8Str(55,2,tem+'H');      
        break;
        
        /******�ն�2*Outdoor******/
    case '2':      
        /*************LED1************/ 
       if(pkt->cmd.Data[1]==1) 
       LCD_P6x8Str(84,4,"off");
       else   LCD_P6x8Str(84,4,"on ");
       /*********DS18B20************/
       tem[0]=pkt->cmd.Data[2]; 
       tem[1]=pkt->cmd.Data[3];
        LCD_P6x8Str(32,4,tem+'C');  
     /*********�������************/ 
      if(pkt->cmd.Data[4]==1)   //����
         LCD_P6x8Str(55,4,"alam ");     
      else   LCD_P6x8Str(55,4,"off ");     //û��

      break;
      
         /******�ն�3*keichen******/
    case '3':  
        /*************LED1************/ 
       if(pkt->cmd.Data[1]==1) 
       LCD_P6x8Str(84,6,"off");
       else   LCD_P6x8Str(84,6,"on ");
         /*********DS18B20************/
       tem[0]=pkt->cmd.Data[2]; 
       tem[1]=pkt->cmd.Data[3];
       LCD_P6x8Str(32,6,tem+'C'); 
       tem[0]=pkt->cmd.Data[4]; 
       tem[1]=pkt->cmd.Data[5];
       tem[2]=pkt->cmd.Data[6];
       
        LCD_P6x8Str(55,6,tem); 
      
      break;
         /******����*******/
    default :       
//      HalUARTWrite(0,"NotEndDevice\n",13);//������ʾ
//      HalUARTWrite(0, pkt->cmd.Data, pkt->cmd.DataLength);  
//      HalUARTWrite(0,"\n",1);  
      break;
      
      
    }

#endif
//    
    
    //    /*****���̵�ַ�ֽ⣬ASC���ӡ************/
    //    HalUARTWrite(0,&asc_16[temp/4096],1);
    //    HalUARTWrite(0,&asc_16[temp%4096/256],1);
    //    HalUARTWrite(0,&asc_16[temp%256/16],1);
    //    HalUARTWrite(0,&asc_16[temp%16],1);
    //    HalUARTWrite(0,"\n",1);
    break;
    
  }
}
  
  
  
  /*********************************************************************
  * @fn      SampleApp_SendPeriodicMessage
  *
  * @brief   Send the periodic message.
  *
  * @param   none
  *
  * @return  none
  */
  //�㲥  
  void SampleApp_SendPeriodicMessage( byte data[8] )
  {
    
    if ( AF_DataRequest( &SampleApp_Periodic_DstAddr, 
                        &SampleApp_epDesc,
                        SAMPLEAPP_PERIODIC_CLUSTERID,
                        8,
                        data,
                        &SampleApp_TransID,
                        AF_DISCV_ROUTE,
                        AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
    {
    }
    else
    {
      // Error occurred in request to send.
    }
  }
  
  /*********************************************************************
  * @fn      SampleApp_SendFlashMessage
  *
  * @brief   Send the flash message to group 1.
  *
  * @param   flashTime - in milliseconds
  *
  * @return  none
  */
  void SampleApp_SendFlashMessage( uint16 flashTime )
  {
    LedState = ~LedState;  //Led
//  HalUARTWrite(0, "GT:", 3); // ��ʾ��������   
    if ( AF_DataRequest( &SampleApp_Flash_DstAddr, 
                        &SampleApp_epDesc,
                        SAMPLEAPP_FLASH_CLUSTERID,
                        1,
                        &LedState,
                        &SampleApp_TransID,
                        AF_DISCV_ROUTE,
                        AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
    {
      if(LedState == 1) // �л�����״̬��ʾ�û������ɹ�
        HalLedSet(HAL_LED_1, HAL_LED_MODE_ON);
      else
        HalLedSet(HAL_LED_1, HAL_LED_MODE_OFF);
    }
    else
    {
      // Error occurred in request to send.
    }
  }
  
  
  void SampleApp_Send_P2P_Message( void )
  { 
    byte send[10];          //��������  
    /*****�ն�1 Flower*****/  
    if(SampleApp_NwkState==DEV_END_DEVICE &&device==0x01){  
       send[0]=device+'0';        
      //��ȡled״̬  ��ֵ �����͸�Э����
      if(LedState == 0) 
        send[1]=0+'0';
         else send[1]=1+'0';   
      //��ʪ�Ȳ���    
           DHT11();     
      //��ȡdsp18b20�¶�   
          byte str[2];
          byte temp=0;
          temp = ReadDs18B20(); //��ȡ�¶�
          str[0] = temp/10;
          str[1] = temp%10;      
          send[2]=str[0]+'0'; 
          send[3]=str[1]+'0';
                            
      //��ȡ��ʪ�� ���
          send[4]=wendu_shi+'0'; 
          send[5]=wendu_ge+'0';
          send[6]=shidu_shi+'0';
          send[7]=shidu_ge+'0'; 
          
          send[8]='#';
          send[9]='#';        
    }
    
    /*****�ն�2 OutDoor*****/   
    else if(SampleApp_NwkState == DEV_END_DEVICE && device==0x02)   
    {
      send[0]=device+'0';         //��ֵ     
      //��ȡled״̬  ��ֵ �����͸�Э����
      if(LedState == 0) 
        send[1]=0+'0';
         else send[1]=1+'0';      
      //��ȡ�¶� dsp18b20   
      byte str[2];
      byte temp=0;
      temp = ReadDs18B20(); //��ȡ�¶�  
      str[0] = temp/10;
      str[1] = temp%10;  
      send[2]=str[0]+'0'; 
      send[3]=str[1]+'0';               
      //��������� 
      if(PEOPLE == 1)
      {
        MicroWait (10000); // Wait 10ms
        if(PEOPLE == 1)
        {
          send[4]=1+'0';  
          HalLedSet(HAL_LED_2, HAL_LED_MODE_ON);
        }
      }    
      else
      {
        send[4]=0+'0';
        HalLedSet(HAL_LED_2, HAL_LED_MODE_OFF);
      }     
      //Ԥ�����3λ
      send[5]=0+'0'; 
      send[6]=0+'0';
      send[7]=0+'0';
      
      send[8]='#';
      send[9]='#';     
    }  
    
    //**********�ն�3 keichen ****ʹ��ΰ��ʦ�ֵװ�
    else if(SampleApp_NwkState == DEV_END_DEVICE&&device==0x03)   
    {
      send[0]=device+'0';         //��ֵ      
      //��ȡled״̬  ��ֵ �����͸�Э����
      if(LedState == 0) 
        send[1]=0+'0';
         else send[1]=1+'0';   
      
      //��ȡ�¶� dsp18b20   
      byte str[2];
      byte temp=0;
      temp = ReadDs18B20(); //��ȡ�¶�   
      str[0] = temp/10;
      str[1] = temp%10;  
      send[2]=str[0]+'0'; 
      send[3]=str[1]+'0';        
      //������
      uint16 a=ReadGasData();
      send[4]=a/100+'0';
      send[5]=(a%100)/10+'0';
      send[6]=a%10+'0';
       //Ԥ�����һλ
      send[7]=0+'0'; 
      
      send[8]='#';//#
      send[9]='#';   
       
//    HalUARTWrite(0,send, 8);  ���ڲ���        
      if(a>30)      //��������ֵ  
//      {                
//         for(int i=0;i<5;i++){
//           HalLedSet(HAL_LED_2, ~0); 
//             MicroWait (10000); // Wait 10ms
//         } 
//      }
         HalLedSet(HAL_LED_2, HAL_LED_MODE_ON); 
     else
        HalLedSet(HAL_LED_2, HAL_LED_MODE_OFF);     
     
    }        
    
    /*****�����ն��豸*****/
    else         
    {    
      for(int i=0;i<8;i++)
      {send[i]=0+'0'; } 
      send[8]='#';
      send[9]='#';      
    }         
    
    if ( AF_DataRequest( &SampleApp_P2P_DstAddr, &SampleApp_epDesc,
                        SAMPLEAPP_P2P_CLUSTERID,
                        10,
                        send,
                        &SampleApp_TransID,
                        AF_DISCV_ROUTE,
                        AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
    {
          HalUARTWrite(0,send, 10);  //���ڵ���
           HalUARTWrite(0,"\n", 1);  //���ڵ���
    }
    else
    {
      // Error occurred in request to send.
    }
  }




  

//͸��

 void SampleApp_SerialCMD(mtOSALSerialData_t *cmdMsg)
    {
      uint8 i,len,*str=NULL; //len �������ݳ���
      str=cmdMsg->msg; // 
      len=*str; //msg ��ĵ� 1 ���ֽڴ����������ݳ���
      uint8 recive[8];
 
      /********��ӡ�����ڽ��յ������ݣ�������ʾ*********/
      for(i=1;i<=len;i++)  
      {
         recive[i-1]=*(str+i)-'0';      
//        HalUARTWrite(0,str+i,1 );    //���ڵ���  ����ӡ�����ڽ��յ�������
      }                
           
      //������յ�������     Э�����򿪹㲥
      if (SampleApp_NwkState == DEV_ZB_COORD)
        SampleApp_SendPeriodicMessage(recive);     //�㲥wifiģ��͸������������    
    }
  /*********************************************************************
  *********************************************************************/
  

  //��ȡ����
  uint16 ReadGasData(void){
      uint16 reading = 0;
    //  P0DIR &= ~0x40; // ���� P0.6 Ϊ���뷽ʽ
      asm("NOP");asm("NOP");
      
  /* Clear ADC interrupt flag */
    ADCIF = 0;
    ADCCON3 = (0x40 | HAL_ADC_DEC_064 | HAL_ADC_CHANNEL_6);
    /* Wait for the conversion to finish */
    while ( !ADCIF );
      asm("NOP");asm("NOP");
    /* Read the result */
      reading = ADCL;
      reading |= (int16) (ADCH << 8);
      reading >>= 8;
      return reading;
  
  }