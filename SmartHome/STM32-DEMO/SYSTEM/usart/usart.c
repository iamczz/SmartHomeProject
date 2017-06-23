#include "sys.h"
#include "usart.h"
#include <string.h>
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART1_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
void USART_Delay(void)
{
	u16 i=0;
	for(i=0;i<1000;i++);
}

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*ʹ��microLib�ķ���*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART1_RX_BUF[USART1_REC_LEN];     //���ջ���,���USART1_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  
u16 RX1_Point=0; 
void USART1_Init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 

}

// void USART1_IRQHandler(void)                	//����1�жϷ������
// 	{
// 	u8 Res;
// #if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
// 	OSIntEnter();    
// #endif
// 	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
// 		{
// 		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
		
// 		if((USART_RX_STA&0x8000)==0)//����δ���
// 			{
// 			if(USART_RX_STA&0x4000)//���յ���0x0d
// 				{
// 				if(Res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
// 				else USART_RX_STA|=0x8000;	//��������� 
// 				}
// 			else //��û�յ�0X0D
// 				{	
// 				if(Res==0x0d)USART_RX_STA|=0x4000;
// 				else
// 					{
// 					USART1_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
// 					USART_RX_STA++;
// 					if(USART_RX_STA>(USART1_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
// 					}		 
// 				}
// 			}   		 
//      } 
// #if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
// 	OSIntExit();  											 
// #endif
// } 
#endif	


void USART1_IRQHandler( void )
{
		unsigned char temp = 0;
		if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
		{
			temp = USART_ReceiveData( USART1 );     /* ��ȡUSART1���ݣ��Զ������־λ RXNE */              
			if( RX1_Point <= USART1_REC_LEN)
			{           
				USART1_RX_BUF[RX1_Point] = temp;              
				RX1_Point++;         
			}      
		}  
}
void USART1_SendData(const unsigned char* string,u16 len)
{	
	u16 i=0;	
	USART_GetFlagStatus(USART1,USART_FLAG_TC);	//���������ɱ�־λ
//	while(i < len && i<strlen(string))//����ַ�����β
	while(i < len)//����ַ�����β
	{				
		USART_SendData(USART1,string[i]);//����һ���ַ�				
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);		//�ȴ�������ɱ�־λ
		i++;		
	}
}

void USART1_ReceiveData(u8 *buf,u16 *len)
{
	u16 rxlen=RX1_Point;
	u16 i=0;
	*len=0;				//Ĭ��Ϊ0
	USART_Delay();
	if(rxlen==RX1_Point&&rxlen)//���յ�������,�ҽ��������
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=USART1_RX_BUF[i];	
		}		
		*len=RX1_Point;	//��¼�������ݳ���
		RX1_Point=0;		//����
	}
	// RX1_Point=0;
}


u8  USART2_RX_BUF[USART2_REC_LEN]; 
u16 RX2_Point=0;
void USART2_Init( u32 bound )
{		
		GPIO_InitTypeDef  GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef  NVIC_InitStructure;
		
		USART_DeInit(USART2);		
		//ʹ�ܴ���3��ʱ�� �Ͷ�ӦGPIOʱ��
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);				
		//����TX����GPIO
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
		GPIO_Init(GPIOA,&GPIO_InitStructure);		
		//����RX����GPIO
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA,&GPIO_InitStructure);		
		//���ô���2
		USART_InitStructure.USART_BaudRate = bound;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(USART2, &USART_InitStructure);		
		//ʹ�ܴ���2
		USART_Cmd(USART2,ENABLE);
		//ʹ�ܴ��ڽ����ж�
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
		//���ô���3�����ж�
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);		
		USART_ClearFlag(USART2, USART_FLAG_RXNE);		
		USART_GetFlagStatus(USART2,USART_FLAG_TC);     /* �ȶ�SR,��дDR */		
}


void USART2_IRQHandler( void )
{
		unsigned char temp = 0;
		if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
		{
			temp = USART_ReceiveData( USART2 );     /* ��ȡUSART2���ݣ��Զ������־λ RXNE */              
			if( RX2_Point <= USART2_REC_LEN)
			{           
				USART2_RX_BUF[RX2_Point] = temp;              
				RX2_Point++;         
			}      
		}  
}
void USART2_SendData(const unsigned char* string,u16 len)
{	
	u16 i=0;	
	USART_GetFlagStatus(USART2,USART_FLAG_TC);	//���������ɱ�־λ
	while(i < len)//����ַ�����β
	{				
		USART_SendData(USART2,string[i]);//����һ���ַ�				
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);		//�ȴ�������ɱ�־λ
		i++;		
	}
}

void USART2_ReceiveData(u8 *buf,u16 *len)
{
	u16 rxlen=RX2_Point;
	u16 i=0;
	*len=0;				//Ĭ��Ϊ0
	USART_Delay();
	if(rxlen==RX2_Point&&rxlen)//���յ�������,�ҽ��������
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=USART2_RX_BUF[i];	
		}		
		*len=RX2_Point;	//��¼�������ݳ���
		RX2_Point=0;		//����
	}
	// RX2_Point=0;
}



u8  USART3_RX_BUF[USART3_REC_LEN]; 
u16 RX3_Point=0;
void USART3_Init( u32 bound )
{		
		GPIO_InitTypeDef  GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef  NVIC_InitStructure;
		
		USART_DeInit(USART3);		
		//ʹ�ܴ���3��ʱ�� �Ͷ�ӦGPIOʱ��
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);				
		//����TX����GPIO
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
		GPIO_Init(GPIOB,&GPIO_InitStructure);		
		//����RX����GPIO
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOB,&GPIO_InitStructure);		
		//���ô���3
		USART_InitStructure.USART_BaudRate = bound;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(USART3, &USART_InitStructure);		
		//ʹ�ܴ���3
		USART_Cmd(USART3,ENABLE);
		//ʹ�ܴ��ڽ����ж�
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
		//���ô���3�����ж�
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);		
		USART_ClearFlag(USART3, USART_FLAG_RXNE);		
		USART_GetFlagStatus(USART3,USART_FLAG_TC);     /* �ȶ�SR,��дDR */		
}


void USART3_IRQHandler( void )
{
		unsigned char temp = 0;
		if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
		{
			temp = USART_ReceiveData( USART3 );     /* ��ȡUSART3���ݣ��Զ������־λ RXNE */              
			if( RX3_Point <= USART3_REC_LEN)
			{           
				USART3_RX_BUF[RX3_Point] = temp;              
				RX3_Point++;         
			}      
		}  
}
void USART3_SendData(const unsigned char* string,u16 len)
{	
	u16 i=0;	
	USART_GetFlagStatus(USART3,USART_FLAG_TC);	//���������ɱ�־λ
	while(i < len)//����ַ�����β
	{				
		USART_SendData(USART3,string[i]);//����һ���ַ�				
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);		//�ȴ�������ɱ�־λ
		i++;		
	}
}

void USART3_ReceiveData(u8 *buf,u16 *len)
{
	u16 rxlen=RX3_Point;
	u16 i=0;
	*len=0;				//Ĭ��Ϊ0
	USART_Delay();
	if(rxlen==RX3_Point&&rxlen)//���յ�������,�ҽ��������
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=USART3_RX_BUF[i];	
		}		
		*len=RX3_Point;	//��¼�������ݳ���
		RX3_Point=0;		//����
	}
	// RX3_Point=0;
}






