#include "esp8266.h"
#include <string.h>
//�������ϵ�ͷ�ļ�����Ҫ�õ�cc2530�Ĵ��ڡ���ʱͷ�ļ�
#include "hal_uart.h"
#include "Onboard.h"

//Ӳ����λ����
//esp8266�͵�ƽ��λ���˺����Ѹ�λ��������100ms������������ʱ1s����
void ESP8266_Rst(void)
{
  //����cc2530���Ƹ�λ������
  //��������Ϊ�͵�ƽ
  //��ʱ100ms����
  //��������Ϊ�ߵ�ƽ
  //��ʱ1s����
  
}


//��ʼ������
/*��ʼ��������Ҫ��WiFiģ��������ã�һ���޸�·�������Ӻͷ��������ӵĲ������ɣ�����
��������Ĭ�ϼ���*/
void ESP8266_Init1(void)
{
//  ESP8266_Rst();//Ӳ����λ
  
  ESP8266_DelayMs(1000);
  
  SendCmd(AT,"OK",10);//����ATָ������������		
  
  ESP8266_DelayMs(100);
  
  SendCmd(CWMODE,"OK",10);	
  
  ESP8266_DelayMs(100);
  
  SendCmd(RST,"OK",20);	
  
  ESP8266_DelayMs(1000);

  SendCmd(CWSAP,"OK",200);
  
  ESP8266_DelayMs(1000);
  
//  SendCmd(CIPSTART,"OK",100);
  SendCmd(CIPMUX,"OK",100);
  
  ESP8266_DelayMs(1000);
  
//  SendCmd(CIPMODE,"OK",10);
  SendCmd(CIPSTO,"OK",100);
  
   ESP8266_DelayMs(1000);
  
   SendCmd(SERVER,"OK",100);

}
//����ATָ���
/**
���ܣ�����һ��ATָ�����1Ӧ��ɹ���0Ӧ��ʧ��
���������
cmd:�����͵�ָ��
result���ڴ����صĽ��
timeout����ʱ�ȴ�ʱ�䣬����Ϊ100ms
���:1��Ӧ��ɹ���0��Ӧ��ʧ��
*/
uint8 SendCmd(char* cmd, char* result, int timeOut)//ATָ�����Ӧ��ͳ�ʱ�ȴ�
{
  unsigned char buf[1024]={'0'};
  uint16 len=0;
  ESP8266_ReceiveData(buf,&len);//��ȡ���ݣ���֮ǰ����������
  memset(buf,0,sizeof(buf));//�������
  len=0;
  if(NULL != cmd) //������ֻ�ȴ�
  {
    ESP8266_SendData((const unsigned char *)buf,strlen((const char *)cmd)); //��������
  }
  else 
  {
    ;
  }
  
  if(NULL == result)//���õȴ�
  {
    return 1;
  }
  else
  {
    ;
  }
  do
  {
    ESP8266_DelayMs(100);
    ESP8266_ReceiveData(buf,&len);
    if(len >= strlen(result)) //�յ�����
    {
      if(NULL != strstr((const char *)buf,(const char *)result))//��Ԥ�ڽ��һ��
      {
        return 1;
      }
      else
      {
        ;
      }
    }
  }
  while(timeOut--);
  return 0;

}
//���WiFiģ���Ƿ�����
//����1��ʾ���ӳɹ���0��ʾû����
char ESP8266_CheckStatus(uint16 timeOut)//���WiFiģ���Ƿ�����
{
  char res=0;
  ESP8266_IntoAtCmd();
  res=SendCmd(CIPSTATUS, "STATUS:3", timeOut);
  return res;
}


void ESP8266_SendData(const unsigned char *send_buf,uint16 buflen)//оƬ�������ݸ�WiFiģ��
{
  HalUARTWrite(0, (uint8 *)send_buf, buflen);
}
void ESP8266_ReceiveData(unsigned char *receive_buf,uint16 *buflen)//оƬ��ȡWiFiģ�鷢�͹���������
{
  *buflen= HalUARTRead(0, (uint8 *)receive_buf,sizeof(receive_buf));
}
void ESP8266_IntoAtCmd(void)//����ATָ��ģʽ
{
  uint8 flag=0;
  do
  {
    flag=SendCmd("+++", "+++",5); //����ATģʽ
  }
  while(!flag);
}
void ESP8266_IntoCIPSEND(void)//����͸��ģʽ
{
  ESP8266_IntoAtCmd(); //����ATģʽ
  ESP8266_DelayMs(100);//��ʱ����
  SendCmd("AT+CIPMODE=1\r\n", "OK",10); //"����͸��ģʽ"
  SendCmd("AT+CIPSEND\r\n", ">",10);//����͸��ģʽ�����Կ�ʼ��������
}


static void ESP8266_DelayMs(uint16 time)//��ʱ����
{
  while(time)
  {
    MicroWait (1000);     // Wait 1ms
    time--;
  }
  
}


void ESP8266_SendDataToService(const unsigned char *send_buf,uint16 buflen)//оƬ�������ݸ�WiFiģ�飬͸��ģʽ
{
  ESP8266_IntoCIPSEND();//����͸��ģʽ
  ESP8266_DelayMs(100);//��ʱ����
  ESP8266_SendData(send_buf,buflen);
  ESP8266_DelayMs(300);//��ʱ����
  
}

void ESP8266_Test(void)//����
{
  while(1)
  {
    ESP8266_Init();
    ESP8266_DelayMs(1000);//��ʱ����
  }
}