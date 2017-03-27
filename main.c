/*
 * KOM.c
 *
 * Created: 2017-03-26 16:50:59
 * Author : Tobias Fridén
 */ 



#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define BAUD 9600		//Sätter baudrate (bps)
#define BAUDRATE ((F_CPU)/(BAUD*16UL)-1)  

#ifndef F_CPU
#define F_CPU 14745600UL
#endif

//Initialisera UART
void uart_init(void)
{
	UBRR1H = (BAUDRATE>>8);
	UBRR1L = BAUDRATE;
	//Aktivera reciever och transmitter
	UCSR1B = (1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1);
	// Sätter frame format: 8data, 2stop bit 
	UCSR1C = (1<<USBS1)|(3<<UCSZ10);	
}

//Initialisera SPI
void spi_init(void)
{
	//Sätter MOSI och SCK till outputs
	DDRB = (1<<5)|(1<<7);
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0)|(1<<SPIE);
}

void transmit_uart(unsigned char data)
{
	//Väntar på att utdatabuffer ska bli tom
	while ( !( UCSR1A & (1<<UDRE1)) );
	//Skickar data
	UDR1 = data;
}

void transmit_spi(char data)
{
	//Skicka data
	SPDR = data;
	//Vänta tills överföringen är klar
	while(!(SPSR & (1<<SPIF)));
}


struct indata_buffer{
		unsigned char uart_indata;
		unsigned char spi_indata;
	};


int main(void)
{
	
    /* Replace with your application code */
    while (1) 
    {
    }
}

ISR(SPI_STC_vect)
{
	spi_indata=SPDR;
}