/*
 * autonom_drive.c
 *
 * Created: 4/6/2017 3:04:17 PM
 *  Author: kargu357
 */

#include "autonom_drive.h"
#include "main.h"
#include "styralgoritm.h"

void autonomous_driving(void)
{
	//drive_forward(distance_forward);
	pd_steering_control(desired_distance_left, distance_left, prior_error_left, 'L');
	pd_steering_control(desired_distance_right, distance_right, prior_error_right, 'R');
}

void korsning(void)
{
	while(velocity > 0)
	{
		//stop_at_line();
	}
}
