/*
 * manuell_styrning.c
 *
 * Created: 4/6/2017 1:27:11 PM
 *  Author: kargu357
 */

#include "styralgoritm.h"
#include "main.h"
#include "motorstyrning.h"
#include <avr/io.h>

#define Kp_speed 1

volatile unsigned char prior_error_right = 0x00;
volatile unsigned char prior_error_left = 0x00;

//extern volatile unsigned char velocity;
//extern volatile unsigned char distance_forward;

unsigned char calculate_error(unsigned char desired, unsigned char actual)
{
	unsigned char error = desired - actual;
	return error;
}

unsigned char derivate(unsigned char error, unsigned char prior_error)
{
	unsigned char derivative = (error - prior_error)/iteration_time;
	return derivative;
}

void pd_steering_control(unsigned char desired_distance, unsigned char actual_distance, unsigned char prior_error, char direction)
{
	unsigned char error = calculate_error(desired_distance, actual_distance);
	unsigned char derivative = derivate(error, prior_error);
	if(error > 0)
	{
		if(direction == 'R')
		{
			turn_left(error*Kp+Kd*derivative);
		}
		else if(direction == 'L')
		{
			turn_right(error*Kp+Kd*derivative);
		}
	}
	prior_error = error;
}



void cruise_control(unsigned char wanted_velocity)
{
	accelerate((calculate_error(wanted_velocity,velocity) * Kp_speed)/10);
	/*unsigned char distance_travelled_cruise;
	while (wanted_velocity != velocity)
	{
		if (velocity < wanted_velocity)
		{
		accelerate();
		//distance_travelled_cruise += velocity * 0.1;
		}
		retardate();
		//distance_travelled_cruise += velocity * 0.1;
	}
	//distance_travelled_cruise += velocity * 0.1;
	//return distance_travelled_cruise;
	return 0;*/
}


//Kanske inte ska ligga som en egen funktion
/*{
	distance = velocity * 0.1;
	return distance
}
*/

void drive_forward_distance(float distance_forward)
{
	float distance_travelled=0;
	while (distance_travelled < distance_forward)
	{
		if (distance_front < 30 )
		{
			brake();
			blink_led(6);
		}
		else
		{		
		cruise_control(30);
		//blink_led(3);			
		}
		distance_travelled += (float)velocity*0.0001; // Måste mäta hur lång tid loopen tar	
		OCR1B=motor_speed;
	}
	brake(); // får testa och se hur lång bromsstäckan blir
	OCR1B=motor_speed;
}



void drive_backwards(unsigned char distance_backwards)
{
	if(distance_backwards < stop_value_backwards)
	{
		accelerate(1);
	}
	retardate();
}
