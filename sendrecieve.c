/*
 * CFile1.c
 *
 * Created: 2017-04-27 10:02:21
 *  Author: Tobias Fridén
 */ 

#include "sendrecieve.h"
#include "main.h"
#include "led_kom.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/* Globala variabler
*/

int change_mode = 0;

//För att hämta indata från datorn
volatile unsigned char computer_indata=0x00;

//Sensordata
volatile unsigned char velocity = 0x00;
volatile unsigned char distance_front = 0x00;
volatile unsigned char distance_back = 0x00;
volatile unsigned char distance_right = 0x00;
volatile unsigned char distance_left = 0x00;
volatile unsigned char camera_right = 0x00;
volatile unsigned char camera_left = 0x00;
unsigned char ack = 0x61;
int counter = 0;

//Styrbeslut
volatile unsigned char steering_decision=0x01;
volatile unsigned char steering_command=0x00;

//Position
volatile unsigned char position=0x00;

/*FUNCTION uart_init()
* sätter de register som krävs för att kunna använda
* uart0 och uart1
*/
void uart_init(void)
{
	//Aktivera UART0
	UBRR0H = (BAUDRATE>>8);
	UBRR0L = BAUDRATE;
	//Aktivera reciever och transmitter
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
	// Sätter frame format: 8data, 2stop bit
	UCSR0C = (1<<USBS1)|(3<<UCSZ10);
	
	//Aktivera UART1
	UBRR1H = (BAUDRATE>>8);
	UBRR1L = BAUDRATE;
	//Aktivera reciever och transmitter
	UCSR1B = (1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1);
	// Sätter frame format: 8data, 2stop bit
	UCSR1C = (1<<USBS1)|(3<<UCSZ10);
}

/* FUNCTION spi_init()
* sätter de register som krävs för att kunna använda SPI
*/
void spi_init(void)
{
	//Sätter MOSI och SCK till outputs
	DDRB = (1<<5)|(1<<7)|(1<<4);
	// Enable SPI, Master, set clock rate fck/16
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR1)|(1<<SPR0)/*|(1<<SPIE)*/;
}

/* FUNCTION transmit_uart0(unsigned char data)
* skickar data över uart0-bussen
*/
void transmit_uart0(unsigned char data)
{
	//Väntar på att utdatabuffer ska bli tom
	while ( !( UCSR0A & (1<<UDRE1)) );
	//Skickar data
	UDR0 = data;
}

/* FUNCTION transmit_uart1(unsigned char data)
* skickar data över uart1-bussen
*/
void transmit_uart1(unsigned char data)
{
	//Väntar på att utdatabuffer ska bli tom
	while ( !( UCSR1A & (1<<UDRE1)) );
	//Skickar data
	UDR1 = data;
}

/* FUNCTION transmit_spi(unsigned char data)
* skickar data över SPI-bussen
*/
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

/* FUNCTION send_data(void)
* skickar all data som skall skickas till datorn
*/
void send_data(void)
{
	transmit_uart0(velocity);
	transmit_uart0(distance_front);
	transmit_uart0(distance_back);
	transmit_uart0(distance_right);
	transmit_uart0(distance_left);
	transmit_uart0(camera_right);
	transmit_uart0(camera_left);
	transmit_uart0(steering_decision);
	transmit_uart0(position);
	transmit_uart0(ack);
}

/* FUNCTION get_sensor_data(unsigned char data)
* sparar mottagen sensordata i korrekt global variabel med hjälp av
* counter, samt kontrollerar att ack-biten skickas. Då denna mottagits 
* blinkas en led på bilen för att signalera att sensordatan uppdaterats
*/
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
		if (data == ack)
		{
			//blink_led(2,10);
			counter=0;
		}
		else
		{
			//blink_led(1,10);
		}
		break;
	}
}

/* Interrupt Service Routine
* avbrottsrutin då ny data mottagits via SPI
*/
ISR(SPI_STC_vect)
{
	//blink_led(4,10);
	steering_decision=SPDR;
}

/* Interrupt Service Routine
* avbrottsrutin då ny data mottagits via uart0
*/
ISR(USART0_RX_vect)
{
	computer_indata=UDR0;
	if (computer_indata == 0xFF)
	{
		change_mode = 1;
	}
	
/*
	if (uart0_indata == 0x77)
	{
		transmit_spi(0x02);
	}
	else if (uart0_indata == 0x73 )
	{
		transmit_spi(0x04);
	}
	else if (uart0_indata == 0x61)
	{
		transmit_spi(0x08);
	}
	else if (uart0_indata == 0x64)
	{
		transmit_spi(0x10);
	}
*/
}

/*Interrupt Service Routine
* avbrottsrutin då ny data mottagits via uart1
*/
ISR(USART1_RX_vect)
{
	get_sensor_data(UDR1);
}
