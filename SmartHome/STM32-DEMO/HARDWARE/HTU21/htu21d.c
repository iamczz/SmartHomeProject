/**
  ******************************************************************************
  * @file    ucos_task_app.c
  * @author  Hoire Application Team
  * @version V2.1.0
  * @date    20-December-2016
  * @brief   WSN�ڵ�����ص�Ӧ�ó�����-Ӧ�ò�
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "htu21d.h"
#include "delay.h"
//#include "myiic.h"

#define DEBUG_HTU21D
#ifdef DEBUG_HTU21D
#include "usart.h"
#endif
/*******************************************************************************************

				�����λ����ÿ�δ򿪵�Դǰ��Ҫ��������������ϵͳ
				������ϵͳ��ʼ��ʼ�������ָ�Ĭ�ϵ�����״̬ 
******************************************************************************************/
void Reset_HTU21D(void)
{
		IIC_Start();                  //��ʼ�ź�
		
		IIC_Send_Byte(0x80);	   //���͵�ַ+д���� ���λΪ0��д
	
		IIC_Wait_Ack();	 //�ȴ�����ACK�ظ�
	
		IIC_Send_Byte(0xfe);	   //���������λ����
	
		IIC_Wait_Ack();	 //�ȴ�����ACK�ظ�
	
		IIC_Stop();//����һ��ֹͣ����	 
	
		delay_ms(20);
}


/*******************************************************************************************

				�����û��Ĵ��� ��Ҫ�����û��Ĵ�����ֵ �����߼������������λд��Ĵ���
				�����û��Ĵ�����7��0λΪ00 ��RH��T �ķֱ��ʷֱ�Ϊ12λ��14λ
******************************************************************************************/
void Write_User_Register(void)
{
	u8 dat;
	
	IIC_Start();                  //��ʼ�ź�
	
	IIC_Send_Byte(0x80);	   //���͵�ַ+д���� ���λΪ0��д
	
	IIC_Wait_Ack();	 //�ȴ�����ACK�ظ�
	
	IIC_Send_Byte(0xe7);	   //���Ͷ��û��Ĵ�������
	
	IIC_Wait_Ack();	 //�ȴ�����ACK�ظ�
	
	IIC_Start();                  //��ʼ�ź�
	
	IIC_Send_Byte(0x81);	   //���͵�ַ+������ ���λΪ1�Ƕ�
	
	IIC_Wait_Ack();	 //�ȴ�����ACK�ظ�
	
	dat=IIC_Read_Byte(0);	//�����û��Ĵ�����ֵ��������NACK�ź�  
	
	IIC_Start();                  //��ʼ�ź�
	
	IIC_Send_Byte(0x80);	   //���͵�ַ+д���� ���λΪ0��д
	
	IIC_Wait_Ack();	 //�ȴ�����ACK�ظ�
	
	IIC_Send_Byte(0xe6);	   //����д�û��Ĵ�������
	
	IIC_Wait_Ack();	 //�ȴ�����ACK�ظ� 
	dat&=~(1<<7);
	dat&=~(1<<0);
	IIC_Send_Byte(dat);	   //�����û��Ĵ�����7��0λΪ00 ��RH��T �ķֱ��ʷֱ�Ϊ12λ��14λ
	
	IIC_Wait_Ack();	 //�ȴ�����ACK�ظ� 
	
	IIC_Stop();//����һ��ֹͣ����
	
	delay_ms(20);
}

/****************************************************************************************

						�ڷ�����ģʽ�¶�ȡ�¶Ȼ�ʪ�ȵ�ֵ��ʪ�ȷֱ���Ϊ12λ���¶ȷֱ���Ϊ14λ
						target Ϊ 1   ��ȡʪ��ֵ 
									 Ϊ 0   ��ȡ�¶�ֵ
****************************************************************************************/
u16 Read_Temperature_humidity(unsigned char target)
{
		u8 DATA_MSB,DATA_LSB;
	
		u16 result;
	
		IIC_Start();                  //��ʼ�ź�
	
		IIC_Send_Byte(0x80);	   //���͵�ַ+д���� ���λΪ0��д
	
		IIC_Wait_Ack();	 //�ȴ�����ACK�ظ� 
	
		if(target)//�ж�������¶Ȼ���ʪ��
		
			IIC_Send_Byte(0xe5);	   //��������ģʽ�����²���ʪ��RH����
	
		else 	IIC_Send_Byte(0xe3);	   //��������ģʽ�����²����¶�T����
		
		IIC_Wait_Ack();	 //�ȴ�����ACK�ظ� 
		
		IIC_Start();                  //��ʼ�ź�
		
		IIC_Send_Byte(0x81);	   //���͵�ַ+������ ���λΪ1�Ƕ�
		
		IIC_Wait_Ack();	 //�ȴ�����ACK�ظ� 
		
		delay_ms(100);
		
		DATA_MSB=IIC_Read_Byte(1);//������λ�ֽ����ݲ����� ACK�ź�
		
		DATA_LSB=IIC_Read_Byte(0);//������λ�ֽ����ݲ����� NACK�ź�
		
		IIC_Stop();//����һ��ֹͣ����
		
		delay_ms(200);
		if(target) DATA_LSB&=~(0x0f);
		else 			 DATA_LSB&=~(0x03);
		result=((DATA_MSB<<8)+(DATA_LSB));//�ϲ����ֽڣ����ֽڵĺ���λҪ��0
//		printf("result is %d \r\n",result);
		return  result ;
		 
}

/**************************************************************************************************

												������ʪ��������
												target Ϊ 1   ��ȡʪ��ֵ 
															 Ϊ 0   ��ȡ�¶�ֵ
**************************************************************************************************/
u16 Task_Read_Temperature_humidity(unsigned char target)
{
		u16 result=0;
//		u8 time=1;
//		while(time--){
//			delay_ms(30);
//			Reset_HTU21D();
//			delay_ms(30);
//			Write_User_Register();
//		
//			result=Read_Temperature_humidity(target);
//		
//			if(result == 0xFFFC){
//					printf("Senor HTU21D read failed,try again.\r\n");
//				continue;					//δ�Ӵ�������ֱ�ӷ��ش������
//			}
//			printf("Air=%x\r\n",result);
//			if(target)	SaveSensorData(Air_Hum_Data,result);
//			else				SaveSensorData(Air_Tem_Data,result);
//			return result;
//		}
//		if(target)	ReadSensorData(Air_Hum_Data,&result);
//		else				ReadSensorData(Air_Tem_Data,&result);
////		TaskSenorErr_Warning( );
		return result;
}

void HTU21D_Reset(void)
{
#ifdef DEBUG_HTU21D
	printf("Into HTU21D_Reset\r\n");
#endif
	IIC_Start();
	IIC_Send_Byte(0x80);
	IIC_Wait_Ack();
	IIC_Send_Byte(0xfe);
	IIC_Wait_Ack();
	IIC_Stop();
#ifdef DEBUG_HTU21D
	printf("Exit HTU21D_Reset\r\n");
#endif
}
u8 HTU21D_ReadReg(void)
{
	u8 res=0;
#ifdef DEBUG_HTU21D
	printf("Into HTU21D_ReadReg\r\n");
#endif
	IIC_Start();
	IIC_Send_Byte(0x80);
	IIC_Wait_Ack();
	IIC_Send_Byte(0xe7);
	IIC_Wait_Ack();
	IIC_Start();
	IIC_Send_Byte(0x81);
	IIC_Wait_Ack();
	res=IIC_Read_Byte(0); //��Ӧ��
	IIC_Stop();
#ifdef DEBUG_HTU21D
	printf("HTU21D_ReadReg Result is %x \r\n",res);
	printf("Exit HTU21D_ReadReg\r\n");
#endif	
	return res;
}
void HTU21D_WriteReg(u8 cmd)
{
#ifdef DEBUG_HTU21D
	printf("Into HTU21D_WriteReg\r\n");
#endif
	IIC_Start();
	IIC_Send_Byte(0x80);
	IIC_Wait_Ack();
	IIC_Send_Byte(0xe5);
	IIC_Wait_Ack();
	IIC_Send_Byte((cmd & 0xe3));//Ԥ�����ݲ��ɸ�
	IIC_Wait_Ack();
	IIC_Stop();
#ifdef DEBUG_HTU21D
	printf("Exit HTU21D_WriteReg\r\n");
#endif	

}
//���÷Ǳ���������ʽ
u16  HTU21D_GetData(HTU21DdataMode mode)
{
	u8 i=150;
	u8 AckRes=1;
	u8 DATA_LSB=0;
	u8 DATA_MSB=0;
	u16 res=0xff;
#ifdef DEBUG_HTU21D
	printf("Into HTU21D_GetData\r\n");
#endif
	IIC_Start();
	IIC_Send_Byte(0x80);
	IIC_Wait_Ack();
	if(TemperatureMode == mode)
	{
		IIC_Send_Byte(0xf3);
	}
	else if(HumidityMode == mode)
	{
		IIC_Send_Byte(0xf5);
	}
	else
	{
		IIC_Stop();
#ifdef DEBUG_HTU21D
	printf("HTU21D_GetData Input data error\r\n");
#endif	
		return 0xffff;
	}
	IIC_Wait_Ack();
	//�ȴ����ݲɼ����
	do
	{
		delay_ms(10); 
		IIC_Start();
		IIC_Send_Byte(0x81);
		AckRes=IIC_Wait_Ack();
	}
	while(i-- && AckRes); //��ʱ�ȴ�Ӧ��ɹ�
	if(i) //���ݲɼ����
	{
		DATA_MSB=IIC_Read_Byte(1);
		DATA_LSB=IIC_Read_Byte(0); //��ȡ����
		IIC_Stop();
		if(0x02 == (0x02 & DATA_LSB) && (HumidityMode == mode)) //���ص���ʪ������,����ͷ���һ��
		{
			DATA_LSB &= 0xFC;
			res=DATA_MSB*256+DATA_LSB;
#ifdef DEBUG_HTU21D
	printf("HTU21D_GetData Humidity data is%d\r\n",res);
#endif
			return res;
		}
		else if(0==(0x02 & DATA_LSB) && (TemperatureMode == mode)) //���ص����¶����ݣ�����ͷ���һ��
		{
			DATA_LSB &= 0xFC;
			res=DATA_MSB*256+DATA_LSB;
#ifdef DEBUG_HTU21D
	printf("HTU21D_GetData Temperature data is%d\r\n",res);
#endif
			return res;
		}
		else
		{
#ifdef DEBUG_HTU21D
	printf("HTU21D_GetData Get data error\r\n");
#endif
			return 0xffff;
		}

	}
	else
	{
		IIC_Stop();
#ifdef DEBUG_HTU21D
	printf("HTU21D_GetData Timeout\r\n");
#endif	
		return 0xffff;
	}
}

//��������ģʽ
//u16  HTU21D_GetData(HTU21DdataMode mode)
//{
//	u8 DATA_MSB,DATA_LSB;
//	
//	u16 result;
//	if(mode !=TemperatureMode && mode !=HumidityMode) //�����������
//	{
//#ifdef DEBUG_HTU21D
//	printf("HTU21D_GetData Input data is %d, error\r\n",mode);
//#endif
//		return 0xffff;
//	}

//	IIC_Start();                  //��ʼ�ź�

//	IIC_Send_Byte(0x80);	   //���͵�ַ+д���� ���λΪ0��д

//	IIC_Wait_Ack();	 //�ȴ�����ACK�ظ� 

//	if(TemperatureMode == mode) //����¶�
//	{
//		IIC_Send_Byte(0xe3);	   //��������ģʽ�����²����¶�T����
//	}
//	else  //���ʪ��
//	{
//		IIC_Send_Byte(0xe5);	   //��������ģʽ�����²���ʪ��RH����
//	}
//	
//	IIC_Wait_Ack();	 //�ȴ�����ACK�ظ� 
//	
//	IIC_Start();                  //��ʼ�ź�
//	
//	IIC_Send_Byte(0x81);	   //���͵�ַ+������ ���λΪ1�Ƕ�
//	
//	IIC_Wait_Ack();	 //�ȴ�����ACK�ظ� 
//	
//	delay_ms(100);
//	
//	DATA_MSB=IIC_Read_Byte(1);//������λ�ֽ����ݲ����� ACK�ź�
//	
//	DATA_LSB=IIC_Read_Byte(0);//������λ�ֽ����ݲ����� NACK�ź�
//	
//	IIC_Stop();//����һ��ֹͣ����
//	
//	delay_ms(200);
//	if(TemperatureMode == mode) //����¶�
//	{
//		DATA_LSB&=0xfffc;
//	}
//	else  //���ʪ��
//	{
//		DATA_LSB&=0xfff0;
//	}
//	result=DATA_MSB*256+DATA_LSB;//�ϲ����ֽڣ����ֽڵĺ���λҪ��0
////		printf("result is %d \r\n",result);
//	return  result ;
//}





void HTU21D_Test(void)
{
	u16 datares=0;
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	USART1_Init(115200);	 	//���ڳ�ʼ��Ϊ115200
	IIC_Init();
	printf("HTU21D_Test\r\n");
	HTU21D_Reset();
	HTU21D_ReadReg();
	while(1)
	{
		datares=HTU21D_GetData(1);
		TemperatureCal(datares);
		delay_ms(500);
		datares=HTU21D_GetData(2);
		HumidityCal(datares);
		delay_ms(500);
		HTU21D_GetData(3);
//		datares=Read_Temperature_humidity(1); //ʪ��
//		HumidityCal(datares);
//		delay_ms(500);
//		datares=Read_Temperature_humidity(0); //�¶�
//		TemperatureCal(datares);
//		delay_ms(500);
		printf("\r\n\r\n\r\n");
	}
}

void HumidityCal(u16 data)
{
	float res=0;
	data &=0xfff0;
	res =125*(float)data/65536-6;
	printf("HumidityCal is %.2f\r\n",res);
	
}


void TemperatureCal(u16 data)
{
	float res=0;
	data &=0xfffc;
	res =175.72*(float)data/65536-46.85;
	printf("TemperatureCal is %.2f\r\n",res);
}



