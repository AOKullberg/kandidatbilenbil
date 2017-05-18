/*
 * main_styr.c
 *
 * Created: 4/6/2017 9:17:15 AM
 *  Author: kargu357
 */

#include "styr_init.h"
#include "motorstyrning.h"
#include "main.h"
#include "autonom_drive.h"
#include "manuell_styrning.h"
#include "styralgoritm.h"
#include "gardsplan.h"
#include "gyro_2.h"
#include "parkeringsficka.h"
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

unsigned char test_flag;

///Sensordata///
volatile unsigned char velocity = 0x00;
volatile unsigned char distance_front = 0x00;
volatile unsigned char distance_back = 0x00;
volatile unsigned char distance_right = 0x00;
volatile unsigned char distance_left = 0x00;
volatile unsigned char camera_right = 0x00;
volatile unsigned char camera_left = 0x00;
volatile unsigned char camera_front = 0x00;
unsigned char ack = 0x61;
unsigned char distance_to_stopline=0;
/*
void blink_led( int nr)
{
	switch (nr)
	{
		case 1 :
		DDRD |= 1<<PD0;
		PORTD |= 1<<PD0;
		_delay_ms(500);
		PORTD |= 0<<PD0;

		case 2 :
		DDRD |= 1<<PD1;
		PORTD |= 1<<PD1;
		_delay_ms(500);
		PORTD |= 0<<PD1;

		case 3 :
		DDRC |= 1<<PC6;
		PORTC |= 1<<PC6;
		_delay_ms(500);
		PORTC |= 0<<PC6;

		case 4 :
		DDRC |= 1<<PC7;
		PORTC |= 1<<PC7;
		_delay_ms(500);
		PORTC |= 0<<PC7;
	}
}*/

void blink_led(int nr)
{
	DDRC=(1<<nr);
	PORTC=(1<<nr);
	_delay_ms(10);
	PORTC=(0<<nr);
	_delay_ms(10);
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

unsigned char spi_tranciever(unsigned char data)
{
	SPDR = data;
	while(!(SPSR & (1 << SPIF)));
	return (SPDR);
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
		motor_speed=OCR1B;
		++counter;
		break;
		
		case 1 :
		distance_front=data;
		++counter;
		break;
		
		case 2 :
		prior_right=camera_right;
		camera_right = data;
		++counter;
		break;
		
		case 3 :
		prior_left=camera_left;
		camera_left = data;
		++counter;
		break;
		
		case 4 :
		camera_front = data;
		++counter;
		break;
		
		case 5 :
		if (data == ack)
		{
			//blink_led(6);
			_delay_ms(20);
			counter=0;
		}
		else
		{
			//blink_led(7);
			counter = 0;
		}
		break;
	}
}






void execute_command(unsigned char newcommand)
{

	if (newcommand == 0xc0)		//V�gf�ljning
	{
		if(((distance_front > 15) || (distance_front == 1))&&(camera_front==0))
		{
			autonomous_driving();
			transmit_spi(0xc0);
		}
		else if ((distance_front < 15)&& (distance_front!=1))
		{
			brake();
			transmit_spi(0xdd);
		}
		else
		{
			drive_to_stopline();			
		}

		}
	
	if (newcommand == 0xd0)	//korsning vanster
	{
		transmit_spi(0xd0);
		crossroad_left();
		while (spi_indata!=0xc0)
		{
			transmit_spi(0xff);
			_delay_ms(10);
		}

	}
	if (newcommand == 0xe0)	//korsning hoger
	{
		transmit_spi(0xe0);
		crossroad_right();
		while (spi_indata!=0xc0)
		{
			transmit_spi(0xff);
			_delay_ms(10);
		}
		

	}
	if (newcommand == 0x81)	//korsning rakt
	{
		transmit_spi(0x81);
		crossroad_forward();
		while (spi_indata!=0xc0)
		{
			transmit_spi(0xff);
			_delay_ms(10);
		}

	}
	if (newcommand == 0x82)	//stopplats
	{
		transmit_spi(0x82);
		stopline();
		while (spi_indata!=0xc0)
		{
			transmit_spi(0xff);
			_delay_ms(10);
		}
		
	}	
	if (newcommand == 0x83)	//parkeringsficka
	{
		transmit_spi(0x83);
		parkeringsficka();
		while (spi_indata!=0xc0)
		{
			transmit_spi(0xff);
			_delay_ms(10);
		}
		
	}
	if ((newcommand & 0x92) == 0x92)	//parkering 2
	{
		
	}
	if ((newcommand & 0xa2) == 0xa2)	//parkering 3
	{
		
	}
	if ((newcommand & 0xb2) == 0xb2)	//parkeringsficka
	{
		
	}
	if (newcommand == 0xaa)
	{
		brake();
		blink_led(6);
/*
		while (newcommand!=0x00)
		{
			transmit_spi(0xff);
		}*/

	}
	if (newcommand < 0x1f)
	{
		manual_command(newcommand);
	}
}

//används för att testa motorstyrningen
ISR(INT2_vect)
{
	test_flag = 1;
}




int main(void)
{
		sei();
		uart_init();
		spi_init();
		pwm_init();
		
		SetUpIMU();
		_delay_ms(5000);
	

		while (1)
		{
			_delay_ms(10);
			execute_command(spi_indata);
		}

}