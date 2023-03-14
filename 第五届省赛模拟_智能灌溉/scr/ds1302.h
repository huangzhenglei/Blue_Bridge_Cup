#ifndef __DS1302_H
#define __DS1302_H

extern unsigned char Hour;
extern unsigned char Min;
extern unsigned char Sec;

void Write_Ds1302(unsigned char temp);
void Write_Ds1302_Byte( unsigned char address,unsigned char dat );
unsigned char Read_Ds1302_Byte( unsigned char address );

void DS1302_Write(unsigned char second, unsigned char minute, unsigned char hour);
void DS1302_Read();

#endif
