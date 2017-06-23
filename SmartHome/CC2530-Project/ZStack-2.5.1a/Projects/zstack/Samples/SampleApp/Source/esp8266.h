#ifndef   ESP8266_H_H
#define   ESP8266_H_H
#include "hal_types.h"

//#define   AT          "AT\r\n"	
//#define   CWMODE      "AT+CWMODE=3\r\n"
//#define   RST         "AT+RST\r\n"
#define   CIFSR       "AT+CIFSR\r\n"
#define   CWJAP       "AT+CWJAP=\"ygnwifi\",\"ygn123456ygn\"\r\n" //WiFi���ֺ�����
#define   CIPSTART    "AT+CIPSTART=\"TCP\",\"183.230.40.39\",876\r\n"  //���������ӷ�ʽ����ַ�Ͷ˿�
#define   CIPMODE     "AT+CIPMODE=1\r\n"
#define   CIPSEND     "AT+CIPSEND\r\n"
#define   CIPSTATUS   "AT+CIPSTATUS\r\n"

#define   AT          "AT\r\n"	
#define   CWMODE      "AT+CWMODE=3\r\n"
#define   RST         "AT+RST\r\n"
#define   CWSAP       "AT+CWSAP=\"ljx\",\"12345678\",1,3\n"
#define   CIPMUX      "AT+CIPMUX=1\n"
#define   CIPSTO      "AT+CIPSTO=2880\n"
#define   SERVER      "AT+CIPSERVER=1,8080\n"


#define   MAX_SEND_BUF_LEN  1024
#define   MAX_RECEIVE_BUF_LEN  1024 //�������ݻ��������ȣ����ɸ��ģ���������׳��ֻ�������С�ڴ�й¶���



extern  char SendBuff[MAX_SEND_BUF_LEN];//�������ݻ�����
extern  char ReceiveBuff[MAX_RECEIVE_BUF_LEN];//�������ݻ�����

/*****
˵����
WiFiģ���cc2530ʹ�ô���͸���ķ�ʽͨ�ţ�WiFiģ����Ҫ�����Ĺ����ǳ�ʼ������λ���������ݡ��������ݡ�����ATָ����WiFiģ���Ƿ����ӳɹ������Եȹ��ܺ���
����Ӳ����λ�������������ݺͽ�������Ϊ�ײ㺯������Ҫ�õ�cc2530��Ӳ����Դ�����������ڷ��ͺͽ��պ����Ļ����Ͻ��п�����Ӧ�ò㺯�������Բ��Ժ�����Ҫ�ǲ���
�����ײ㺯���Ƿ������������󣬲��ܶ�Ӧ�ò����غ������в��ԡ�
ygn
*****/


extern  void ESP8266_Rst(void);//Ӳ����λ����
extern  void ESP8266_Init1(void);//��ʼ������
extern  uint8 SendCmd(char* cmd, char* result, int timeOut);//ATָ�����Ӧ��ͳ�ʱ�ȴ�
extern  char ESP8266_CheckStatus(uint16 timeOut);//���WiFiģ���Ƿ�����
extern  void ESP8266_SendData(const unsigned char *send_buf,uint16 buflen);//оƬ�������ݸ�WiFiģ��
extern  void ESP8266_ReceiveData(unsigned char *receive_buf,uint16 *buflen);//оƬ��ȡWiFiģ�鷢�͹���������
extern  void ESP8266_IntoAtCmd(void);//����ATָ��ģʽ
extern  void ESP8266_IntoCIPSEND(void);//����͸��ģʽ
static  void ESP8266_DelayMs(uint16 time);//��ʱ����
extern  void ESP8266_SendDataToService(const unsigned char *send_buf,uint16 buflen);//оƬ�������ݸ�WiFiģ�飬͸��ģʽ
extern  void ESP8266_Test(void);//����
#endif


