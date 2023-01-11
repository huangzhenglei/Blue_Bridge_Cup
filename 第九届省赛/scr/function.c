#include "function.h"
#include <STC15F2K60S2.H>
#include <intrins.h>

u8 code LED[23] = {0xff, 0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f, 
                  0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe,
                  0x7e, 0xbd, 0xdb, 0xe7, 
                  0xdb, 0xbd, 0x7e};

u8 code COM[9] = {0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};       //-
u8 code NUM[11] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0xbf};

void Delay1ms()		//@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	_nop_();
	_nop_();
	i = 11;
	j = 190;
	do
	{
		while (--j);
	} while (--i);
}

/**
 * @brief 将继电器，蜂鸣器全部关闭
 * 
 * @param state 写0，全部关闭
 */
void Relay_Buzz_Ctrl(u8 state)
{
    P2 = (P2 & 0X1F) | 0XA0;
    P0 = state;
    P2 &= 0X1F;
}

/**
 * @brief LED控制函数
 * 
 * @param led控制灯亮的状态 
 */
void Led_Ctrl(u8 led)
{
    P2 = (P2 & 0X1F) | 0X80;
    P0 = LED[led];
    P2 &= 0X1F;
}

/**
 * @brief 数码管显示
 * 
 * @param com 位选（控制第几个数码管亮）
 * @param num 段选（控制要显示的数字） 
 */
void Display(u8 com, u8 num)
{    
    P2 = (P2 & 0X1F) | 0XC0;
    P0 = COM[com];

    P2 = (P2 & 0X1F) | 0XE0;
    P0 = NUM[num];

    P2 &= 0X1F;
    Delay1ms();
}

/**
 * @brief 按键扫描
 * @param 无
 * @return u8 返回按键的状态，如果没有按键按下，返回0
 */
u8 Key_Scan()
{
    if(P30 == 0)
    {
        return s7;
    }
    else if(P31 == 0)
    {
        return s6;
    }
    else if(P32 == 0)
    {
        return s5;
    }
    else if(P33 == 0)
    {
        return s4;
    }
    else 
    {
        return No_Press;
    }
}

/**
 * @brief 按键返回一次有效值
 * @param 无
 * @return KEY_State 返回按键有效值
 */
u8 Key_Val()
{
    static u8 key = No_Press, state = No_Press;

    state = Key_Scan();
    if(state != key)
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
 * @brief Timer0Init初始化
 * @note 50毫秒中断
 */
void Timer0Init(void)		//50毫秒@11.0592MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x00;		//设置定时初值
	TH0 = 0x4C;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
}

/**
 * @brief 亮度等级显示
 * @param level Led的等级
 */
void Level_Display(u8 level)
{
    Display(7, 10);
    Display(8, level);
    Display(0, 0);
}

/**
 * @brief 显示运行模式和时间
 * 
 * @param flag 运行模式
 * @param time 所对应运行模式下的闪烁时间
 */
void Display_Mode_Time(u8 flag, u8 time)
{
    Display(1, 10);
    Display(2, flag);
    Display(3, 10);

    if(time < 10)
    {
        Display(5, 0);
        Display(6, time); 
    }
    else
    {
        Display(5, time / 10);
        Display(6, time % 10);       
    }

    Display(7, 0);
    Display(8, 0);
}
