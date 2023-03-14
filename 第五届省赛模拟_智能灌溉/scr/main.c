#include <STC15F2K60S2.H>
#include <intrins.h>
#include "function.h"
#include "ds1302.h"
#include "iic.h"

void main()
{
	u8 key = No_Press;
	u8 mode = Automatic;
	float humidity = 0;
	u8 threshold = 50;
	u8 buzz_flag = 1;

	Led_Ctrl(LED[1], GPIO_Pin_Reset);
	Buzz_Ctrl(GPIO_Pin_Reset);
	Relay_Ctrl(GPIO_Pin_Reset);
	DS1302_Write(0, 30, 8);

	PCF8591_ADC_Init(0x03);

    while (1)
    {
		DS1302_Read(); 
		humidity = (float)PCF8591_ADC_Get() / 255 * 99;
		key = Key_Val();

		switch(mode)
		{
			case Automatic:

				Display_Time();
				Display_Humidity(humidity);

				if(humidity < threshold)
				{
					Relay_Ctrl(GPIO_Pin_Set);
				}
				else
				{
					Relay_Ctrl(GPIO_Pin_Reset);
				}

				if(key == s7)
				{
					mode = Manual;
					Led_Ctrl(LED[1], GPIO_Pin_Set);
					Led_Ctrl(LED[2], GPIO_Pin_Reset);
					Relay_Ctrl(GPIO_Pin_Reset);
					buzz_flag = 1;
				}
				else if(key == s6)
				{
					mode = HumidityAdjust;
					Relay_Ctrl(GPIO_Pin_Reset);
				}

				break;

			case Manual:

				Display_Time();
				Display_Humidity(humidity);

				if(buzz_flag)
				{
					if(humidity < threshold)
					{
						Buzz_Ctrl(GPIO_Pin_Set);
					}
					else
					{
						Buzz_Ctrl(GPIO_Pin_Reset);
					}
				}

				if(key == s6)
				{
					buzz_flag = !buzz_flag;
				}
				else if(key == s5)
				{
					Relay_Ctrl(GPIO_Pin_Set);
				}
				else if(key == s4)
				{
					Relay_Ctrl(GPIO_Pin_Reset);
				}
				else if(key == s7)
				{
					mode = Automatic;
					Buzz_Ctrl(GPIO_Pin_Reset);
					Led_Ctrl(LED[1], GPIO_Pin_Reset);
					Led_Ctrl(LED[2], GPIO_Pin_Set);					
				}

				break;

			case HumidityAdjust:
				Display_Threshold(threshold);

				if(key == s5)
				{
					if(threshold != 99)
						threshold++;
					else
						;
				}
				else if (key == s4)
				{
					if(threshold != 0)
						threshold--;
					else
						;
				}
				else if(key == s6)
				{
					mode = Automatic;
					EEPROM_Write(0, threshold);
				}

				break;

			default:
				break;
		}

    }
}
