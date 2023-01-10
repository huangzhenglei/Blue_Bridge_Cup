#ifndef __DS1302_H
#define __DS1302_H

extern unsigned char Hour ;
extern unsigned char Min ;
extern unsigned char Sec ;

void Write_Ds1302(unsigned char temp);
void Write_Ds1302_Byte( unsigned char address,unsigned char dat );
unsigned char Read_Ds1302_Byte( unsigned char address );

void RTC_Write(unsigned char hour, unsigned char minute, unsigned char second);
void RTC_Read();

#endif
