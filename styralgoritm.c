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
		accelerate(60);
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
		accelerate(60);
	}
	retardate(1);
}

void drive_for_time(char direction, int dist, unsigned char speed, unsigned char steering_decision)
{
	transmit_spi(steering_decision);
	turn_both_directions(319*8);
	int dist_passed = 0;

	while (velocity==0)
	{
		accelerate(speed);
		transmit_spi(steering_decision);
	}
	
	if (direction == 'F')
	{
		accelerate(speed);
		while(dist > dist_passed)
		{	
			transmit_spi(steering_decision);		
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
				accelerate(60);
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
				accelerate(60);
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

void turn_x_degrees(char direction, char direction_turn, char degrees, unsigned char steering_decision)
{
	Angle = 0;
	if (direction == 'F')
	{
		if(direction_turn == 'R')
		{
			while (Angle > - degrees)
			{
				transmit_spi(steering_decision);
				time_calc_angle = 0;
				TCNT0 = 0;				
				turn_both_directions(0);
				accelerate(60);
				Get_Angle();
			}

				OCR1A = 320*8;
		}
		else if (direction_turn == 'L')
		{
			while (Angle < degrees)
			{
				transmit_spi(steering_decision);
				time_calc_angle = 0;
				TCNT0 = 0;
				turn_both_directions(10000);
				accelerate(60);
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
		accelerate(10);
		two_line_control();
		transmit_spi(0xf0);
	}
	brake();
	_delay_ms(1000);
	if(camera_front != 0)
	{
		while (camera_front > 0)
		{
			accelerate(10);
			two_line_control();
			transmit_spi(0xf0);
		}
		brake();
		_delay_ms(1000);
	}
} 

void crossroad_left(void)
{
	_delay_ms(1000);
	drive_for_time('F',600,60,0xd0);
	turn_x_degrees('F','L',30,0xd0);
	
	lost_left=1;
	lost_right=1;


	while (lost_right)
	{
		transmit_spi(0xd0);
		check_lines();
		accelerate(60);
		turn_both_directions(10000);		
	}
		
	
}

void crossroad_right(void)
{
	lost_left=1;
	lost_right=1;
	
	_delay_ms(1000);
	drive_for_time('F',100,20,0xe0);
	turn_x_degrees('F','R',20,0xe0);
	while (lost_left||lost_right)
	{
		transmit_spi(0xe0);
		check_lines();
		accelerate(60);
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
	drive_for_time('F',300,60,0x81);
	while (lost_right)
	{
		transmit_spi(0x81);
		check_lines();
		accelerate(60);
		turn_both_directions(8*(311+Kp*(camera_left-30)));
		
	}
}

void stopline(void)
{
	_delay_ms(1000);
	drive_for_time('F',100,60,0x82);
}

