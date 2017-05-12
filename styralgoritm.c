/*
 * manuell_styrning.c
 *
 * Created: 4/6/2017 1:27:11 PM
 *  Author: kargu357
 */

#include "math.h"
#include "styralgoritm.h"
#include "main.h"
#include "motorstyrning.h"
#include "gyro_2.h"
#include <avr/io.h>
#include <util/delay.h>


volatile short prior_right = 0;
volatile short prior_left = 0;

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
/*
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
	}*/
}

void one_line_control(char desired_distance, unsigned char actual_distance, signed char prior_error, char side_of_road)
{
/*
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
		prior_error = error;*/
}

short calculate_angle(void)
{
	short diff_l=prior_left-camera_left;
	short diff_r=prior_right-camera_left;
	
	short angle_l=10*atan(diff_l);
	short angle_r=10*atan(diff_r);
	
	if (lost_left)
	{
		return angle_r;
	}
	else if (lost_right)
	{
		return angle_l;
	}
	else
	{
		return (angle_l+angle_r)/2;
	}
	
}

void check_lines()
{
	if (camera_left==50)
	{
		lost_left=1;
	}
	else
	{
		lost_left=0;
	}
	
	if (camera_right==50)
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
	
	int prior_lost_left=lost_left;
	int prior_lost_right=lost_right;
	//int Kp_turn;
	

	check_lines();
	
	if ((lost_left && lost_right))
	{
		if (prior_lost_right)
		{
			camera_left=0;
		}
		if (prior_lost_left)
		{
			camera_right=0;
		}
	}
		
	short placement = camera_left - camera_right;
	prior_placement = prior_left-prior_right;
	turn_both_directions(8*(311+Kp*placement));
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
		_delay_ms(100);
	}
	brake(); // får testa och se hur lång bromsstäckan blir
}



void drive_backwards(unsigned char distance_backwards)
{
	if(distance_backwards < stop_value_backwards)
	{
		accelerate(1);
	}
	retardate(1);
}

void drive_for_time(char direction, int dist, unsigned char speed)
{
	turn_both_directions(319*8);
	int dist_passed = 0;

	while (velocity==0)
	{
		accelerate(speed);
	}
	
	if (direction == 'F')
	{
		accelerate(speed);
		while(dist > dist_passed)
		{			
			_delay_ms(10);
			dist_passed += velocity * 0.1;
		}
	}
	else if (direction == 'B')
	{
/*
		retardate(speed);
		while(dist >= dist_passed)
		{
			_delay_ms(10);
			dist_passed += velocity*0.01;
		}*/
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
			while (Angle > -55)
			{
				time_calc_angle = 0;
				TCNT0 = 0;
				turn_both_directions(0);
				accelerate(1);
				Get_Angle();
			}

				OCR1A = 320*8;
		}
		else if (direction_turn == 'L')
		{
			while (Angle < 65)
			{
				time_calc_angle = 0;
				TCNT0 = 0;
				turn_both_directions(10000);
				accelerate(1);
				Get_Angle();
			}

				OCR1A = 320*8;
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
				turn_both_directions(0);
				retardate(1);
				Get_Angle();
			}

				OCR1A = 320*8;
		}
		else if (direction_turn == 'L')
		{
			while (Angle > -75)
			{
				time_calc_angle = 0;
				TCNT0 = 0;				
				turn_both_directions(10000);
				retardate(1);
				Get_Angle();
			}

				OCR1A = 8*320;
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
				turn_both_directions(0);
				accelerate(1);
				Get_Angle();
			}

				OCR1A = 320*8;
		}
		else if (direction_turn == 'L')
		{
			while (Angle < degrees)
			{
				time_calc_angle = 0;
				TCNT0 = 0;
				turn_both_directions(10000);
				accelerate(1);
				Get_Angle();
			}

				OCR1A = 320*8;
		}
	}
	else if (direction == 'B')
	{
		if(direction_turn == 'R')
		{
			while (Angle < degrees)
			{
				turn_both_directions(0);
				retardate(1);
				Get_Angle();
			}

				OCR1A = 8*320;
		}
		else if (direction_turn == 'L')
		{
			while (Angle > -degrees)
			{
				turn_both_directions(10000);
				retardate(1);
				Get_Angle();
			}

				OCR1A = 8*320;
		}
	}
}

void drive_to_stopline(void)
{
	while (camera_front > 0)
	{
		transmit_spi(0xf0);
	}
	brake();
	_delay_ms(1000);
} 

void crossroad_left(void)
{
	_delay_ms(1000);
	drive_for_time('F',600,1);
	turn_x_degrees('F','L',20);
	
	lost_left=1;
	lost_right=1;

	while (lost_right)
	{
		transmit_spi(0xd0);
		check_lines();
		accelerate(1);
		turn_both_directions(10000);		
	}
		
	
//	turn_90_degrees('F','L');
}

void crossroad_right(void)
{
	lost_left=1;
	lost_right=1;
	
	_delay_ms(1000);
	while (lost_left||lost_right)
	{
		transmit_spi(0xe0);
		check_lines();
		accelerate(1);
		if (lost_right)
		{
			turn_both_directions(2000);
		}
		else
		{
			turn_both_directions(2560-1*camera_right);
		}
	}
	
}

void crossroad_forward(void)
{
	lost_left=1;
	lost_right=1;
	turn_both_directions(319*8);
	_delay_ms(1000);
	drive_for_time('F',300,1);
	while (lost_right)
	{
		check_lines();
		accelerate(1);
		turn_both_directions(8*(311+Kp*(camera_left-30)));
		
	}
}

