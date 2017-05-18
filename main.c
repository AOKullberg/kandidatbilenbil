/*
 * KOM.c
 *
 * Created: 2017-03-26 16:50:59
 * Author : Tobias Fridén
 */ 




#include "main.h"
#include "sendrecieve.h"
#include "map.h"
#include "led_kom.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


/*Globala variabler
*/

//0=prog, 1=manual, 2=autonom
int mode=0;

/* Main-loop vid manuell körning
*/
void manual_main(void)
{
	blink_led(1,10);
	if (change_mode)
	{
		mode=2;
		change_mode=0;
	}
	else if (new_destination)
	{
		transmit_spi(0xaa);
		get_start();
		get_destination();
		transmit_uart0(0x72);
		new_route(destination);
		new_destination=0;
		mode=2;
		execute_node_end();
	}
	else
	{
		transmit_spi(computer_indata);	
	}
}

/* Main-loop vid autonom körning
*/
void autonom_main(void)
{
	blink_led(2,10);
	transmit_spi(0xc0);
	if (change_mode)
	{
		mode=1;
		change_mode=0;
	}
	else if (new_destination)
	{
		transmit_spi(0xaa);
		get_start();
		current_node=route[current];
		get_destination();
		transmit_uart0(0x72);
		new_route(destination);	
		new_destination=0;
	}
	else if ( steering_decision == 0xf0  )
	{

		if (distance_front > 15)
		{
			execute_node_end();
		}
		else 
		{
			transmit_spi(0xc0);
		}
		
	}
	else if (steering_decision==0xc0)
	{
		transmit_spi(0xc0);
	}
	else if (steering_decision==0xdd)
	{
		
	}

}

/*Main-loop vid programmeringsmode
*/
void prog_main(void)
{
	show_number(3);
	get_no_nodes();
	init_maps();
	update_distancemap();
	update_roadmap();
	get_start();
	get_destination();
	//transmit_uart0(no_nodes);
	//check_map(distancemap);
	//check_map(roadmap);
	transmit_uart0(0x72);
	new_route(destination);
	mode=1;
}


/*Mainloop
*/
int main(void)
{
	sei();
	uart_init();
	spi_init();
	//show_number(1);
	//blink_led(2,10);
	_delay_ms(1000);
	
    while (1) 
    {
		_delay_ms(50);

		switch(mode)
		{
			case 0 :
			prog_main();
			break;
			
			case 1 :
			send_data();
			manual_main();
			break;
			
			case 2 :
			send_data();
			autonom_main();
			break;
		}
    }
	
}

