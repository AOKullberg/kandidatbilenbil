/*
 * main_styr.c
 *
 * Created: 4/6/2017 9:17:15 AM
 *  Author: kargu357
 */

#include "styr_init.h"
#include "motorstyrning.h"
#include "main_styr.h"
#include "autonom_drive.h"
#include "manuell_styrning.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//Senast mottagen data via uart1
volatile unsigned char uart1_indata = 0x00;
//Senast mottagen data via SPI
volatile unsigned char spi_indata = 0x00;
//Senast utf�rt kommando
volatile unsigned char executed_command = 0x00;
//r�knare f�r data fr�n sensorenhet
int counter = 0;

///Sensordata///
//Avst�nd fram�t
volatile unsigned char distance_forward;
//Avst�nd bak�t
volatile unsigned char distance_backwards;
//Avst�nd h�ger
volatile unsigned char distance_right;
//Avst�nd v�nster
volatile unsigned char distance_left;
//Hastighet
volatile unsigned char velocity;


void blink_led( int nr)
{
	switch (nr)
	{
		case 1 :
		DDRD = 1<<PD0;
		PORTD = 1<<PD0;
		_delay_ms(500);
		PORTD = 0<<PD0;

		case 2 :
		DDRD = 1<<PD1;
		PORTD = 1<<PD1;
		_delay_ms(500);
		PORTD = 0<<PD1;

		case 3 :
		DDRC = 1<<PC6;
		PORTC = 1<<PC6;
		_delay_ms(500);
		PORTC = 0<<PC6;

		case 4 :
		DDRC = 1<<PC7;
		PORTC = 1<<PC7;
		_delay_ms(500);
		PORTC = 0<<PC7;
	}
}

//Skicka data �ver uart1
void transmit_uart1(unsigned char data)
{
	//V�ntar p� att utdatabuffer ska bli tom
	while ( !( UCSR1A & (1<<UDRE1)) );
	//Skickar data
	UDR1 = data;
}

//Skicka data �ver SPI
void transmit_spi(unsigned char data)
{
	//Skicka data
	SPDR = data;
}

//Avbrottsrutin d� ny data mottagits via SPI
ISR(SPI_STC_vect)
{
	spi_indata=SPDR;
}

//Avbrottsrutin d� ny data mottagits via UART
ISR(USART1_RX_vect)
{
	get_sensor_data(UDR1);
}

void get_sensor_data(unsigned char data)
{
	switch(counter)
	{
		case 0 :
		velocity=data;
		++counter;
		break;

		case 1 :
		distance_forward=data;
		++counter;
		break;

		case 2 :
		distance_backwards=data;
		++counter;
		break;

		case 3 :
		distance_right=data;
		++counter;
		break;

		case 4 :
		distance_left = data;
		++counter;
		break;

		case 5 :
		if (data == ack)
		{
			blink_led(4);
			counter=0;
		}
		else
		{
			blink_led(1);
		}
		break;
	}
}



//H�mtar vilket kommando som skall utf�ras givet insignal
void execute_command(unsigned char newcommand)
{
	executed_command = 0x00;
	if(CHECK_BIT(newcommand,0))
	{
		autonomous_command(newcommand);
	}
	else
	{
		manual_command(newcommand);
	}
}

void autonomous_command(unsigned char newcommand)
{
	if ((newcommand & 0x80) == 0x80)	//Utfart g�rdsplan
	{

	}
	if ((newcommand & 0x90) == 0x90)	//Infart g�rdsplan
	{

	}
	if ((newcommand & 0xa0) == 0xa0)	//V�gf�ljning
	{
		autonomous_driving();
	}
	if ((newcommand & 0xb0) == 0xb0)	//korsning
	{

	}
	if ((newcommand & 0xc0) == 0xc0)	//parkeringsficka
	{

	}
	if ((newcommand & 0x0d) == 0xd0)	//parkering
	{

	}
}

int main(void)
{
	sei();
	uart_init();
	spi_init();
	pwm_init();


	while (1)
	{
		cli();
		execute_command(spi_indata);
		sei();
		OCR1A=steering_degree;
		OCR1B=motor_speed;
		transmit_spi(executed_command);
		_delay_ms(100);
	}

}
