#ifndef CFG_H
#define CFG_H

//�豸����
#define KITC "kitche_"  //����
#define LIVI "living_"  //����
#define BALC "balcon_"  //��̨



//����������
#define LIGHT "light,"
#define TEMP "temp,"
#define HUMIDITY "humi,"
#define SOIL  "soil,"
#define RAINDROP  "rain,"
#define COMBUSTIBLE "combus,"
#define CARBON  "carbon,"
#define FORMALDEHYDE "formal,"
#define INFRARED "infra,"
#define BEEP "beep,"
#define LED "led,"
#define RELAY "relay,"


typedef enum
{
	LightSensor = 0x01,  //���մ�����
	AirTemperatureSensor = 0x02, //�����¶ȴ�����
	AirHumiditySensor = 0x03, //����ʪ�ȴ�����
	SoilSensor = 0x04, //����ʪ�� ������
	RaindropSensor=0x05, //��δ�����
	CombustibleGasSensor=0x06,//��ȼ���崫����
	CarbonMonoxideSensor=0x07, //һ����̼������
	FormaldehydeSensor=0x08, //��ȩ������
	InfraRedSensor=0x09, //���⴫����
	BeepControler=0x0a, //������
	LEDControler=0x0b, //led
	RelayControler=0x0c //�̵���
	
}TypedefEquipmentName;

typedef enum
{
	GatawayAdd = 0x00,
	BalconyAdd = 0x01,
	LivingRoomAdd = 0x02,
	KitchenAdd =0x03	
	
}TypedefEquipmentAdd;

typedef enum
{
	ATHeader=0xfe,
	DataHeader=0xaa	
	
}TypedefDataHead;

typedef enum
{
	ATReadMode=0xfe,
	ATWriteMode=0xaa,
	DataMode=0x1f,
	ControlMode=0x2c
	
	
}TypedefDataCmd;








#endif

