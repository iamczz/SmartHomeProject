#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"	 
#include "esp8266.h"
#include <string.h>

u8 Wifibuf[1024]={'0'};
u16 WifiDatalen=0;
u8 zigbeebuf[1024]={'0'};
u16 zigbeeDatalen=0;

static void system_init(void);
static void HAL_Init(void);

 
int main(void)
{	 
	
	
//	u8 Wifibuf[1024]={'0'};
//	u16 WifiDatalen=0;
//	u8 zigbeebuf[1024]={'0'};
//	u16 zigbeeDatalen=0;
	u8* pWifibuf=NULL;
	system_init(); //ϵͳ��ʼ��
	HAL_Init();//����ģ���ʼ��
	printf("wait connect\r\n");
	while(1)
	{
		delay_ms(10);
		USART2_ReceiveData(Wifibuf,&WifiDatalen);
		if(WifiDatalen > 5)
		{
			if(NULL != strstr((const char*)Wifibuf,(const char*)"CONNECT"))
			{
				memset(Wifibuf,0,WifiDatalen);
				break;
			}
		}
		memset(Wifibuf,0,WifiDatalen);
	}//�ȴ��ֻ�����
	while(1)
	{
		delay_ms(10);
		USART2_ReceiveData(Wifibuf,&WifiDatalen);
		if(WifiDatalen > 5)
		{
			if(NULL != strstr((const char*)Wifibuf,(const char*)"CONNECT"))
			{
				memset(Wifibuf,0,WifiDatalen);
				break;
			}
		}
		memset(Wifibuf,0,WifiDatalen);
	}//�ȴ��ֻ�����
	
	USART2_ReceiveData(Wifibuf,&WifiDatalen);
	memset(Wifibuf,0,WifiDatalen);
	printf("connected\r\n");
	while(1)
	{
		
		USART2_ReceiveData(Wifibuf,&WifiDatalen); //ɨ��WiFiģ���Ƿ��յ�����
		if(WifiDatalen >= 6) //WiFiģ���յ�����
		{
			printf("I get WiFi data%s\r\n",Wifibuf);//��ӡ�յ�����Ϣ
			//��ȡ��������
			pWifibuf=strstr((const char*)Wifibuf,":");
			delay_ms(10);
			if(NULL != pWifibuf) //��Ч����
			{
				pWifibuf++;
				USART3_SendData((const unsigned char*)pWifibuf,strlen((const char*)pWifibuf));//�������ݸ�zigbeeģ��
			}
			else
			{
				;
			}
			
			memset(Wifibuf,0,sizeof(Wifibuf));//���������
			
		}
		else
		{
			;
		}
		
		USART3_ReceiveData(zigbeebuf,&zigbeeDatalen); //ɨ��zigbeeģ���Ƿ��յ�����
		if(zigbeeDatalen >= 6) //zigbeeģ���յ�����
		{
			printf("I get zigbee data%s\r\n",zigbeebuf);//���ڴ�ӡ�յ���zigbee����
//			memset(buf,0,1024);
			delay_ms(10);
			ESP8266_IntoCIPSEND();//����͸��ģʽ
			delay_ms(100);
			ESP8266_SendDataToPhone((const char *)zigbeebuf);//ͨ��WiFiģ������ݷ��͵��ֻ�
			memset(zigbeebuf,0,sizeof(zigbeebuf));//���������
		}
		else
		{
			;
		}
		delay_ms(5);
		
	}
	return 0;
}



static void system_init(void)
{
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	USART1_Init(115200);	 	//���ڳ�ʼ��Ϊ115200
	USART2_Init(115200);	 	//���ڳ�ʼ��Ϊ115200
	USART3_Init(115200);	 	//���ڳ�ʼ��Ϊ115200
	
}

static void HAL_Init(void)
{
	ESP8266_Init();
//	LED_Init();
}

