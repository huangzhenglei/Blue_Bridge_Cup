#include <STC15F2K60S2.H>
#include "function.h"
#include "iic.h"

u8 counter = 0;                             //定时器0计时用
u8 counter1 = 0;                            //定时器1计时用
u8 mode = Close_Mode;                       //定义全局变量mode，控制模式

void Timer0_IT() interrupt 1                //定时器0中断，控制输入错误时LED1亮5s
{
    counter++;
    if(counter >= 100)                      //到达了5s
    {
        ET0 = 0;                            //定时器0中断关闭
        counter = 0;                        //counter重新初始化为0
        Led_Ctrl(LED[1], GPIO_PIN_SET);     //关闭LED1
        mode = Close_Mode;                  //模式切换为Close_Mode
    }
}

void Timer1_IT() interrupt 3                //定时器1中断，控制输入正确时
{
    counter1++;
    if(counter1 >= 100)                     //到达了5s
    {
        ET1 = 0;                            //定时器1中断关闭
        counter1 = 0;                       //counter1重新初始化为0
        Relay_Ctrl(GPIO_PIN_RESET);         //继电器关闭
        mode = Close_Mode;                  //模式切换为Close_Mode
    }
}

void main()
{
    u8 key = No_Press;                      //接收按键返回的值，初始化为No_Press
    u8 password[7] = {8, 8, 8, 8, 8, 8, 8}; //密码数组，初始化密码为6个8
    u8 input_password[7] = {0};             //输入密码时的数组，初始化为0
    u8 password_flag = 0;                   //数组控制变量

    Close();                                //初始化，关闭继电器和蜂鸣器
    Led_Ctrl(LED[0], GPIO_PIN_SET);         //初始化，关闭所有LED
    Timer0Init();                           //初始化定时器0
    Timer1Init();                           //初始化定时器1
    EA = 1;                                 //打开全局中断
    ET0 = 0;                                //关闭定时器0中断
    ET1 = 0;                                //关闭定时器1中断

    if(EEPROM_Read(0) != 125)               //判断是否第一次上电
    {
        EEPROM_Write(0, 125);               //若第一次上电，往0地址写125，1~6地址写密码6个8
        EEPROM_Write(1, password[1]);
        EEPROM_Write(2, password[2]);
        EEPROM_Write(3, password[3]);
        EEPROM_Write(4, password[4]);
        EEPROM_Write(5, password[5]);
        EEPROM_Write(6, password[6]);
    }
    else                                    //不是第一次上电，从地址1~6读出密码
    {
        password[1] = EEPROM_Read(1);
        password[2] = EEPROM_Read(2);
        password[3] = EEPROM_Read(3);
        password[4] = EEPROM_Read(4);
        password[5] = EEPROM_Read(5);
        password[6] = EEPROM_Read(6);
    }

    while (1)
    {
        key = Key_Map();                    //实时接收按键返回值

        switch (mode)                       //模式控制
        {
            case Close_Mode:                //关闭模式

                P2 = (P2 & 0X1F) | 0XC0;    //数码管全部关闭
                P0 = 0x00;
                P2 &= 0x1f;

                if(key == s16)              //按下s16进入输入密码模式
                {
                    mode = Input_Mode;
                    password_flag = 0;      //初始化数组控制变量为0
                    Led_Ctrl(LED[7], GPIO_PIN_RESET);               //LED7打开
                }
                
                break;

            case Input_Mode:                                        //输入密码模式

                Display_Passward(input_password, password_flag);    //显示输入的密码

                if(0 <= key && key <= 9)                            //若按键返回0~9，进入其中
                {  
                    if(password_flag != 6)                          //若数组控制变量password_flag不等于6，说明没有密码输入完
                    {
                        password_flag++;                            //password_flag先自增1
                        input_password[password_flag] = key;        //将按键所返回的值放到输入密码数组中

                        if(password_flag == 6)                      //判断密码是否输入完成
                        {
                            int i = 1;                              //for循环控制变量   
                            u8 flag = 0;                            //判断是否密码正确变量

                            for(i = 1; i < 7; i++)                  //判断密码是否正确
                            {
                                if(input_password[i] == password[i])
                                {
                                    flag++;                         //密码正确，flag自增1
                                }
                                else
                                {
                                    break;                          //否则直接跳出for循环
                                }
                            }

                            if(flag == 6)                           //flag等于6说明密码正确
                            {   
                                Relay_Ctrl(GPIO_PIN_SET);           //继电器打开
                                Led_Ctrl(LED[7], GPIO_PIN_SET);     //LED7关闭
                                counter1 = 0;                       //counter1重新初始化为0
                                ET1 = 1;                            //打开定时器1中断，计时
                                mode = Open_Mode;                   //模式切换为Open_Mode
                                break;
                            }
                            else
                            {
                                ET0 = 1;                            //否则密码错误，打开定时器0中断
                                Led_Ctrl(LED[1], GPIO_PIN_RESET);   //LED1亮
                            }
                        }
                    }  
                }
                else if(key == s8)                                  //按下s8，清除按键
                { 
                    Led_Ctrl(LED[1], GPIO_PIN_SET);                 //关闭LED1
                    ET0 = 0;                                        //关闭定时器0
                    counter = 0;                                    //counter重新初始化为0
                    if(password_flag != 0)                          //判断是否清除完数字
                    {
                        password_flag--;                            //若没清除完，password_flag减1
                    }
                    else
                    {
                        Display(0, 0);                              //清除完，数码管全部关闭
                    }
                }
                
                break;

            case Open_Mode:                                         //密码正确，显示OPEN模式
                
                Display_Open();                                     //显示OPEN

                if(key == s12)                                      //按下s12
                {   
                    ET1 = 0;                                        //定时器1中断关闭
                    password_flag = 0;                              //password_flag重新初始化为0
                    Relay_Ctrl(GPIO_PIN_RESET);                     //继电器关闭
                    Led_Ctrl(LED[8], GPIO_PIN_RESET);               //LED8亮
                    mode = Revise_Mode;                             //模式切换为Revise_Mode
                    break;
                }

                break;

            case Revise_Mode:                                        //修改密码模式
                
                Display_Revise_Passward(password, password_flag);    //显示修改的密码

                if(0 <= key && key <= 9)                             //判断按键是否返回0~9
                {
                     if(password_flag != 6)                          //若数组控制变量password_flag不等于6，说明修改密码没有输入到最后一个
                    {
                        password_flag++;                             //password_flag先自增1
                        password[password_flag] = key;               //将按键所返回的值放到密码数组中
                        EEPROM_Write(password_flag, password[password_flag]);   //将密码存储到EEPROM中
                    }
                    else                                             //密码输入到最后一个
                    {
                        password[password_flag] = key;               //将按键所返回的值放到密码数组中
                        EEPROM_Write(password_flag, password[password_flag]);   //将密码存储到EEPROM中
                    }              
                }  
                else if(key == s8)                                   //按下s8，清除按键
                { 
                    if(password_flag != 0)                           //判断是否清除完数字
                    {
                        password_flag--;                             //若没清除完，password_flag减1
                    }
                    else
                    {
                        Display(0, 0);                               //清除完，数码管全部关闭
                    }
                }
                else if (key == s12)                                 //按下s12，返回Close_Mode模式
                {
                    if(password_flag == 6)                           //判断修改密码是否输入完成
                    {
                        mode = Close_Mode;                           //模式切换为Close_Mode
                        Led_Ctrl(LED[8], GPIO_PIN_SET);              //LED8灭
                        break;      
                    }
                }

                break;
            
            default:
                break;
        }

    }
}