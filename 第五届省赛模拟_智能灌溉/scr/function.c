#include "function.h"
#include <STC15F2K60S2.H>
#include <intrins.h>
#include "ds1302.h"
	
u8 code COM[9] = {0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};		//.   //-
u8 code NUM[12] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0x7f, 0xbf};
u8 code LED[9] = {0xff, 0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f};
u8 buzz_relay_flag = 0x00;

void Delay3ms()		//@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	_nop_();
	i = 33;
	j = 66;
	do
	{
		while (--j);
	} while (--i);
}

/**
 * @brief LED控制
 * @param led 控制第几个灯
 * @param state 控制灯的状态
 * 		@arg GPIO_Pin_Reset LED开启
 * 		@arg GPIO_Pin_Set	LED关闭
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
		Led_flag |= ~led; 	 
	}

	P2 = (P2 & 0x1f) | 0x80;
	P0 = Led_flag;
	P2 &= 0x1f;
} 	

/**
 * @brief 蜂鸣器控制
 * @param state 控制蜂鸣器的状态
 * 		@arg GPIO_Pin_Reset 蜂鸣器关闭
 * 		@arg GPIO_Pin_Set	蜂鸣器开启
 * @return 无
 */
void Buzz_Ctrl(u8 state)
{
	P0 = buzz_relay_flag;
	P2 = (P2 & 0X1F) | 0xa0;
	P06 = state;
	P2 &= 0x1f;
	buzz_relay_flag = P0;
}

/**
 * @brief 控制继电器的状态
 * @param state 
 *  	@arg GPIO_Pin_Reset 继电器关闭
 * 		@arg GPIO_Pin_Set	继电器开启
 * @return 无
 */
void Relay_Ctrl(u8 state)
{
	P0 = buzz_relay_flag;
	P2 = (P2 & 0X1F) | 0xa0;
	P04 = state;
	P2 &= 0x1f;
	buzz_relay_flag = P0;
}

/**
 * @brief 按键扫描
 * @param 无
 * @return u8 返回按键的状态，如果没有按键按下，返回0x99
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
 * @brief 返回一次按键有效值
 * @param 无
 * @return u8 返回按键有效值
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
 * @brief 数码管显示
 * @param com 位选（控制第几个数码管亮）
 * @param num 段选（控制要显示的数字）
 * @return 无
 */
void Display(u8 com, u8 num)
{
	P2 = (P2 & 0X1F) | 0Xc0;
	P0 = COM[com];

	P2 = (P2 & 0X1F) | 0Xe0;
	P0 = NUM[num];

	P2 &= 0x1f;
	Delay3ms();
}

/**
 * @brief 显示时间
 * @param 无
 * @return 无
 */
void Display_Time()
{
	Display(1, Hour / 10);
	Display(2, Hour % 10);

	Display(3, 11);

	Display(4, Min / 10);
	Display(5, Min % 10);
}

/**
 * @brief 显示湿度
 * @param humidity 传入湿度的大小
 * @return 无
 */
void Display_Humidity(u8 humidity)
{

	Display(7, humidity / 10);
	Display(8, humidity % 10);

}

/**
 * @brief 显示阈值
 * @param threshold 传入阈值的大小
 * @return 无
 */
void Display_Threshold(u8 threshold)
{

	Display(1, 11);
	Display(2, 11);

	Display(7, threshold / 10);
	Display(8, threshold % 10);

}

