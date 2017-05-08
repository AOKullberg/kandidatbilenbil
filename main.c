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
int mode=2;

/* Main-loop vid manuell körning
*/
void manual_main(void)
{
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
	if (change_mode)
	{
		mode=1;
		change_mode=0;
	}
	else if ( (steering_decision == 0xf0)  ) //Om bilen stannat, vad innebär det för steering_decision?
	{
		blink_led(1,10);
		//_delay_ms(1000);
		if (distance_front > 15)
		{
			steering_command = 0xd0;
		}
		
		/*while (steering_decision!=0xff);
		{
			blink_led(2,10);
			transmit_spi(0xd0);			
		}
		transmit_spi(0xc0);*/
		/*if (100*position/dist_to_next > 95 ) //Kolla hur långt bilen kommit på vägsträckan, om den är i slutet har den nått en korsning
		{
			execute_node_end();
		}
		else
		{
			//Todo svänga förbi hinder?
		}*/
	}
	else if (steering_decision==0xff)
	{
		steering_command=0xc0;
	}
	else
	{
		steering_command=0xc0;
	}
	
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
	blink_led(2,10);
	_delay_ms(1000);
	
    while (1) 
    {

		//send_data();
		_delay_ms(1);
		steering_decision = spi_tranciever(steering_command);
		transmit_uart0(steering_decision);
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

