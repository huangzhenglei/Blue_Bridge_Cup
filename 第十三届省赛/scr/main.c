#include <STC15F2K60S2.H>
#include "function.h"
#include <intrins.h>
#include "ds1302.h"
#include "onewire.h"    

u8 counter = 0;                                 //计时用

void Timer0_IT() interrupt 1                    //Time0中断函数
{
    static u8 Led_flag = GPIO_PIN_SET;

    counter++;
    if(counter >= 2)
    {
        counter = 0;
        Led_flag = !Led_flag;
        Led_Ctrl(LED[3], Led_flag);
    }
}

void main()
{
    u8 key = No_Press;                                          //按键初始化
    u8 mode = Temperture_Mode;                                  //显示界面模式初始化，初始化为温度界面
    u8 threshold = 23;                                          //温度阈值初始化
    u8 Ctrl_flag = Temperture_Ctrl;                             //温度时间控制模式变量
    float temperture = 0;                                       

    Buzz_Ctrl(GPIO_PIN_RESET);                                  //初始化，关闭蜂鸣器
    Relay_Ctrl(GPIO_PIN_RESET);                                 //初始化，关闭继电器
    RTC_Write(23, 59, 55);                                      //初始化时间为23，59，55
    Led_Ctrl(LED[2], !Ctrl_flag);                               //初始化时，为温度控制模式，灯2亮
    Timer0Init();                                               //初始化时钟0
    EA = 1;                                                     //打开全局中断
    ET0 = 0;                                                    //关闭Time0的中断

    while (1)
    {
        key = Key_Val();                                        //实时获取按键状态
        RTC_Read();                                             //实时更新时间
        temperture = rd_temperature_f();                        //实时获取时间

        if (key == s13)                                         //若s13按下，温度时间控制模式发生变化
        {                                                       //同时灯2的状态也发生变化，温度控制灯2亮，时间控制灯2灭
           Ctrl_flag = !Ctrl_flag;
           Led_Ctrl(LED[2], !Ctrl_flag);
        }

        if(Min == 0)                                            //判断是否整点，先判断分钟
        {
            if(Sec == 0)                                        //在判断秒钟
            {
                Led_Ctrl(LED[1], GPIO_PIN_RESET);               //是整点，灯1亮
            }
            else if (Sec >= 5)                                  //超过5秒后，灯1灭
            {
                Led_Ctrl(LED[1], GPIO_PIN_SET);
            }
        }
        
        switch(Ctrl_flag)                                       //温度时间控制模式
        {
            case Temperture_Ctrl:                               //温度控制

                if (temperture > threshold)                     //若温度大于阈值
                {
                    Relay_Ctrl(GPIO_PIN_SET);                   //开启继电器，同时灯3开始闪烁
                    ET0 = 1;
                }
                else
                {
                    Relay_Ctrl(GPIO_PIN_RESET);                 //若不大于，继电器关闭，灯3停止闪烁
                    ET0 = 0;
                    Led_Ctrl(LED[3], GPIO_PIN_SET);
                } 

                break;

            case Time_Ctrl:                                      //时间控制
                
                if(Min == 0)                                     //判断是否整点，先判断分钟            
                {
                    if( Sec == 0)                                //在判断秒钟
                    {
                        Relay_Ctrl(GPIO_PIN_SET);                //是整点，开启继电器，同时灯3开始闪烁
                        ET0 = 1;
                    }
                    else if (Sec >= 5)                           //超过5秒后，继电器关闭，灯3停止闪烁
                    {
                        Relay_Ctrl(GPIO_PIN_RESET);
                        ET0 = 0;
                        Led_Ctrl(LED[3], GPIO_PIN_SET);
                    }
                }

                break;    
        }

        switch (mode)                                             //控制界面显示
        {
        case Temperture_Mode:                                     //温度显示界面

                Display_Temperture(temperture * 10);              //显示温度

                if (key == s12)                                   //若s12按下，切换显示界面
                {
                   mode = Time_Mode;
                }

            break;

        case Time_Mode:                                         //时间显示界面

                if (key == s12)                                 //若s12按下，切换显示界面
                {
                    mode = Param_Set_Mode;
                }
                else if (Key_Scan() == s17)                     //再次判读s17是否按下，按下显示分，秒
                {
                    Display(1, 11);
                    Display(2, 2); 

                    Display(4, Min / 10);
                    Display(5, Min % 10);
                    Display(6, 12);
                    Display(7, Sec / 10);
                    Display(8, Sec % 10);
                }
                else 
                {
                    Display_Time();                              //若s12， s17都没按下，显示时，分
                }

            break;

        case Param_Set_Mode:                                     //阈值显示界面   

                Display_Threshlod(threshold);                    //显示阈值

                if (key == s12)                                  //若s12按下，切换显示界面
                {
                    mode = Temperture_Mode;
                }
                else if (key == s16)                             //若s16按下，阈值加一，最大99
                {
                    if(threshold != 99)
                    {
                        threshold++;
                    }
                    else 
                        ;
                }
                else if (key == s17)                             //若s17按下，阈值减一，最小10
                {
                    if(threshold != 10)
                    {
                        threshold--;
                    }
                    else 
                        ;  
                }
            break;
        
        default:
            break;
        }
    }
}
