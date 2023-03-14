#include "ds1302.h"  									
#include <STC15F2K60S2.H>
#include <intrins.h>

sbit SCK = P1^7;		
sbit SDA = P2^3;		
sbit RST = P1^3; 

unsigned char Hour = 0;
unsigned char Min = 0;
unsigned char Sec = 0;

//写字节
void Write_Ds1302(unsigned  char temp) 
{
	unsigned char i;
	for (i=0;i<8;i++)     	
	{ 
		SCK = 0;					//拉低SCLK，为脉冲上升沿写入数据做好准备
		SDA = temp&0x01;			//取出dat的第0位数据写入ds1302
		temp>>=1; 					//将dat的各数据位右移1位，准备写入下一个数据位	
		SCK=1;						//上升沿写入数据
	}
}   

//向DS1302寄存器写入数据
void Write_Ds1302_Byte( unsigned char address,unsigned char dat )     
{
 	RST=0;	_nop_();					//禁止数据传递
 	SCK=0;	_nop_();					//确保写数据前SCLK被拉低
 	RST=1; 	_nop_();  					//启动数据传输
 	Write_Ds1302(address);				//写入命令字
 	Write_Ds1302(dat);					//写数据
 	RST=0; 								//禁止数据传递
}

//从DS1302寄存器读出数据
unsigned char Read_Ds1302_Byte ( unsigned char address )
{
 	unsigned char i,temp=0x00;
 	RST=0;	_nop_();					//拉低RST
 	SCK=0;	_nop_();					//确保写数居前SCLK被拉低
 	RST=1;	_nop_();					//启动数据传输
 	Write_Ds1302(address);				//写入命令字
 	for (i=0;i<8;i++) 					//读出数据
 	{		
		SCK=0;							//拉低SCLK，为脉冲上升沿写入数据做好准备			
		temp>>=1;						//将dat的各数据位右移1位，因为先读出的是字节的最低位

 		if(SDA)							//将取出一位数据，写在dat的最高位
 			temp|=0x80;	

 		SCK=1;							//将SCLK置于高电平，为下降沿读出
	} 
 	RST=0;	_nop_();					//禁止数据传递					
 	SCK=0;	_nop_();					//将时钟电平置于已知状态
	SCK=1;	_nop_();
	SDA=0;	_nop_();					//将信号电平置于已知状态
	SDA=1;	_nop_();
	return (temp);			
}

/**
 * @brief RTC芯片设置时间
 * @note 数据范围，使用24小时制
 * @param hour：小时数
 * @param minute：分钟数
 * @param second：秒数
 * @return 无
 */
void DS1302_Write(unsigned char second, unsigned char minute, unsigned char hour)
{						      
	Write_Ds1302_Byte(0x8e, 0x00);			//清楚写保护位

	Write_Ds1302_Byte(0x80, ((second / 10) << 4) + (second % 10));			//写秒
	Write_Ds1302_Byte(0x82, ((minute / 10) << 4) + (minute % 10));			//写分钟
	Write_Ds1302_Byte(0x84, ((hour / 10) << 4) + (hour % 10));				//写小时

	Write_Ds1302_Byte(0x8e, 0x80);			//开启写保护写
}

/**
 * @brief 获取RTC内部时间数据
 * @note 数据范围，24小时制，并使用全局变量存储读数的数据（Hour，Min，Sec）
 * @param 无
 * @return 无
 */
void DS1302_Read()
{
	unsigned char second, minute, hour;

	second = Read_Ds1302_Byte(0x81);					//读出秒的数据
	minute = Read_Ds1302_Byte(0x83);					//读出分的数据
	hour = Read_Ds1302_Byte(0x85);						//读出时的数据

	Hour = ((hour >> 4) * 10) + (hour & 0x0f);			//将BCD码形式的秒、分、时，转化为10进制
	Min = ((minute >> 4) * 10) + (minute & 0x0f);
	Sec = ((second >> 4) * 10) + (second & 0x0f);
}

