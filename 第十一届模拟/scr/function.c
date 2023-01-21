#include "function.h"
#include <STC15F2K60S2.H>

u8 code LED[9] = {0xff, 0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f};
u8 code COM[9] = {0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};       //-    p     e      n     c
u8 code NUM[15] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0xbf, 0x8c, 0x86, 0xc8, 0xc6};

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
 *@brief 继电器控制
 *@param relay 继电器开关控制参数
 *              GPIO_PIN_RESET 继电器关闭
 *              GPIO_PIN_SET   继电器开启
 * @return 无
 */
void Relay_Ctrl(u8 state)
{
    P0 = 0x00;
    P2 = (P2 & 0X1F) | 0XA0;
    P04 = state;
    P2 &= 0X1F; 
}

/*
 *@brief 关闭蜂鸣器和继电器     
 *@param 无
 *@return 无
 */
void Close()
{
    P2 = (P2 & 0X1F) | 0XA0;
    P0 = 0x00;
    P2 &= 0X1F; 
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
 *@brief	矩阵按键
 *@note		无
 *@param	无
 *@retval	返回对应按下的功能
 */
u8 Key_Map()
{
    u8 key = No_Press;

    key = Key_Val();

    switch (key)
    {
    case s4:
        return No_Press;
        break;

    case s5:
        return 8;
        break;    

    case s6:
        return 4;
        break;

    case s7:
        return 0;
        break;
    
    case s8:            //清除按键
        return s8;  
        break;

    case s9:
        return 9;
        break;

    case s10:
        return 5;
        break;

    case s11:
        return 1;
        break;

    case s12:          //修改按键
        return s12;     
        break;

    case s13:
        return No_Press;
        break;

    case s14:
        return 6;
        break;

    case s15:
        return 2;
        break;

    case s16:           //输入按键
        return s16;  
        break;

    case s17:
        return No_Press;
        break;

    case s18:
        return 7;
        break;

    case s19:
        return 3;
        break;

    case No_Press:
        return No_Press;
        break;        

    default:
        return No_Press;
        break;
    }
}

/**
 * @brief 显示输入的密码
 * 
 * @param input_password  输入密码的数组
 * @param num             当前输入了几个数字
 */
void Display_Passward(u8 input_password[], u8 num)
{
    Display(1, 10);

    switch(num)
    {
        case 1:
        Display(8, input_password[1]);
        break;

        case 2:
        Display(7, input_password[1]);
        Display(8, input_password[2]);
        break;

        case 3:
        Display(6, input_password[1]);
        Display(7, input_password[2]);
        Display(8, input_password[3]);
        break;

        case 4:
        Display(5, input_password[1]);
        Display(6, input_password[2]);
        Display(7, input_password[3]);
        Display(8, input_password[4]);
        break;

        case 5:
        Display(4, input_password[1]);
        Display(5, input_password[2]);
        Display(6, input_password[3]);
        Display(7, input_password[4]);
        Display(8, input_password[5]);
        break;

        case 6:
        Display(3, input_password[1]);
        Display(4, input_password[2]);
        Display(5, input_password[3]);
        Display(6, input_password[4]);
        Display(7, input_password[5]);
        Display(8, input_password[6]);
        break;

        default :
        break;
    }
  
}

/**
 * @brief 定时器0初始化
 * 
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
 * @brief 显示open
 * 
 */
void Display_Open()
{
    Display(1, 0);
    Display(5, 0);
    Display(6, 11);
    Display(7, 12);
    Display(8, 13);
}

/**
 * @brief 定时器1初始化
 * 
 */
void Timer1Init(void)		//50毫秒@11.0592MHz
{
	AUXR &= 0xBF;		//定时器时钟12T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x00;		//设置定时初值
	TH1 = 0x4C;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
}

/**
 * @brief 显示修改的密码
 * 
 * @param input_password 输入修改密码的数组
 * @param num            当前输入了几个数字 
 */
void Display_Revise_Passward(u8 input_password[], u8 num)
{
    Display(1, 14);

    switch(num)
    {
        case 1:
        Display(8, input_password[1]);
        break;

        case 2:
        Display(7, input_password[1]);
        Display(8, input_password[2]);
        break;

        case 3:
        Display(6, input_password[1]);
        Display(7, input_password[2]);
        Display(8, input_password[3]);
        break;

        case 4:
        Display(5, input_password[1]);
        Display(6, input_password[2]);
        Display(7, input_password[3]);
        Display(8, input_password[4]);
        break;

        case 5:
        Display(4, input_password[1]);
        Display(5, input_password[2]);
        Display(6, input_password[3]);
        Display(7, input_password[4]);
        Display(8, input_password[5]);
        break;

        case 6:
        Display(3, input_password[1]);
        Display(4, input_password[2]);
        Display(5, input_password[3]);
        Display(6, input_password[4]);
        Display(7, input_password[5]);
        Display(8, input_password[6]);
        break;

        default :
        break;
    }
  
}

