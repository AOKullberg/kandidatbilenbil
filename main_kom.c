/*
 * KOM.c
 *
 * Created: 2017-03-26 16:50:59
 * Author : Tobias Fridén
 */ 

#define F_CPU 14745600UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define BAUD 115200		//Sätter baudrate (bps)
#define BAUDRATE ((F_CPU)/(BAUD*16UL)-1)  




//Initialisera UART
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

//Initialisera SPI
void spi_init(void)
{
	//Sätter MOSI och SCK till outputs
	DDRB = (1<<5)|(1<<7)|(1<<4);
	// Enable SPI, Master, set clock rate fck/16 
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR1)|(1<<SPR0)|(1<<SPIE);
}

void transmit_uart0(unsigned char data)
{
	//Väntar på att utdatabuffer ska bli tom
	while ( !( UCSR0A & (1<<UDRE1)) );
	//Skickar data
	UDR0 = data;
}

void transmit_uart1(unsigned char data)
{
	//Väntar på att utdatabuffer ska bli tom
	while ( !( UCSR1A & (1<<UDRE1)) );
	//Skickar data
	UDR1 = data;
}

void transmit_spi(unsigned char data)
{
	//Skicka data
	SPDR = data;
}


volatile unsigned char uart0_indata = 0x00;
volatile unsigned char uart1_indata = 0x00;
volatile unsigned char spi_indata = 0x00;

	

ISR(SPI_STC_vect)
{
	spi_indata=SPDR;
}

ISR(USART0_RX_vect)
{
	uart0_indata=UDR0;
}

ISR(USART1_RX_vect)
{
	uart1_indata=UDR1;
}



int main(void)
{
	sei();
	uart_init();
	spi_init();
	
    while (1) 
    {
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
		transmit_spi(0x00);
		transmit_uart0(spi_indata);
		_delay_ms(200);
    }
}
