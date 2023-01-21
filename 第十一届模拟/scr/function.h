#ifndef __FUNCTION_H
    #define __FUNCTION_H

#define u8 unsigned char
#define u16 unsigned int  

#define GPIO_PIN_SET   1
#define GPIO_PIN_RESET 0

#define No_Press 0x99
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

#define Close_Mode 0x01 
#define Input_Mode 0x02
#define Open_Mode  0x03
#define Revise_Mode 0x04

extern u8 code LED[9];

void Led_Ctrl(u8 led, u8 state);
void Relay_Ctrl(u8 state);
void Close();
void Display(u8 com, u8 num);
u8 Key_Scan();
u8 Key_Val();
u8 Key_Map();
void Display_Passward(u8 input_password[], u8 num);
void Timer0Init(void);
void Display_Open();
void Timer1Init(void);
void Display_Revise_Passward(u8 input_password[], u8 num);

#endif // !__FUNCTION_H

