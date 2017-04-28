/*
 * led_kom.c
 *
 * Created: 4/28/2017 9:13:36 AM
 *  Author: tobfr427
 */ 


#include "led_kom.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void delay( int mysec )
{
	for(int i=0;i<mysec;i++)
	{
		_delay_ms(1);
	}
}

void led_init()
{
	DDRC |= (1<<0)|(1<<1)|(1<<6)|(1<<7);
}

void blink_led(int nr, int mysec)
{
	switch(nr)
	{
		case 1 :
		PORTC = (1<<0);
		delay(mysec);
		PORTC = (0<<0);
		delay(mysec);
		break;
		
		case 2 :
		PORTC = (1<<1);
		delay(mysec);
		PORTC = (0<<1);
		delay(mysec);
		break;
		
		case 3 :
		PORTC = (1<<6);
		delay(mysec);
		PORTC = (0<<6);
		delay(mysec);
		break;
		
		case 4 :
		PORTC = (1<<7);
		delay(mysec);
		PORTC = (0<<7);
		delay(mysec);
		break;
	}
}