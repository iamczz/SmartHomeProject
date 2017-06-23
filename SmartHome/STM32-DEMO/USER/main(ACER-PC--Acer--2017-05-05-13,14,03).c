#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"	 
#include "adc.h"
#include "htu21d.h" 
#include "BH1750.h"
#include "esp8266.h"
#include "packdata.h"
/************************************************
 ALIENTEK ս��STM32F103������ʵ��17
 ADC ʵ�� 
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/


 
 int main(void)
 {	 
	TypedefData Pckdata;
	SensorMSG Sensor[5]={
	{Sensor[0].SensorName=0x01,Sensor[0].DataH=0x34,Sensor[0].DataL=0x67},
	{Sensor[1].SensorName=0x02,Sensor[1].DataH=0x56,Sensor[1].DataL=0x45},
	{Sensor[2].SensorName=0x03,Sensor[2].DataH=0x78,Sensor[2].DataL=0x54},
	{Sensor[3].SensorName=0x04,Sensor[3].DataH=0x90,Sensor[3].DataL=0x22},
	{Sensor[4].SensorName=0x05,Sensor[4].DataH=0x23,Sensor[4].DataL=0x48}

	};
	Pckdata.SensorMSGPck=Sensor;
	unsigned char  *p=NULL;
	USART1_Init(115200);
	printf("hello\r\n");
	while(1)
	{	
		
		p=packMSG(Pckdata);
		USART1_SendData((const u8 *)p,p[4]+4);
		while(1);
	}
	return 0;

}

