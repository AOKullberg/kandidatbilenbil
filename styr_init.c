/*
 * styr_init.c
 *
 * Created: 4/6/2017 1:37:57 PM
 *  Author: kargu357
 */

//Initialisera PWM-signaler
#include "main.h"
#include "styr_init.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void pwm_init(void)
{
	DDRD|=(1<<PD7)|(1<<PD5)|(1<<PD4); //Makes PORTD as Output
	TCCR1A|=(1<<WGM11)|(1<<COM1A1)|(1<<COM1B1);
	TCCR1B|=(1<<WGM12)|(1<<WGM13)|(1<<CS11)|(1<<CS10);
	TCCR2A|=(1<<WGM21)|(1<<COM2A1)|(1<<WGM20);
	TCCR2B|=(1<<CS22)|(1<<CS21)|(1<<CS20);
	TCCR0B |= (1 << CS00);	//initierar timer0 utan prescale. 8 bitars timer. anvands istallet 16 bitar blir maxvarde ca 4,4 ms --> behovs avbrott
	TIMSK0 |= (1 << TOIE0); //enablar avbrott vid overflow av timer0
	ICR1 = 4607;
	OCR1A = 320;
	OCR1B = 340;
	OCR2A = 19;
}

//Initialisera UART
void uart_init(void)
{
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
	//Sätter MISO till output
	DDRB = (1<<6);
	/* Enable SPI, Slave, set clock rate fck/16 */
	SPCR = (1<<SPE)|(1<<SPIE);
}
