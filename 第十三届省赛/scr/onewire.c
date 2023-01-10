#include "onewire.h"
#include <STC15F2K60S2.H>

sbit DQ = P1^4;  

//单总线内部延时函数
void Delay_OneWire(unsigned int t)  
{
	unsigned char i;
	while(t--)
	{
		for(i = 0; i < 12; i++)
			;
	}
}

//单总线写操作
void Write_DS18B20(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		DQ = 0;
		DQ = dat&0x01;
		Delay_OneWire(5);
		DQ = 1;
		dat >>= 1;
	}
	Delay_OneWire(5);
}

//单总线读操作
unsigned char Read_DS18B20(void)
{
	unsigned char i;
	unsigned char dat;
  
	for(i=0;i<8;i++)
	{
		DQ = 0;
		dat >>= 1;
		DQ = 1;
		if(DQ)
		{
			dat |= 0x80;
		}	    
		Delay_OneWire(5);
	}
	return dat;
}

//DS18B20初始化
bit init_ds18b20(void)
{
  	bit initflag = 0;
  	
  	DQ = 1;
  	Delay_OneWire(12);
  	DQ = 0;
  	Delay_OneWire(80);
  	DQ = 1;
  	Delay_OneWire(10); 
    initflag = DQ;     
  	Delay_OneWire(5);
  
  	return initflag;
}

/**
 * @brief 获取温度
 * @param 无
 * @return unsigned char 返回整数温度值
 */
unsigned char rd_temperature(void)
{
	unsigned char low, high;
	unsigned int temperature;

	init_ds18b20();						//初始化DS18B20
	Write_DS18B20(0xcc);				//跳过ROM指令
	Write_DS18B20(0x44);				//开启温度转换 	
	Delay_OneWire(200);					//等待温度转换

	init_ds18b20();						//初始化DS18B20
	Write_DS18B20(0xcc);				//跳过ROM指令
	Write_DS18B20(0xbe);				//读取DS18B20温度值

	low = Read_DS18B20();				//先获取低字节
	high = Read_DS18B20();				//先获取高字节

	temperature = (low >> 4) + (high << 4);

	return temperature;
}

/**
 * @brief 获取温度
 * @param 无
 * @return float 返回浮点类型温度值
 */
float rd_temperature_f(void)
{

	unsigned char low, high;
	unsigned int temp;
	float temperature;

	init_ds18b20();						//初始化DS18B20
	Write_DS18B20(0xcc);				//跳过ROM指令
	Write_DS18B20(0x44);				//开启温度转换 	
	Delay_OneWire(200);					//等待温度转换

	init_ds18b20();						//初始化DS18B20
	Write_DS18B20(0xcc);				//跳过ROM指令
	Write_DS18B20(0xbe);				//读取DS18B20温度值

	low = Read_DS18B20();				//先获取低字节
	high = Read_DS18B20();				//先获取高字节

	temp = high;						
	temp <<= 8;
	temp |=  low;						

	temperature = temp * 0.0625;		

	return temperature;	

}
