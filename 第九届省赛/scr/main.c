#include <STC15F2K60S2.H>
#include <intrins.h>
#include "iic.h"
#include "function.h"

u8 counter = 0;                                     //计时用
u8 now_time = 0;                                    //生成100ms的时间

void Timer0_IT() interrupt 1                        //Timer0中断
{
    counter++;                                      
    if(counter >= 2)                                //每大于等于2后，将counter置0，生成100ms的时间
    {
        counter = 0;
        now_time++;                                 //now_time每次加一代表时间加100ms
    }
}

void main()
{
    u8 mode = Display_Mode;                         //模式控制变量
    u8 key = No_Press;                              //按键接收变量
    u8 state = Stop_State;                          //状态控制变量 
    u8 time[5] = {4, 4, 4, 4, 4};                   //初始化时间为4
    u8 time_flag = 1;                               //时间控制变量
    u8 led_flag = 1;                                //LED控制标志位变量
    float adc = 0.0;                                //接收ADC的值
    u8 level = 1;                                   //接收光强的亮度

    Relay_Buzz_Ctrl(0);                             //初始化，继电器和蜂鸣器关闭
    Led_Ctrl(0);                                    //初始化，LED关闭
    PCF8591_ADC_Init(0x03);                         //初始化，采集电位器的电压值
    Timer0Init();                                   //初始化Timer0
    EA = 1;                                         //打开全局中断
    ET0 = 0;                                        //Timer0中断先不打开

    if(EEPROM_Read(0) != 100)                       //判断是否是第一次上电
    {
        EEPROM_Write(0, 100);                       //是往0地址写100以及1，2，3，4第地址存初始化时间
        EEPROM_Write(1, 4);
        EEPROM_Write(2, 4);
        EEPROM_Write(3, 4);
        EEPROM_Write(4, 4);
    }
    else                                            //不是第一次上电，从EEPROM中读取数据
    {
        time[1] = EEPROM_Read(1);
        time[2] = EEPROM_Read(2);
        time[3] = EEPROM_Read(3);
        time[4] = EEPROM_Read(4);
    }

    while(1)
    {
        key = Key_Val();                            //实时接收按键返回值
        adc = (float)PCF8591_ADC_Get() * 5 / 255;   //实时接收ADC采集通道的返回值

        if(adc < 1.25)                              //如果adc < 1.25，亮度等级赋为1
        {
            level = 1;
        }
        else if(1.25 <= adc && adc < 2.5)           
        {
            level = 2;
        }
        else if(2.5 <= adc && adc <= 3.75)
        {
            level = 3;
        }
        else                    
        {
            level = 4;
        }

        switch (mode)                               
        {
        case Display_Mode:                          //显示模式下

            if(key == s6)                           //按下s6进入设置运行编号模式
            {
                mode = Set_Pattern_Mode;
                state = Stop_State;                 //设置时，LED状态设置为停止
                ET0 = 0;                            //关闭定时器0中断
                Led_Ctrl(0);                        //关闭LED
                time_flag = 1;                      //重新初始化为1
                led_flag = 1;                       //重新初始化为1
                break;                              //切换模式
            }
            else if(Key_Scan() == s4)               //一直按下s4，显示亮度等级
            {
                Level_Display(level);
            }
            else if(key == s7)                      //s7按下，LED开与关
            {
                state = !state;                     //状态取反
                if(state != Stop_State)             //若状态不等于停止状态
                {
                    Timer0Init();                   //重新初始化Timer0
                    ET0 = 1;                        //打开定时器0中断，LED开始工作
                }
                else                                //否则LED停止工作，关闭
                {
                    ET0 = 0;
                    Led_Ctrl(0);
                }
            }

            switch(state)                           //控制LED的状态
            {
                case Work_State:                    //工作状态

                    if(now_time >= time[time_flag]) //控制LED闪烁的时间
                    {
                        now_time = 0;               //重新置0
                        Led_Ctrl(led_flag);         //打开相应的LED灯
                        led_flag++;                 //控制相应的LED灯亮
                        if((led_flag == 9) || (led_flag == 16) || (led_flag == 20)) 
                        {
                            time_flag++;            //led_flag等于9或16或20时，time_flag加一
                        }
                        else if(led_flag > 22)      //LED到最后一个时， time_flag和led_flag重新置0
                        {
                            time_flag = 1;
                            led_flag = 1;
                        }
                    }

                    break;

                default:
                    break;
            }
           
            break;

        case Set_Pattern_Mode:                              //设置运行编号模式

            Display_Mode_Time(time_flag, time[time_flag]);  //显示运行编号和时间

            if(key == s6)                                   //按下s6，进入设置时间模式
            {
                mode = Set_Time_Mode;
                break;
            }
            else if (key == s5)                             //按下s5，运行编号加一，最大为4
            { 
                if(time_flag < 4)
                {
                    time_flag++;
                }
                else
                {
                    ;
                } 
            }
            else if(key == s4)                               //按下s4，运行编号减一，最小为1
            {
                if(time_flag != 1)
                {
                    time_flag--;
                }
                else
                {
                    ;
                }               
            }
           
            break;

        case Set_Time_Mode:                                 //设置时间模式

            Display_Mode_Time(time_flag, time[time_flag]);  //显示运行编号和时间
            
            if(key == s6)                                   //按下s6，进去显示模式
            {
                mode = Display_Mode;                
                Display(0, 1);                              //数码管熄灭
                EEPROM_Write(time_flag, time[time_flag]);   //存储相应的时间
                time_flag = 1;                              //time_flag重新置0
                break;
            }
            else if(key == s5)                              //s5按下，相应的时间加100ms，最大为1200ms
            {
                if(time[time_flag] != 12)
                {
                    time[time_flag]++;
                }
                else
                {
                    ;
                }
            }      
            else if(key == s4)                              //s3按下，相应的时间减100ms，最小为400ms
            {
                if(time[time_flag] != 4)
                {
                    time[time_flag]--;
                }
                else
                {
                    ;
                }
            }    

            break;
        
        default:
            break;
        }
    }

}
