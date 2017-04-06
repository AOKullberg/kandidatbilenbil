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
volatile unsigned char speed;

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
	uart1_indata=UDR1;
	if (counter == 0)
	{
		distance_forward = uart1_indata;
	}
	if (counter == 1)
	{
		distance_backwards = uart1_indata;
	}
	if (counter == 2)
	{
		distance_right = uart1_indata;
	}
	if (counter == 3)
	{
		distance_left = uart1_indata;
	}
	if (counter == 4)
	{
		speed = uart1_indata;
		counter = -1;
	}
	
	counter +=  1;
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