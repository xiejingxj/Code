#include<reg52.h>
#include"config.h"
#include"temp.h"

bit Presence;
sbit DataPort =P3^5;		//18B20的数据口
uint8 TempL,TempH;

void uDelay(uint8 DelayTime)		 
{ 	
	while(--DelayTime);		  
}

void Inti()
{
	DataPort=0;	 //拉低
	uDelay(180);	 //延时后拉高
	DataPort=1;
	uDelay(24);	 //延时后读状态，如果存在，由18B20将端口拉低
	Presence=DataPort;//状态存入，用于后面显示判断
	uDelay(230);	//延时完成一周期时间
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
    val=val/2;	   // 左移1位
  }
  uDelay(50);
}

//字节读取函数
uint8 ReadByte()	 
{
uint8 i;
uint8 value=0;
	for (i=0;i<8;i++)//读入8字节
	{
		DataPort=0;	//拉低后再拉高，产生读间隙
		DataPort=1;
		uDelay(4); //延时后读入
		if(DataPort) value|=0x01<<i;
		uDelay(20); //延时完成一周期时间
	}
return (value);
}

void SetStep(uint8 Dat)	
{
	Inti();
	WriteByte(0xcc);
	WriteByte(0x4e);
	WriteByte(0x15);  //写地址2—TH
	WriteByte(0x00);   //写地址3—TL
	WriteByte(Dat);  //地址4,保留区，用于小数后分辨率，1F，3F，5F，7F取值，值越大分辨率越高
	Inti();
	WriteByte(0xcc);
	WriteByte(0xbe);
	ReadByte();
	Inti();
	WriteByte(0xcc);
	WriteByte(0x48); //拷贝指令，发出后等6ms等待18B20将数据写入EEPROM，写结束再复位。
	uDelay(100);
	Inti();
	DataPort=1;
}

void GetTemp()	   
{ 
    //温度转换
	Inti();				  //复位               //每执行一条指令补充显示，消除闪动
	WriteByte(0xcc);		 //ROM 指令
	WriteByte(0x44);	 //RAM指令
	//读取RAM
	Inti();					 //复位
	WriteByte(0xcc);		 //ROM 指令
	WriteByte(0xbe);		 //RAM指令
	TempL=ReadByte();		 //读取温度低字节
	TempH=ReadByte();		 //读取温度高字节
}

void temp_init()
{
    uDelay(100);
	Presence=1;
	SetStep(0x7f);	//入口参数
	GetTemp();	   //温度获得子程序
	//Display(100); //发送第一个取温度指令后，要延时一段时间等待内部转换。（解决开机显示85度的问题。）
}


void temp(void)
{   
	GetTemp();	//温度获得子程序
	//Display(50);//数码管显示子程序
}