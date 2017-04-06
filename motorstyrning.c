/*
 * motorstyrning.c
 *
 * Created: 4/6/2017 2:09:23 PM
 *  Author: kargu357
 */ 

#include "motorstyrning.h"

//Räknare för pwm-signal till styrservo
volatile unsigned short steering_degree = 320;
//Räknare för pwm-signal till motorservo
volatile unsigned short motor_speed = 340;

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
void accelerate(void)
{
	if (motor_speed < 340 )
	{
		motor_speed = 340;
	}
	else if (motor_speed < 350 )
	{
		motor_speed += 1;
	}
}

//Sänker farten/backar
void retardate(void)
{
	if (motor_speed > 340 )
	{
		motor_speed = 340;
	}
	else if (motor_speed > 327 )
	{
		motor_speed -= 1;
	}
}
