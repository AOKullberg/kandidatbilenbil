/*
 * motorstyrning.c
 *
 * Created: 4/6/2017 2:09:23 PM
 *  Author: kargu357
 */
#define F_CPU 14745600UL

#include "styr_init.h"
#include "motorstyrning.h"
#include "main.h"
#include "autonom_drive.h"
#include "manuell_styrning.h"
#include "styralgoritm.h"
#include <util/delay.h>
#include <avr/io.h>




//Räknare för pwm-signal till styrservo
volatile unsigned short steering_degree = 320;
//Räknare för pwm-signal till motorservo
volatile unsigned short motor_speed = 345;

int reversing = 0;

//Svänger hjulen vänster
void turn_left(short data)
{
	if (data < 262 )
	{
		OCR1A = 262;
	}
	else if (data < 382)
	{
		OCR1A = data;
	}
	else
	{
		OCR1A = 382;
	}
}

//Svänger hjulen höger
void turn_right(short data)
{
	if (data > 382 )
	{
		OCR1A = 382;
	}
	else if (data > 262)
	{
		OCR1A = data;
	}
	else
	{
		OCR1A = 262;
	}
}

void turn_both_directions(short data)
{
	if (data > 380*8)
	{
		OCR1A = 380*8;
		//transmit_spi(0x56);
	}
	else if (data < 245*8)
	{
		OCR1A = 245*8;
		//transmit_spi(0x48);
	}
	else
	{
		OCR1A = data;
		if(data > 340)
		{
			//transmit_spi(0x56);
		}
		else if(data < 300)
		{
			//transmit_spi(0x48);
		}
		else
		{
			//transmit_spi(0x00);
		}
	}
}


void accelerate(unsigned char data)
{
	reversing = 0;
	short error=60-velocity;
	


	if (OCR1B < 2800)
	{
		OCR1B=2800;
	}
	else
	{
		if (error*Kp_speed + motor_speed < 2835)
		{
			OCR1B = motor_speed + error*Kp_speed;
		}
		else
		{
			OCR1B = 2835;
		}
	
	}

}


void retardate(unsigned char data)
	{
		if (! reversing )
		{
			OCR1B = 345*8;
			for (int i=0; i<177; i++)
			{
				_delay_ms(17);
			}
			reversing = 1;
		}
		switch (data) {
			case 1 :
			OCR1B = 330*8;
			break;

			case 2 :
			OCR1B = 327*8;
			break;

			case 3 :
			OCR1B = 326*8;
			break;

			case 4 :
			OCR1B = 325*8;
			break;
			
			case 5 :
			OCR1B = 324*8;
			break;
			
			case 6 :
			OCR1B = 323*8;
			break;
			
			case 7 :
			OCR1B = 322*8;
			break;			
		}
	}



void brake(void)
{
	
	if (OCR1B > 8*345)
	{
		OCR1B = 8*345;
/*
		_delay_ms(40);
		OCR1B=8*345;*/
	}
}
