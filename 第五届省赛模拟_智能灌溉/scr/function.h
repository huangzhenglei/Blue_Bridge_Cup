#ifndef __FUNCTION_H
#define __FUNCTION_H

#define u8 unsigned char
#define u16 unsigned int

#define GPIO_Pin_Reset 0
#define GPIO_Pin_Set   1

#define Automatic       0
#define Manual          1
#define HumidityAdjust  2

#define No_Press 0x99
#define s4 0x11
#define s5 0x12
#define s6 0x13
#define s7 0x14

extern u8 code LED[9];

void Delay3ms();
void Led_Ctrl(u8 led, u8 state);
void Buzz_Ctrl(u8 state);
void Relay_Ctrl(u8 state);
u8 Key_Scan();
u8 Key_Val();
void Display(u8 com, u8 num);

void Display_Time();
void Display_Humidity(u8 humidity);
void Display_Threshold(u8 threshold);

#endif // __FUNCTION_H

