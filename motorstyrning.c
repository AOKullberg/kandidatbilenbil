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
	if (OCR1A < 260 )
	{
		OCR1A = 260;
	}
	else if (OCR1A < 374)
	{
		OCR1A = data;
	}
	else
	{
		OCR1A = 374;
	}
}

//Svänger hjulen höger
void turn_right(short data)
{
	if (OCR1A > 374 )
	{
		OCR1A = 374;
	}
	else if (OCR1A > 260)
	{
		OCR1A = data;
	}
	else
	{
		OCR1A = 260;
	}
}

void turn_both_directions(short data)
{
	if (OCR1A > 374)
	{
		OCR1A = 374;
		transmit_spi(0x56);
	}
	else if (OCR1A < 260)
	{
		OCR1A = 260;
		transmit_spi(0x48);
	}
	else
	{
		OCR1A = data;
		if(data > 340)
		{
			transmit_spi(0x56);
		}
		else if(data < 300)
		{
			transmit_spi(0x48);
		}
		else
		{
			transmit_spi(0x00);
		}
	}
}


void accelerate(unsigned char data)
{
	reversing = 0;
	switch (data)
	{
		case 1:
		motor_speed = 353;  //ungefär 30cm/s min hastighet
		break;

		case 2:
		motor_speed = 354; //ungefär 45cm/s
		break;

		case 3:
		motor_speed = 355; //ungefär 70cm/s
		break;

		case 4:
		motor_speed = 356; //ungefär 95cm/s max hastighet
		break;
		
		case 5:
		motor_speed = 357;
		break;
		
		case 6:
		motor_speed = 358;
		break;
	}
	OCR1B = motor_speed;
	transmit_spi(0x41);
}

//void turn_camera

void retardate(unsigned char data)
	{
		/*if (! reversing )
		{
			OCR1B = 345;
			for (int i=0; i<177; i++)
			{
				_delay_ms(17);
			}
			reversing = 1;
		}*/
		switch (data) {
			case 1 :
			OCR1B = 328;
			break;

			case 2 :
			OCR1B = 327;
			break;

			case 3 :
			OCR1B = 326;
			break;

			case 4 :
			OCR1B = 325;
			break;
			
			case 5 :
			OCR1B = 324;
			break;
			
			case 6 :
			OCR1B = 323;
			break;
			
			case 7 :
			OCR1B = 322;
			break;			
		}
		transmit_spi(0x52);
	}



void brake(void)
{
	OCR1B=345;
	transmit_spi(0x53);
}
