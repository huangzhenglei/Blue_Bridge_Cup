#ifndef _IIC_H
#define _IIC_H

void Delay5ms()	;

void IIC_Start(void); 
void IIC_Stop(void);  
bit IIC_WaitAck(void);  
void IIC_SendAck(bit ackbit); 
void IIC_SendByte(unsigned char byt); 
unsigned char IIC_RecByte(void); 

void EEPROM_Write(unsigned char addr, unsigned char date);
unsigned char EEPROM_Read(unsigned char addr);
void EEPROM_ContinuousRead(unsigned char addr, unsigned char *date, unsigned char num);
void PCF8591_ADC_Init(unsigned char channel);
unsigned char PCF8591_ADC_Get();

#endif