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

//Avbrottsrutin d� ny data mottagits via SPI
ISR(SPI_STC_vect)
{
	spi_indata=SPDR;
	transmit_spi(spi_indata);
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
		distance_front=data;
		++counter;
		break;
		
		case 2 :
		distance_back=data;
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
		camera_right = data;
		++counter;
		break;
		
		case 6 :
		camera_left = data;
		++counter;
		break;
		
		case 7 :
		camera_front = data;
		++counter;
		break;
		
		case 8 :
		if (data == ack)
		{
			_delay_ms(20);
			blink_led(6);
			counter=0;
		}
		else
		{
			blink_led(7);
			counter = 0;
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
	if ((newcommand & 0x80) == 0x80)		//Utfart g�rdsplan rakt
	{

	}
	if ((newcommand & 0x90) == 0x90)		//utfart gardsplan svang
	{
		
	}
	if ((newcommand & 0xa0) == 0xa0)		//infart gardsplan rakt
	{
		
	}
	if ((newcommand & 0xb0) == 0xb0)		//Infart g�rdsplan svang
	{

	}
	if ((newcommand & 0xc0) == 0xc0)		//V�gf�ljning
	{
		//blink_led(6);
		if(camera_front > 20)
		{
			autonomous_driving();
		}
		else
		{
			brake();
			//satta flagga
		}
	}
	if ((newcommand & 0xd0) == 0xd0)	//korsning vanster
	{

	}
	if ((newcommand & 0xe0) == 0xe0)	//korsning hoger
	{

	}
	if ((newcommand & 0xf0) == 0xf0)	//korsning rakt
	{

	}
	if ((newcommand & 0x82) == 0x82)	//parkering 1
	{
		
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


		/*DDRB = 0<<PD3;
		EIMSK = 1 <<INT2;*/
		//_delay_ms(5000);
		
		/*while (1)
		{
			OCR2A=22;
			_delay_ms(1000);
			OCR2A=16;
			_delay_ms(1000);
		}
		*/
		OCR1B = 345; 
		_delay_ms(3000);
		while(distance_front > 15)
		{
			autonomous_driving();
		}
		drive_to_stopline();
		//turn_90_degrees('F', 'R');
		//turn_90_degrees('F', 'L');
		//turn_90_degrees('F','R');
		/*if (test_flag == 1)
		{*/
			/*	if (spi_indata == 0x0a)
				{
					//blink_led(6);
					if(camera_front > 20)
					{
						autonomous_driving();
					}
					else
					{
						brake();
						//satta flagga
					}
				}
				else
				{
					brake();
					blink_led(6);
				}*/

		//}
			//pd_steering_control(desired_distance_right, distance_front, prior_error_right, 'R');		
			/*if (distance_front > 50)
			{
				accelerate(1);
				if (camera_left < 60)
				{
					pd_steering_control(desired_distance_left, camera_left, prior_error_left, 'L');
				}
				else if (camera_right < 60)
				{
					
					pd_steering_control(desired_distance_right, camera_right, prior_error_right, 'R');
				}
	
				_delay_ms(100);
			}
			else
			{
				brake();
				_delay_ms(100);
			}*/
			
		
		

}