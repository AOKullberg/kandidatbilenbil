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
int executing_node = 0;
int node_finished = 0;

//0=prog, 1=manual, 2=autonom
int mode=2;

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
	
	
	
/*
	else if (steering_decision==0xff)
	{
		transmit_spi(0xc0);
	}
	else if (steering_decision==0xd0)
	{
		transmit_spi(0xd0);
	}
	else if(steering_decision==0xe0)
	{
		transmit_spi(0xe0);
	}
	else if(steering_decision==0x81)
	{
		transmit_spi(0x81);
	}
	else
	{
		transmit_spi(0xc0);
	}*/
	
//position+=velocity*0.01;//Todo bestämma vad detta värde ska vara för att ge rätt position

//Skicka koden för autonomous driving

}

/*Main-loop vid programmeringsmode
*/
void prog_main(void)
{
	clear_maps();
	get_no_nodes();
	transmit_uart0(no_nodes);
	init_maps();
	update_distancemap();
	check_map(distancemap);
	//update_roadmap();
	//check_map(roadmap);
	get_destination();
	new_route(destination);
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
	transmit_spi(0xc0);
	
    while (1) 
    {

		send_data();
		_delay_ms(10);


		switch(mode)
		{
			case 0 :
			prog_main();
			break;
			
			case 1 :
			manual_main();
			break;
			
			case 2 :
			autonom_main();
			break;
		}
    }
	
}

