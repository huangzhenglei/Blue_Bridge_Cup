#ifndef __FUNCTION_H
    #define __FUNCTION_H

#define u8 unsigned char
#define u16 unsigned int

#define GPIO_PIN_SET   1
#define GPIO_PIN_RESET 0

#define s4 0x11
#define s5 0x12
#define s6 0x13
#define s7 0x14
#define No_Press 0x00

#define Display_Mode         0x01
#define Set_Pattern_Mode     0x02
#define Set_Time_Mode        0x03

#define Work_State 0x00
#define Stop_State 0x01

void Delay1ms()	;
void Relay_Buzz_Ctrl(u8 state);
void Led_Ctrl(u8 led);
void Display(u8 com, u8 num);
u8 Key_Scan();
u8 Key_Val();
void Timer0Init(void)	;
void Level_Display(u8 level);
void Display_Mode_Time(u8 flag, u8 time);

#endif 

