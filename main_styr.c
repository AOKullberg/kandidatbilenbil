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
#include <string.h>

#define BAUD 115200		//Sätter baudrate (bps)
#define BAUDRATE ((F_CPU)/(BAUD*16UL)-1)

//Macro för att undersöka en specifik bit
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

#define Kp 1	//definiera Kp och Kd
#define Kd 1

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

int klocka = 0;

//Avbrottsrutin då ny data mottagits via SPI
ISR(SPI_STC_vect)
{
	spi_indata=SPDR;
}

//Avbrottsrutin då ny data mottagits via UART
ISR(USART1_RX_vect)
{
	uart1_indata=UDR1;
	if (klocka == 0)
	{
		check_distance_forward(uart1_indata);
	}
	if (klocka == 1)
	{
		check_distance_behind(uart1_indata);
	}
	if (klocka == 3)
	{
		cli();
		pd_steering_control(desired_distance_right, uart1_indata, prior_error_right, right);
		sei();
	}
	if (klocka == 4)
	{
		cli();
		pd_steering_control(desired_distance_left, uart1_indata, prior_error_left, left);
	}
}

//Svänger hjulen vänster
void turn_left(uint8_t data=0x04)
{
	if (steering_degree < 400 )
	{
				steering_degree += data;
	}
}

//Svänger hjulen höger
void turn_right(uint8_t data=0x04)
{
	if (steering_degree > 250 )
	{
		steering_degree -= data;
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
		autnomous_driving();
	}
	else
	{
		manual_command(newcommand);
	}
}

volatile uint8_t prior_error_right = 0x00;
volatile uint8_t prior_error_left = 0x00;
uint8_t desired_distance_right = 32;	//millimeter
uint8_t desired_distance_left = 32;	//millimeter

void check_sensorvalues(unsigned char sensorvalues){}

uint8_t calculate_error(uint8_t desired_distance, uint8_t actual_distance)
{
	uint8_t error = desired_distance - actual_distance;
	return error;
}

uint8_t derivative(uint8_t error, uint8_t prior_error)
{
	uint8_t derivative = (error - prior_error)/iteration_time;
	return derivative;
}

void pd_steering_control(uint8_t desired_distance, uint8_t actual_distance, uint8_t prior_error, string direction)
{
	uint8_t error = calculate_error(desired_distance, actual_distance);
	uint8_t derivative = derivative(error, prior_error);
	if(error > 0)
	{
		if(direction == right)
		{
			turn_left(error*Kp+Kd*derivative);
		}
		else if(direction == left)
		{
			turn_right(error*Kp+Kd*derivative);
		}
	}
	prior_error = error;
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
