#include "iic.h"
#include <STC15F2K60S2.H>
#include <intrins.h>

#define DELAY_TIME 5

sbit SDA = P2^1;
sbit SCL = P2^0;

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
    /* 当SCL高电平时，SDA出现一个下降沿表示I2C总线启动信号 */
    SDA = 1;                        //将SDA初始化为高电平                
    SCL = 1;                        //初SCL始化为高电平
    IIC_Delay(DELAY_TIME);
    SDA = 0;            
    IIC_Delay(DELAY_TIME);
    SCL = 0;	                    // 拉低SCL开始传输数据
}

//I2C总线停止信号
void IIC_Stop(void)
{
    /* 当SCL高电平时，SDA出现一个上升沿表示I2C总线停止信号 */
    SDA = 0;                        //将SDA初始化为低电平      
    SCL = 1;                        //初SCL初始化为高电平         
    IIC_Delay(DELAY_TIME);
    SDA = 1;                             
    IIC_Delay(DELAY_TIME);          
}

//发送应答或非应答信号
void IIC_SendAck(bit ackbit)
{
    SCL = 0;                        // 拉低SCL开始传输数据                          
    SDA = ackbit;  		            //要发送的信号，发应答信号或者非应答信号			
    IIC_Delay(DELAY_TIME);
    SCL = 1;                        // 拉高SCL           
    IIC_Delay(DELAY_TIME);
    SCL = 0;                       
    SDA = 1;                       
    IIC_Delay(DELAY_TIME);
}

//等待应答
bit IIC_WaitAck(void)
{
    bit ackbit;
	
    SCL  = 1;                   // 拉高时钟线
    IIC_Delay(DELAY_TIME);
    ackbit = SDA;               // 获取数据线的电平
    SCL = 0;                    // 拉低SCL开始传输数据
    IIC_Delay(DELAY_TIME);
    return ackbit;              //返回信号
}

//I2C总线发送一个字节数据
void IIC_SendByte(unsigned char byt)
{
    unsigned char i;

    for(i=0; i<8; i++)
    {
        SCL  = 0;                       // 拉低SCL开始传输数据  
        IIC_Delay(DELAY_TIME);
        if(byt & 0x80) SDA  = 1;        //先把高位取出来，给SDA幅值
        else SDA  = 0;
        IIC_Delay(DELAY_TIME);
        SCL = 1;                        //准备下一个数据
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
    	SCL = 1;                    //准备接收数据
	IIC_Delay(DELAY_TIME);
	da <<= 1;                       //向左移一位
	if(SDA) da |= 1;                //获取这一位的数据
	SCL = 0;                        //准备下一个传输数据 
	IIC_Delay(DELAY_TIME);
    }
    return da;    
}

/**
 * @brief 往AT24C02写一个数据进去
 * @note 注意这是8位，地址范围为0~255，数据范围为0~255
 * @param addr 存放数据的地址
 * @param date 所要存放的数据大小
 */
void EEPROM_Write(unsigned char addr, unsigned char date)
{
    IIC_Start();                    //起始信号

    IIC_SendByte(0xa0);             //发送器件的地址并使用写数据功能
    IIC_WaitAck();                  //等待应答

    IIC_SendByte(addr);             //发送存储数据的地址
    IIC_WaitAck();                  //等待应答

    IIC_SendByte(date);             //发送要存储的数据          
    IIC_WaitAck();                  //等待应答

    IIC_Stop();                     //停止信号
}

/**
 * @brief 往AT24C02读取一个数据出来
 * 
 * @param addr 读取数据的地址
 * @return unsigned char 返回读取的数据
 */
unsigned char EEPROM_Read(unsigned char addr)
{

    unsigned char temp = 0;

    IIC_Start();                //起始信号

    IIC_SendByte(0xa0);         //发送器件的地址并使用写数据功能
    IIC_WaitAck();              //等待应答

    IIC_SendByte(addr);         //发送要读取数据的地址
    IIC_WaitAck();              //等待应答

    IIC_Start();                //读写转换时，需重新启用IIC总线          

    IIC_SendByte(0xa1);         //发送器件的地址并使用读数据功能
    IIC_WaitAck();              //等待应答

    temp = IIC_RecByte();       //读数数据
    IIC_SendAck(1);             //发送非应答信号，告诉芯片我后面不读数据了

    IIC_Stop();                 //停止信号，停止通信      

    return temp;                //将读取的数据返回
}

/**
 * @brief 往AT24C02连续写数据进去
 * @note 这种连续写法，要注意延时
 * @param addr 写数据的起始地址
 * @param date 存放的数据地址
 * @param number 往AT24C02写多少个数据
 */
void EEPROM_ContinuousWrite(unsigned char addr, unsigned char date[], unsigned char number)
{
    while(number--)                          //控制写多少个数据             
    {
        EEPROM_Write(addr++, *date++);       //多次调用EEPROM_Write函数往里面写数据
        Delay5ms();                          //写进AT24C02需要一定的时间
    }
}

/**
 * @brief 连续从AT24C02读数据
 * 
 * @param addr 读数据的起始地址
 * @param date 读取数据存放的地址
 * @param num 读多少个数据
 * @return 无
 */
void EEPROM_ContinuousRead(unsigned char addr, unsigned char date[], unsigned char number)
{
    
    IIC_Start();                 //起始信号

    IIC_SendByte(0xa0);          //发送器件的地址并使用写数据功能
    IIC_WaitAck();               //等待应答

    IIC_SendByte(addr);          //发送要读取数据的地址
    IIC_WaitAck();               //等待应答

    IIC_Start();                 //起始信号
    IIC_SendByte(0xa1);          //发送器件的地址并使用读数据功能
    IIC_WaitAck();               //等待应答          

    while(number--)              //控制读取多少个数据
    {
        *date++ = IIC_RecByte(); //读数据，并指向下一个地址来接收下一个数据

        if(number != 0)          //如果不是读取最后一个数据发送应答信号，否则发送非应答信号
        {
            IIC_SendAck(0);      //发送应答信号
        }
        else
        {
            IIC_SendAck(1);      //发送非应答信号
        }
    }

    IIC_Stop();                   //停止使用IIC总线  
}

/**
 * @brief PCF8591_DAC输出
 * @note 参考电压5V，8位数据
 * @param date：需要转化的数据，范围为0~255
 * @return 无
 */
void PCF8591_DAC(unsigned char date)
{
    IIC_Start();                     //起始信号

    IIC_SendByte(0x90);              //发送器件的地址并使用写数据功能
    IIC_WaitAck();                   //等待应答        

    IIC_SendByte(0x40);              //启用DAC模式，输出到通道OUT 
    IIC_WaitAck();                   //等待应答  

    IIC_SendByte(date);              //发送要转换的数据 
    IIC_WaitAck();                   //等待应答 

    IIC_Stop();                      //停止使用IIC总线 
} 

/**
 * @brief PCF8591_ADC初始化
 * 
 * @param channel：ADC转化通道
 *          @arg：0x00：通道0，未使用
 *          @arg：0x01：通道1，光敏电阻
 *          @arg：0x02：通道2，外接输入
 *          @arg：0x03：通道3，电位器
 * @return 无
 */
void PCF8591_ADC_Init(unsigned char channel)
{
    IIC_Start();                        //启用IIC总线 

    IIC_SendByte(0x90);                 //发送器件的地址并使用写数据功能
    IIC_WaitAck();                      //等待应答

    IIC_SendByte(channel);              //启用ADC模式，发送转换通道
    IIC_WaitAck();                      //等待应答

    IIC_Stop();                         //停止使用IIC总线 
}

/**
 * @brief 获取PCF8591_ADC的转换结果
 * @note 参考电压5V，8位数据
 * @param 无
 * @return 返回转换结果
 */
unsigned char PCF8591_ADC_Get()
{
    unsigned char date = 0;

    IIC_Start();                    //启用IIC总线 

    IIC_SendByte(0x91);             //发送器件地址并使用读数据功能
    IIC_WaitAck();                  //等待应答

    date = IIC_RecByte();           //接收转换好的数据
    IIC_SendAck(1);                 //发送非应答信号    

    IIC_Stop();                     //停止使用IIC总线

    return date;                    //返回转换结果
}
