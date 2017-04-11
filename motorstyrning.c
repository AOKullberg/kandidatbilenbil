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

//Accelererar bilen
void accelerate(unsigned char data)
{

	if (motor_speed < 329 )
	{
		motor_speed +=data;
	}
	else if(motor_speed < 352)
	{
		motor_speed = 352;
	}
	else if (motor_speed < 356)
	{
		motor_speed += data;
	}

}


//Sänker farten/backar
void retardate(void)
{
	if (motor_speed > 351 )
	{
		motor_speed -=1;
	}
	else if (motor_speed > 345 )
	{
		motor_speed = 345;
	}
	else if(motor_speed > 332 )
	{
		for (int i=0; i<177; i++)
		{
			_delay_ms(17);
		}
	motor_speed=332;
	}
	else if (motor_speed > 325 )
	{
		motor_speed -= 1;
	}

}

void brake(void)
{
	motor_speed=345;
}
