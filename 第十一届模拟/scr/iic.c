#include "iic.h"
#include <STC15F2K60S2.H>
#include <intrins.h>

sbit SDA = P2^1;
sbit SCL = P2^0;


#define DELAY_TIME 5

void Delay5ms()		//@11.0592MHz
{
	unsigned char i, j;

	i = 54;
	j = 199;
	do
	{
		while (--j);
	} while (--i);
}

//I2C总线内部延时函数
void IIC_Delay(unsigned char i)
{
    do{_nop_();}
    while(i--);        
}

//I2C总线启动信号
void IIC_Start(void)
{
    SDA = 1;
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SDA = 0;
    IIC_Delay(DELAY_TIME);
    SCL = 0;	
}

//I2C总线停止信号
void IIC_Stop(void)
{
    SDA = 0;
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SDA = 1;
    IIC_Delay(DELAY_TIME);
}

//发送应答或非应答信号
void IIC_SendAck(bit ackbit)
{
    SCL = 0;
    SDA = ackbit;  					
    IIC_Delay(DELAY_TIME);
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SCL = 0; 
    SDA = 1;
    IIC_Delay(DELAY_TIME);
}

//等待应答
bit IIC_WaitAck(void)
{
    bit ackbit;
	
    SCL  = 1;
    IIC_Delay(DELAY_TIME);
    ackbit = SDA;
    SCL = 0;
    IIC_Delay(DELAY_TIME);
    return ackbit;
}

//I2C总线发送一个字节数据
void IIC_SendByte(unsigned char byt)
{
    unsigned char i;

    for(i=0; i<8; i++)
    {
        SCL  = 0;
        IIC_Delay(DELAY_TIME);
        if(byt & 0x80) SDA  = 1;
        else SDA  = 0;
        IIC_Delay(DELAY_TIME);
        SCL = 1;
        byt <<= 1;
        IIC_Delay(DELAY_TIME);
    }
    SCL  = 0;  
}

//I2C总线接收一个字节数据
unsigned char IIC_RecByte(void)
{
    unsigned char i, da;
    for(i=0; i<8; i++)
    {   
    	SCL = 1;
	IIC_Delay(DELAY_TIME);
	da <<= 1;
	if(SDA) da |= 1;
	SCL = 0;
	IIC_Delay(DELAY_TIME);
    }
    return da;    
}

/**
 * @brief 往AT24C02写一个数据
 * 
 * @param addr 存放数据的地址，地址范围为0~255
 * @param date 存放的数据大小，数据范围为0~255
 * @return 无
 */
void EEPROM_Write(unsigned char addr, unsigned char date)
{
    IIC_Start();                    //启用IIC总线

    IIC_SendByte(0xa0);             //发送器件地址并使用写数据功能
    IIC_WaitAck();                  //等待器件应答

    IIC_SendByte(addr);             //发送数据的地址
    IIC_WaitAck();                  //等待器件应答

    IIC_SendByte(date);             //发送数据
    IIC_WaitAck();                  //等待器件应答

    IIC_Stop();                     //停止使用IIC总线
    Delay5ms();
}

/**
 * @brief 从AT24C02中读一个数据
 * 
 * @param addr 所要读的数据的地址
 * @return 返回读出的数据
 */
unsigned char EEPROM_Read(unsigned char addr)
{
    unsigned char date = 0;

    IIC_Start();                        //启用IIC总线

    IIC_SendByte(0xa0);                 //发送器件地址并使用写数据功能
    IIC_WaitAck();                      //等待器件应答

    IIC_SendByte(addr);                 //发送数据的地址   
    IIC_WaitAck();                      //等待器件应答

    IIC_Start();                        //读写转换时，需重新启用IIC总线              
    IIC_SendByte(0xa1);                 //发送器件地址并使用读数据功能 
    IIC_WaitAck();                      //等待器件应答

    date = IIC_RecByte();               //接收一个字节数据
    IIC_SendAck(1);                     //发送非应答信号

    IIC_Stop();                         //停止使用IIC总线         

    return date;                        //返回读取到的数据
}

