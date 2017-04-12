/*
 * motorstyrning.c
 *
 * Created: 4/6/2017 2:09:23 PM
 *  Author: kargu357
 */
#define F_CPU 14745600UL

#include "motorstyrning.h"
#include <util/delay.h>


//Räknare för pwm-signal till styrservo
volatile unsigned short steering_degree = 320;
//Räknare för pwm-signal till motorservo
volatile unsigned short motor_speed = 345;

int reversing = 0;

//Svänger hjulen vänster
void turn_left(unsigned char data)
{
	if (steering_degree < 400 )
	{
		steering_degree += data;
	}
}

//Svänger hjulen höger
void turn_right(unsigned char data)
{
	if (steering_degree > 250 )
	{
		steering_degree -= data;
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
	}
	OCR1B = motor_speed;

}

void retardate(unsigned char data)
	{
		if (! reversing )
		{
			OCR1B = 345;
			for (int i=0; i<177; i++)
			{
				_delay_ms(17);
			}
			reversing = 1;
		}
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

		}
	}



void brake(void)
{
	motor_speed=345;
}
