#include "motorstyrning.h"
#include "main.h"
#include "gyro_2.h"
#include "styralgoritm.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "parkeringsficka.h"


void parkeringsficka(void)
{
	turn_x_degrees('F','R',28,0x83);
	int delay=0;

	while(delay < 150)
	{
		accelerate(20);
		turn_both_directions(8*(311+Kp*(50-camera_right)));
		_delay_ms(10);
		delay++;
		transmit_spi(0x83);
	}
	

	//turn_x_degrees('F', 'L', 20);

	while (camera_left > 25)
	{
		accelerate(20);
		//turn_both_directions(10000);
		two_line_control();
		transmit_spi(0x83);
	}
	brake();
	delay=0;
	while (delay < 3000)
	{
		_delay_ms(1);
		transmit_spi(0x83);
		delay++;
	}
	turn_x_degrees('F', 'L', 20, 0x83);
}
