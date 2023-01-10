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
		SCK = 0;
		SDA = temp&0x01;
		temp>>=1; 
		SCK=1;
	}
}   

//向DS1302寄存器写入数据
void Write_Ds1302_Byte( unsigned char address,unsigned char dat )     
{
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1; 	_nop_();  
 	Write_Ds1302(address);	
 	Write_Ds1302(dat);		
 	RST=0; 
}

//从DS1302寄存器读出数据
unsigned char Read_Ds1302_Byte ( unsigned char address )
{
 	unsigned char i,temp=0x00;
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
 	RST=1;	_nop_();
 	Write_Ds1302(address);
 	for (i=0;i<8;i++) 	
 	{		
		SCK=0;
		temp>>=1;	
 		if(SDA)
 		temp|=0x80;	
 		SCK=1;
	} 
 	RST=0;	_nop_();
 	SCK=0;	_nop_();
	SCK=1;	_nop_();
	SDA=0;	_nop_();
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
void RTC_Write(unsigned char hour, unsigned char minute, unsigned char second)
{
	Write_Ds1302_Byte(0x8e, 0x00);				//清楚写保护位
	Write_Ds1302_Byte(0x80, ((second / 10) << 4) + (second % 10));	//往秒寄存器写second
	Write_Ds1302_Byte(0x82, ((minute / 10) << 4) + (minute % 10));	//往分钟寄存器写minute
	Write_Ds1302_Byte(0x84, ((hour / 10) << 4) + (hour % 10));		//往小时寄存器写hour
	Write_Ds1302_Byte(0x8e, 0x80);				//开启写保护位
}

/**
 * @brief 获取RTC内部时间数据
 * @note 数据范围，24小时制，并使用全局变量存储读数的数据（Hour，Min，Sec）
 * @param 无
 * @return 无
 */
void RTC_Read()
{
	unsigned char hour, minute, second;
	
	second = Read_Ds1302_Byte(0x81);		//读取秒寄存器的值
	minute = Read_Ds1302_Byte(0x83);		//读取分钟寄存器的值
	hour = Read_Ds1302_Byte(0x85);			//读取小时寄存器的值

	Hour = ((hour >> 4) * 10) + (hour & 0x0f);		//BCD转10进制
	Min = ((minute >> 4) * 10) + (minute & 0x0f);
	Sec = ((second >> 4) * 10) + (second & 0x0f);
}

