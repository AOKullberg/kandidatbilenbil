/*
 * autonom_drive.c
 *
 * Created: 4/6/2017 3:04:17 PM
 *  Author: kargu357
 */

#include "autonom_drive.h"
#include "main.h"
#include "styralgoritm.h"
#include "motorstyrning.h"

char previous_side = 'L';

void autonomous_driving(void)
{
	accelerate(1);
	if(camera_left < camera_right)
	{
		one_line_control(desired_distance_left, camera_left, prior_error_left, 'L');
	}
	else if(camera_right < camera_left)
	{
		one_line_control(desired_distance_right, camera_right, prior_error_right, 'R');
	}
}

void korsning(void)
{
	while(velocity > 0)
	{
		//stop_at_line();
	}
}
