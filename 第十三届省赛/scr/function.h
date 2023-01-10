#ifndef __FUNCTION_H
    #define __FUNCTION_H

#define u8 unsigned char
#define u16 unsigned int

#define GPIO_PIN_SET   1
#define GPIO_PIN_RESET 0

#define No_Press 0x00
#define s4 0x11
#define s5 0x12
#define s6 0x13
#define s7 0x14
#define s8 0x21
#define s9 0x22
#define s10 0x23
#define s11 0x24
#define s12 0x31
#define s13 0x32
#define s14 0x33
#define s15 0x34
#define s16 0x41
#define s17 0x42
#define s18 0x43
#define s19 0x44

#define Temperture_Mode 0x01
#define Time_Mode       0x02
#define Param_Set_Mode 0x03

#define Temperture_Ctrl 0x01
#define Time_Ctrl       0x00

extern u8 code LED[9];

void Led_Ctrl(u8 led, u8 state);
void Buzz_Ctrl(u8 state);
void Relay_Ctrl(u8 state);
void Display(u8 com, u8 num);
u8 Key_Scan();
u8 Key_Val();

void Display_Temperture(float temperture);
void Display_Time();
void Display_Threshlod(u8 threshlod);
void Timer0Init(void);

#endif 


