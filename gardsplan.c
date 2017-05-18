/*
 * gardsplan.c
 *
 * Created: 4/11/2017
 *  Author: carra862
 */


 #include "styralgoritm.h"
 #include "main.h"
 #include "motorstyrning.h"
 #include <avr/io.h>
 #include <util/delay.h>
 #include "gyro_2.h"



void from_garage_to_house(void)
{
	turn_x_degrees('B','R', 50,0x00);
	_delay_ms(3000);
	drive_forward_distance(1);
	turn_x_degrees('F','L',110,0x00);
	drive_to_stopline();
}

void in_from_upside(void)
{
	turn_90_degrees('F','R');
	drive_forward_distance(10);
	turn_90_degrees('F','L');
	turn_90_degrees('F','L');
	turn_90_degrees('F','R');
	while (distance_front > 5)
	{
		accelerate(60);
	}
	brake();
}

void in_from_side(void)
{
	turn_90_degrees('F','R');
	turn_90_degrees('F','L');
	turn_90_degrees('F','R');
	while (distance_front > 5)
	{
		accelerate(60);
	}
	brake();	
}

void out_from_yard(char direction)
{
	from_garage_to_house();
	if (direction == 'F')
	{
		drive_to_stopline();
	}
	else if (direction == 'L')
	{
		turn_90_degrees('F','L');
		drive_to_stopline();
	}
}

void in_to_yard(char direction)
{
	if (direction == 'U')
	{
		in_from_upside();
	}
	else if (direction == 'S')
	{
		in_from_side();	
	}
}
