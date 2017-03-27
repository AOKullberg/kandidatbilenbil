/*
 * STYR.c
 *
 * Created: 2017-03-27 12:55:17
 * Author : Tobias Fridén
 */ 

#define F_CPU 14745600UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define BAUD 115200		//Sätter baudrate (bps)
#define BAUDRATE ((F_CPU)/(BAUD*16UL)-1)

//Macro för att undersöka en specifik bit
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))


//Initialisera PWM-signaler
void pwm_init(void)
{
	DDRD|=(1<<PD7)|(1<<PD5)|(1<<PD4); //Makes PORTD as Output
	TCCR1A|=(1<<WGM11)|(1<<COM1A1)|(1<<COM1B1);
	TCCR1B|=(1<<WGM12)|(1<<WGM13)|(1<<CS11)|(1<<CS10);
	ICR1 = 4607;
	OCR1A = 320;
	OCR1B = 340;
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

//Skicka data över uart1
void transmit_uart1(unsigned char data)
{
	//Väntar på att utdatabuffer ska bli tom
	while ( !( UCSR1A & (1<<UDRE1)) );
	//Skickar data
	UDR1 = data;
}

//Skicka data över SPI
void transmit_spi(unsigned char data)
{
	//Skicka data
	SPDR = data;
}

//Senast mottagen data via uart1
volatile unsigned char uart1_indata = 0x00;
//Senast mottagen data via SPI
volatile unsigned char spi_indata = 0x00;
//Räknare för pwm-signal till styrservo
volatile uint16_t steering_degree = 320;
//Räknare för pwm-signal till motorservo
volatile uint16_t motor_speed = 340;
//Senast utfört kommando
volatile unsigned char executed_command = 0x00;


//Avbrottsrutin då ny data mottagits via SPI
ISR(SPI_STC_vect)
{
	spi_indata=SPDR;
}

//Avbrottsrutin då ny data mottagits via UART
ISR(USART1_RX_vect)
{
	uart1_indata=UDR1;
}

//Svänger hjulen vänster
void turn_left(void)
{
	if (steering_degree < 400 )
	{
				steering_degree += 4;
	}		
}

//Svänger hjulen höger
void turn_right(void)
{
	if (steering_degree > 250 )
	{
		steering_degree -= 4;
	}	
}

//Accelererar bilen
void accelerate(void)
{
	if (motor_speed < 340 )
	{
		motor_speed = 340;
	}
	else if (motor_speed < 350 )
	{
		motor_speed += 1;
	}	
}

//Sänker farten/backar
void retardate(void)
{
	if (motor_speed > 340 )
	{
		motor_speed = 340;
	}
	else if (motor_speed > 327 )
	{
		motor_speed -= 1;
	}
}

//Utför manuellt kommando
void manual_command(unsigned char newcommand)
{
	if (CHECK_BIT(newcommand,1))
	{
		accelerate();
		executed_command |= (1<<1);
	}
	if (CHECK_BIT(newcommand,2))
	{
		retardate();
		executed_command |= (1<<2);
	}
	if (CHECK_BIT(newcommand,3))
	{
		turn_left();
		executed_command |= (1<<3);
	}
	if (CHECK_BIT(newcommand,4))
	{
		turn_right();
		executed_command |= (1<<4);
	}
}

//Hämtar vilket kommando som skall utföras givet insignal
void execute_command(unsigned char newcommand)
{
	executed_command = 0x00;
	if(CHECK_BIT(newcommand,0))
	{
		//Autonom körning implementeras senare
	}
	else
	{
		manual_command(newcommand);
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
	execute_command(spi_indata);	
	OCR1A=steering_degree;
	OCR1B=motor_speed;
	transmit_spi(executed_command);
	_delay_ms(100);
	}
	
}



	