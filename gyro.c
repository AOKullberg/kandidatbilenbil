/*
 * GYRO_170404.c
 *
 * Created: 4/4/2017 10:15:45 AM
 *  Author: kargu357
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


#define AngularVelocitySlaveAddress 0xD6	//address to gyro
#define AngularVelocity_WHO_AM_I_Register 0x0F  // Identifikationsregister för gyrot
#define AngularVelocity_WHO_AM_I_RegisterDefault 0xD7 //defaultvärde
#define AngularVelocity_CTRL1_Register 0x20     // Enabla axlar, bandbredd och ouput data rate
#define AngularVelocity_OUT_Z_L_Register 0x2C   // Z-axel Gyro Låg byte
#define AngularVelocity_OUT_Z_H_Register 0x2D   // Z-axel Gyro Hög byte

#define SCL_CLOCK 100000L	//Intern klocka gyro
#define G_GAIN 0.00875		//Noggrannhetsmått

void I2C_INT();
uint8_t SetUpIMU();
uint8_t SendI2CData(uint8_t SlaveAddress, uint8_t Register, uint8_t Data);
uint8_t GetI2CData(uint8_t SlaveAddress, uint8_t Register, uint8_t *Data);
float ConvertToAngles(uint16_t Data);

/// Initierar I2C ///
void I2C_INT()
{	
	TWBR = ((F_CPU/SCL_CLOCK)-16)/2;                
	TWCR = (1<<TWEN);								
	TWSR = 0;
	return;
}

uint8_t SendI2CData(uint8_t SlaveAddress, uint8_t Register, uint8_t Data)
{
	/// Set start ///
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));   // vänta
	if ((TWSR & 0xF8) != 0x08)
	return -1;

	/// Skicka slave address + write ///
	TWDR = SlaveAddress;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	if ((TWSR & 0xF8) != 0x18)
	return -2;
	
	/// Skicka sub address ///
	TWDR = Register;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	if ((TWSR & 0xF8) != 0x28)
	return -3;
	
	/// Skicka Data ///
	TWDR = Data;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	if ((TWSR & 0xF8) != 0x28)
	return -4;
	
	/// Stoppbit ///
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

	/// Skicka slave address + write ///
	TWDR = SlaveAddress;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	if ((TWSR & 0xF8) != 0x18)
	//    if ((TWSR & 0xF8) != 0x48)
	return -2;

	/// Skicka sub address ///
	TWDR = Register;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	if ((TWSR & 0xF8) != 0x28)
	return -3;
	
	/// Skicka repeated start ///
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));   // wait
	if ((TWSR & 0xF8) != 0x10)
	return -4;
	
	/// Skicka slave address + read ///
	TWDR = SlaveAddress | 0x01;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));   // wait
	if ((TWSR & 0xF8) != 0x40)
	return -5;
	
	/// Hämta data ///
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));   // wait
	
	/// Stoppbit ///
	TWCR = (1<<TWINT) | (1<<TWSTO);
	
	*Data = TWDR;
	
	return 0;
}

uint8_t SetUpIMU()
{
	uint8_t RegisterData;
	
	I2C_INT();
	_delay_ms(10);
	
	/// Initialisering av gyrot i sensorn /// 
	if( GetI2CData(AngularVelocitySlaveAddress, AngularVelocity_WHO_AM_I_Register,  &RegisterData) )
	return -1;
	if( RegisterData != AngularVelocity_WHO_AM_I_RegisterDefault)	//Kontrollera att vi kommunicerar med rätt slave
	return -1;
	
	if( SendI2CData(AngularVelocitySlaveAddress, AngularVelocity_CTRL1_Register, 0x0F) )   /// Setup
	return -1;

	return 0;
}

//Koverterar sensorvärden till grader
float ConvertToAngles(uint16_t Data)
{
	float DegreesPerSecond;
	float Angle;
	uint16_t NegData = (Data ^ 0xffff) + 1;	//Gör om från två-komplement till "vanliga värden"
	if ((Data & 0x8000) == 0x8000)	//Kontrollera om data är negativ
	{
		DegreesPerSecond = (float) NegData * G_GAIN;
		Angle = DegreesPerSecond * 0.2008;
		Angle = -Angle;
	}
	else
	{
		DegreesPerSecond = (float) Data * G_GAIN;
		Angle = DegreesPerSecond * 0.2008;
	}

	return Angle;
}


int main(void)
{
	sei();
	uart_init();
	spi_init();
	pwm_init();
	uint8_t HighData;	
	uint8_t LowData;
	float Angle = 0;
	
	SetUpIMU();
	
	while (1)
	{
	execute_command(spi_indata);	
	OCR1A=steering_degree;
	OCR1B=motor_speed;	
	GetI2CData(AngularVelocitySlaveAddress, AngularVelocity_OUT_Z_L_Register, &LowData);	//Hämta låg och hög databyte
	GetI2CData(AngularVelocitySlaveAddress, AngularVelocity_OUT_Z_H_Register, &HighData);
	Angle += ConvertToAngles(((uint16_t)HighData << 8) | LowData);	//Lägger till skillnaden i vinkel varje loop
	LowData = (uint8_t)Angle;
	transmit_spi(LowData);	//skickar bara låga bitarna, dvs max 255 grader
	_delay_ms(200);
	}
}
