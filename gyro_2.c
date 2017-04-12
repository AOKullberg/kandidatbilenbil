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
		Angle = DegreesPerSecond * 0.2008; //behöver ändra då loopen inte är samma som tidigare
		Angle = -Angle;
	}
	else
	{
		DegreesPerSecond = (float) Data * G_GAIN;
		Angle = DegreesPerSecond * 0.2008;  //behöver ändra då loopen inte är samma som tidigare
	}

	return Angle;
}
