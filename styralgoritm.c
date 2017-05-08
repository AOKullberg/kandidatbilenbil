/*
 * manuell_styrning.c
 *
 * Created: 4/6/2017 1:27:11 PM
 *  Author: kargu357
 */

#include "styralgoritm.h"
#include "main.h"
#include "motorstyrning.h"
#include "gyro_2.h"
#include <avr/io.h>
#include <util/delay.h>

#define Kp_speed 1

volatile signed char prior_error_right = 0x00;
volatile signed char prior_error_left = 0x00;

int lost_right=0;
int lost_left=0;

volatile short prior_placement = 0;

signed char calculate_error(char desired, unsigned char actual)
{
	signed char error = desired - actual;
	return error;
}

signed char derivate(char error, signed char prior_error)
{
	signed char derivative = (error - prior_error)/*/iteration_time*/;
	return derivative;
}

void pd_steering_control(char desired_distance, unsigned char actual_distance, signed char prior_error, char direction)
{
	signed char error = calculate_error(desired_distance, actual_distance);
	signed char derivative = derivate(error, prior_error);
	if(error > 0)
	{
		if(direction == 'R')
		{
			turn_left(error*Kp+320+Kd*derivative);
		}
		else if(direction == 'L')
		{
			turn_right(320-error*Kp-Kd*derivative);
		}
		prior_error = error;
	}
	else
	{
		//OCR1A = 320;
		prior_error = 0;
	}
}

void one_line_control(char desired_distance, unsigned char actual_distance, signed char prior_error, char side_of_road)
{
	signed char error = calculate_error(desired_distance, actual_distance);
	signed char derivative = derivate(error, prior_error);
	if(side_of_road == 'R')
	{
		turn_both_directions(error*Kp+320+Kd*derivative);
		prior_error_left = 0;
	}
	else if(side_of_road == 'L')
	{
		turn_both_directions(320-(error*Kp+Kd*derivative));
		prior_error_right = 0;
	}
		prior_error = error;
}

void check_lines()
{
	if (camera_left==100)
	{
		lost_left=1;
	}
	else
	{
		lost_left=0;
	}
	
	if (camera_right==100)
	{
		lost_right=1;
	}
	else
	{
		lost_right=0;
	}
}

void two_line_control(void)
{
	int prior_left=lost_left;
	int prior_right=lost_right;
	
	check_lines();
	
	if ((lost_left && lost_right))
	{
		if (prior_right)
		{
			camera_left=0;
		}
		if (prior_left)
		{
			camera_right=0;
		}
	}
	
	
	short placement = camera_left - camera_right;
	short derivative = placement-prior_placement;
	executed_command=derivative;
	turn_both_directions(8*(311+Kp*placement-Kd*derivative));
	_delay_ms(50);
	prior_placement = placement;
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
			//blink_led(6);
		}
		else
		{
		accelerate(1);
		//blink_led(3);
		}
		distance_travelled += (float)velocity*0.1; // Måste mäta hur lång tid loopen tar
		OCR1B=motor_speed;
		_delay_ms(100);
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
	retardate(1);
}

void drive_for_time(char direction, int time, unsigned char speed)
{
	turn_both_directions(319);
	int time_passed = 0;
	if (direction == 'F')
	{
		accelerate(speed);
		while(time >= time_passed)
		{			
			_delay_ms(1);
			time_passed = time_passed + 1;
		}
		brake();
	}
	else if (direction == 'B')
	{
		retardate(speed);
		while(time >= time_passed)
		{
			_delay_ms(1);
			time_passed = time_passed + 1;
		}
		brake();
	}
	else
	{
		//blink_led(6);
	}
}

void turn_90_degrees(char direction, char direction_turn)
{
	Angle = 0;
	if (direction == 'F')
	{
		if(direction_turn == 'R')
		{
			while (Angle > -80)
			{
				time_calc_angle = 0;
				TCNT0 = 0;
				OCR1A = 262;
				accelerate(1);
				Get_Angle();
			}

				OCR1A = 320;
				brake();
		}
		else if (direction_turn == 'L')
		{
			while (Angle < 60)
			{
				time_calc_angle = 0;
				TCNT0 = 0;
				OCR1A = 382;
				accelerate(1);
				Get_Angle();
			}

				OCR1A = 320;
				brake();
		}
	}
	else if (direction == 'B')
	{
		if(direction_turn == 'R')
		{
			while (Angle < 85)
			{
				time_calc_angle = 0;
				TCNT0 = 0;				
				OCR1A = 262;
				retardate(1);
				Get_Angle();
			}

				OCR1A = 320;
				brake();
		}
		else if (direction_turn == 'L')
		{
			while (Angle > -85)
			{
				time_calc_angle = 0;
				TCNT0 = 0;				
				OCR1A = 382;
				retardate(1);
				Get_Angle();
			}

				OCR1A = 320;
				brake();
		}
	}
}

void turn_x_degrees(char direction, char direction_turn, char degrees)
{
	Angle = 0;
	if (direction == 'F')
	{
		if(direction_turn == 'R')
		{
			while (Angle > - degrees)
			{
				OCR1A = 259;
				accelerate(1);
				_delay_ms(100);
				Get_Angle();
			}

				OCR1A = 320;
				brake();
		}
		else if (direction_turn == 'L')
		{
			while (Angle < degrees)
			{
				OCR1A = 373;
				accelerate(1);
				_delay_ms(100);
				Get_Angle();
			}

				OCR1A = 320;
				brake();
		}
	}
	else if (direction == 'B')
	{
		if(direction_turn == 'R')
		{
			while (Angle < degrees)
			{
				OCR1A = 259;
				retardate(1);
				_delay_ms(100);
				Get_Angle();
			}

				OCR1A = 320;
				brake();
		}
		else if (direction_turn == 'L')
		{
			while (Angle > -degrees)
			{
				OCR1A = 373;
				retardate(1);
				_delay_ms(100);
				Get_Angle();
			}

				OCR1A = 320;
				brake();
		}
	}
}

void drive_to_stopline(void)
{
	//while (velocity > 0)
	//{
		//OCR1B = 326;
	//}
	brake();
} 

