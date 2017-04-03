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
#include <compat/twi.h>

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
//senast mottagen data via I2C
volatile unsigned char i2c_indata = 0x00;
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

//Avbrottsrutin då ny data mottagits via I2C
/*ISR(TWI_vect)
{
	i2c_indata=TWDR;
}*/

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

/*
struct IMURawData_S
{
	uint16_t AngularVelocityZ;
};
*/
#define AngularVelocitySlaveAddress 0xD6
#define AngularVelocity_WHO_AM_I_Register 0x0F  // Device identification register
#define AngularVelocity_WHO_AM_I_RegisterDefault 0xD7 //D4?
#define AngularVelocity_CTRL1_Register 0x20     // Enable axis, bandwidth, and output data rate
#define AngularVelocity_OUT_Z_L_Register 0x2C   // Z axes AngularVelocity Low bit
#define AngularVelocity_OUT_Z_H_Register 0x2D   // Z axes AngularVelocity High bit

#define SCL_CLOCK 100000L

void I2C_INT();
uint8_t SetUpIMU();
uint8_t SendI2CData(uint8_t SlaveAddress, uint8_t Register, uint8_t Data);
uint8_t GetI2CData(uint8_t SlaveAddress, uint8_t Register, uint8_t *Data);

/// Sets up the I2C ///
void I2C_INT()
{	
	TWBR = ((F_CPU/SCL_CLOCK)-16)/2;                // [Bit rate register] controls signal speed p.232
	TWCR = (1<<TWEN);								//     // [Control register] p.232
	TWSR = 0;									//&= ~((1<<TWPS1) | (1<<TWPS0));             // [Status register 0-1] controls signal speed p.234
	return;
}

uint8_t SendI2CData(uint8_t SlaveAddress, uint8_t Register, uint8_t Data)
{
	/// Set start ///
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));   // wait
	if ((TWSR & 0xF8) != 0x08)
	return -1;

	/// Send slave address + w ///
	TWDR = SlaveAddress;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	if ((TWSR & 0xF8) != 0x18)
	return -2;
	
	/// Send sub address ///
	TWDR = Register;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	if ((TWSR & 0xF8) != 0x28)
	return -3;
	
	/// Send  Data ///
	TWDR = Data;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	if ((TWSR & 0xF8) != 0x28)
	return -4;
	
	/// Stop bit ///
	TWCR = (1<<TWINT) | (1<<TWSTO);
	
	return 0;
}

uint8_t GetI2CData(uint8_t SlaveAddress, uint8_t Register,uint8_t *Data)
{
	/// Set start ///
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);	
	while (!(TWCR & (1<<TWINT)));   // wait
	if ((TWSR & 0xF8) != 0x08)
	return -1;

	/// Send slave address + w ///
	TWDR = SlaveAddress;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	if ((TWSR & 0xF8) != 0x18)
	//    if ((TWSR & 0xF8) != 0x48)
	return -2;

	/// Send  sub address ///
	TWDR = Register;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	if ((TWSR & 0xF8) != 0x28)
	return -3;
	
	/// Set repeat start ///
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));   // wait
	if ((TWSR & 0xF8) != 0x10)
	return -4;
	
	/// Send slave address + r ///
	TWDR = SlaveAddress | 0x01;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));   // wait
	if ((TWSR & 0xF8) != 0x40)
	return -5;
	
	/// Get data ///
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));   // wait
	
	/// Stop bit ///
	TWCR = (1<<TWINT) | (1<<TWSTO);
	
	*Data = TWDR;
	
	return 0;
}

uint8_t SetUpIMU()
{
	uint8_t RegisterData;
	
	I2C_INT();
	_delay_ms(10);
	
	/// Set up the AngularVelocity /// WIP
	if( GetI2CData(AngularVelocitySlaveAddress, AngularVelocity_WHO_AM_I_Register,  &RegisterData) )
	return -1;
	if( RegisterData != AngularVelocity_WHO_AM_I_RegisterDefault)
	return -1;
	
	if( SendI2CData(AngularVelocitySlaveAddress, AngularVelocity_CTRL1_Register, 0x0F) )   /// Enable stuff WIP (fast with high cut off freq?)
	return -1;

	return 0;
}

//uint16_t AngularVelocityZ;

/*
uint8_t IMUGAngularVelocity_H()
{	
	if ( GetI2CData(AngularVelocitySlaveAddress, AngularVelocity_OUT_Z_H_Register) )
	return -1;
	//if ( GetI2CData(AngularVelocitySlaveAddress, AngularVelocity_OUT_Z_H_Register,  (uint8_t*)&(AngularVelocityZ) + 1  ) )
	//return -1;
	
	return 0;
}

uint8_t IMUGAngularVelocity_L()
{
	if ( GetI2CData(AngularVelocitySlaveAddress, AngularVelocity_OUT_Z_L_Register ) )
	return -1;
	//if ( GetI2CData(AngularVelocitySlaveAddress, AngularVelocity_OUT_Z_H_Register,  (uint8_t*)&(AngularVelocityZ) + 1  ) )
	//return -1;
	
	return 0;
}

*/



int main(void)
{
	sei();
	uart_init();
	spi_init();
	pwm_init();
	uint8_t HighData;
	uint8_t LowData;

	SetUpIMU();
	
	while (1)
	{
	//execute_command(spi_indata);	
	//OCR1A=steering_degree;
	//OCR1B=motor_speed;
		
	GetI2CData(AngularVelocitySlaveAddress, AngularVelocity_OUT_Z_H_Register, &HighData);
	GetI2CData(AngularVelocitySlaveAddress, AngularVelocity_OUT_Z_L_Register, &LowData);
	
	transmit_spi(HighData);
	transmit_spi(LowData);
	_delay_ms(200);
	}
	

		
	
}

