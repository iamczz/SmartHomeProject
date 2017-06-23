#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "adc.h"
#include "htu21d.h" 
#include "BH1750.h"
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
//    u16 adcx;
//	u8 i=0;
//	 float temp=0;
//	//u16 Temperature=0;
//	u8 receivebuf[200]={0};
//	HTU21DdataMode mode=TemperatureMode;
//	mode=HumidityMode;
//	
//	delay_init();	    	 //��ʱ������ʼ��	  
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
//	USART1_Init(115200);	 	//���ڳ�ʼ��Ϊ115200
// 	LED_Init();			     //LED�˿ڳ�ʼ��
////	LCD_Init();			 	
//// 	Adc_Init();		  		//ADC��ʼ��
//	IIC_Init();
//	BH1750_Init();
//	USART1_SendData("Hello\n",6);
	HTU21D_Test();
	while(1)
	{
		adcx=BH1750_ReadData();
		BH1750_Cal(adcx);
		printf("\r\n\r\n\r\n");
		delay_ms(500);
	}
	while(1)
	{
		
		//////////////////////////////////////////////////////////////////////////////////////
		printf("/////////////////////////////////////\n");
		adcx=Get_Adc_Average(ADC_Channel_0,3);
		temp=(float)adcx*(3.3/4096);
		printf("ADC_Channel_0 is %.2f\n",temp);
		
		adcx=Get_Adc_Average(ADC_Channel_1,3);
		temp=(float)adcx*(3.3/4096);
		printf("ADC_Channel_1 is %.2f\r\n",temp);
		
		adcx=Get_Adc_Average(ADC_Channel_4,3);
		temp=(float)adcx*(3.3/4096);
		printf("ADC_Channel_4 is %.2f\r\n",temp);
		adcx=Get_Adc_Average(ADC_Channel_5,3);
		temp=(float)adcx*(3.3/4096);
		printf("ADC_Channel_5 is %.2f\r\n",temp);
		LED0=!LED0;
		delay_ms(1000);	
	}
 }

