#include<reg52.h>
#include"config.h"
#include"temp.h"

bit Presence;
sbit DataPort =P3^5;		//18B20�����ݿ�
uint8 TempL,TempH;

void uDelay(uint8 DelayTime)		 
{ 	
	while(--DelayTime);		  
}

void Inti()
{
	DataPort=0;	 //����
	uDelay(180);	 //��ʱ������
	DataPort=1;
	uDelay(24);	 //��ʱ���״̬��������ڣ���18B20���˿�����
	Presence=DataPort;//״̬���룬���ں�����ʾ�ж�
	uDelay(230);	//��ʱ���һ����ʱ��
}

void WriteByte(uint8 val)	
{
 uint8 i;
  for (i=8; i>0; i--) 
  {
    DataPort = 0;
	uDelay(4); 
    DataPort = val&0x01;
    uDelay(20); 
    DataPort = 1;
    val=val/2;	   // ����1λ
  }
  uDelay(50);
}

//�ֽڶ�ȡ����
uint8 ReadByte()	 
{
uint8 i;
uint8 value=0;
	for (i=0;i<8;i++)//����8�ֽ�
	{
		DataPort=0;	//���ͺ������ߣ���������϶
		DataPort=1;
		uDelay(4); //��ʱ�����
		if(DataPort) value|=0x01<<i;
		uDelay(20); //��ʱ���һ����ʱ��
	}
return (value);
}

void SetStep(uint8 Dat)	
{
	Inti();
	WriteByte(0xcc);
	WriteByte(0x4e);
	WriteByte(0x15);  //д��ַ2��TH
	WriteByte(0x00);   //д��ַ3��TL
	WriteByte(Dat);  //��ַ4,������������С����ֱ��ʣ�1F��3F��5F��7Fȡֵ��ֵԽ��ֱ���Խ��
	Inti();
	WriteByte(0xcc);
	WriteByte(0xbe);
	ReadByte();
	Inti();
	WriteByte(0xcc);
	WriteByte(0x48); //����ָ��������6ms�ȴ�18B20������д��EEPROM��д�����ٸ�λ��
	uDelay(100);
	Inti();
	DataPort=1;
}

void GetTemp()	   
{ 
    //�¶�ת��
	Inti();				  //��λ               //ÿִ��һ��ָ�����ʾ����������
	WriteByte(0xcc);		 //ROM ָ��
	WriteByte(0x44);	 //RAMָ��
	//��ȡRAM
	Inti();					 //��λ
	WriteByte(0xcc);		 //ROM ָ��
	WriteByte(0xbe);		 //RAMָ��
	TempL=ReadByte();		 //��ȡ�¶ȵ��ֽ�
	TempH=ReadByte();		 //��ȡ�¶ȸ��ֽ�
}

void temp_init()
{
    uDelay(100);
	Presence=1;
	SetStep(0x7f);	//��ڲ���
	GetTemp();	   //�¶Ȼ���ӳ���
	//Display(100); //���͵�һ��ȡ�¶�ָ���Ҫ��ʱһ��ʱ��ȴ��ڲ�ת���������������ʾ85�ȵ����⡣��
}


void temp(void)
{   
	GetTemp();	//�¶Ȼ���ӳ���
	//Display(50);//�������ʾ�ӳ���
}