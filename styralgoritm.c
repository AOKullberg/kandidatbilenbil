/*
 * manuell_styrning.c
 *
 * Created: 4/6/2017 1:27:11 PM
 *  Author: kargu357
 */ 

#include "styralgoritm.h"
#include "main_styr.h"
#include "motorstyrning.h"

volatile unsigned char prior_error_right = 0x00;
volatile unsigned char prior_error_left = 0x00;

unsigned char calculate_error(unsigned char desired_distance, unsigned char actual_distance)
{
	unsigned char error = desired_distance - actual_distance;
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

void drive_forward(unsigned char distance_forward)
{
	if(distance_forward < stop_value_forward)
	{
		retardate();
	}
	accelerate();
}

void drive_backwards(unsigned char distance_backwards)
{
	if(distance_backwards < stop_value_backwards)
	{
		accelerate();
	}
	retardate();
}