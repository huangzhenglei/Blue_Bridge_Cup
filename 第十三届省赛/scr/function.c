#include "function.h"
#include <STC15F2K60S2.H>
#include <intrins.h>
#include "ds1302.h"

u8 code LED[9] = {0xff, 0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f};
u8 code COM[9] = {0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};       //.     u    -
u8 code NUM[13] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0x7f, 0xc1, 0xbf};
u8 buzz_relay_flag = 0x00;

void Delay500us()		//@12.000MHz
{
	unsigned char i, j;

	i = 6;
	j = 211;
	do
	{
		while (--j);
	} while (--i);
}

/*
 *@brief LED控制
 *@param led 控制第几个
 *@param state 控制灯的亮灭
 *       @arg GPIO_PIN_RESET: 亮
 *       @arg GPIO_PIN_SET  : 灭
 * @return 无
 */
void Led_Ctrl(u8 led, u8 state)
{
    static u8 Led_flag = 0xff;

    if(!state)
    {
        Led_flag &= led;
    }
    else
    {
        Led_flag |= (~led);
    }

    P2 = (P2 & 0X1F) | 0X80;
    P0 = Led_flag;
    P2 &= 0X1F;
}

/*
 *@brief 蜂鸣器控制
 *@param buzz 蜂鸣器开关控制参数
 *          @arg GPIO_PIN_RESET: 蜂鸣器关闭
 *          @arg GPIO_PIN_SET  : 蜂鸣器开启
 * @return 无
 */
void Buzz_Ctrl(u8 state)
{
    P0 = buzz_relay_flag;
    P2 = (P2 & 0X1F) | 0XA0;
    P06 = state;
    P2 &= 0X1F; 
    buzz_relay_flag = P0;
}

/*
 *@brief 继电器控制
 *@param relay 继电器开关控制参数
 *              GPIO_PIN_RESET 继电器关闭
 *              GPIO_PIN_SET   继电器开启
 * @return 无
 */
void Relay_Ctrl(u8 state)
{
    P0 = buzz_relay_flag;
    P2 = (P2 & 0X1F) | 0XA0;
    P04 = state;
    P2 &= 0X1F; 
    buzz_relay_flag = P0;
}

/**
 * @brief 数码管显示
 * @param com 位选（控制第几个数码管亮）
 * @param num 段选（控制要显示的数字）
 * @return 无
 */
void Display(u8 com, u8 num)
{
    P2 = (P2 & 0X1F) | 0XC0;
    P0 = COM[com];

    P2 = (P2 & 0X1F) | 0XE0;
    P0 = NUM[num];

    P2 &= 0X1F; 
    Delay500us();
}
 
/**
 *@brief	矩阵按键
 *@note		无
 *@param	无
 *@retval	连续返回有效值
 */
u8 Key_Scan()
{
    u8 line, row;

    P3 &= 0XF0;
    P34 = 1;
    P35 = 1;
    P42 = 1;
    P44 = 1;

    if(P34 == 0)
    {
        line = 4;
    } 
    else if(P35 == 0)
    {
        line = 3;
    }
    else if(P42 == 0)
    {
        line = 2;
    }
    else if(P44 == 0)
    {
        line = 1;
    }
    else
    {
        return No_Press;
    }

    P3 |= 0X0F; 
    P34 = 0;
    P35 = 0;
    P42 = 0;
    P44 = 0;

    if(P30 == 0)
    {
        row = 4;
    } 
    else if(P31 == 0)
    {
        row = 3;
    }
    else if(P32 == 0)
    {
        row = 2;
    }
    else if(P33 == 0)
    {
        row = 1;
    }

    return (line << 4) + row;
} 

/**
 * @brief 按键返回一次有效值
 * @param 无
 * @return u8 返回按键有效值
 */
u8 Key_Val()
{
    static u8 key = No_Press, state = No_Press;

    state = Key_Scan();
    if(key != state)
    {
        key = state;
        return key;
    }
    else
    {
        return No_Press;
    }
}

/**
 * @brief 温度显示
 * 
 * @param temperture 所要显示的温度
 * @return 无
 */
void Display_Temperture(float temperture)
{

    Display(1, 11);
    Display(2, 1);

    Display(6, (u16)temperture / 100);
    Display(7, (u16)temperture / 10 % 10);
    Display(7, 10);
    Display(8, (u16)temperture % 10);
}

/**
 * @brief 时间显示
 * @param 无
 * @return 无
 */
void Display_Time()
{
    Display(1, 11);
    Display(2, 2); 

    Display(4, Hour / 10);
    Display(5, Hour % 10);
    Display(6, 12);
    Display(7, Min / 10);
    Display(8, Min % 10);
}

/**
 * @brief 温度阈值显示
 * 
 * @param threshlod 所要显示的阈值
 * @return 无
 */
void Display_Threshlod(u8 threshlod)
{
    Display(1, 11);
    Display(2, 3);    

    Display(7, threshlod / 10); 
    Display(8, threshlod % 10); 
}

/**
 * @brief 定时器0初始化
 * @param 无
 * @return 无
 */
void Timer0Init(void)		//50毫秒@12.000MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xB0;		//设置定时初值
	TH0 = 0x3C;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
}

