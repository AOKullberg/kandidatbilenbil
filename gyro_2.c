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
#include "gyro_2.h"

volatile float Angle = 0;

volatile unsigned char time_calc_angle = 0;

#define AngularVelocitySlaveAddress 0xD6	//address to gyro
#define AngularVelocity_WHO_AM_I_Register 0x0F  // Identifikationsregister för gyrot
#define AngularVelocity_WHO_AM_I_RegisterDefault 0xD7 //defaultvärde
#define AngularVelocity_CTRL1_Register 0x20     // Enabla axlar, bandbredd och ouput data rate
#define AngularVelocity_OUT_Z_L_Register 0x2C   // Z-axel Gyro Låg byte
#define AngularVelocity_OUT_Z_H_Register 0x2D   // Z-axel Gyro Hög byte

#define SCL_CLOCK 100000L	//Intern klocka gyro
#define G_GAIN 0.00875



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
		Angle = DegreesPerSecond * (time_calc_angle * 256 + TCNT0)/F_CPU; //behöver ändra då loopen inte är samma som tidigare
		Angle = -Angle;
	}
	else
	{
		DegreesPerSecond = (float) Data * G_GAIN;
		Angle = DegreesPerSecond * (time_calc_angle * 256 + TCNT0)/F_CPU;  //behöver ändra då loopen inte är samma som tidigare
	}

	return Angle;
}

ISR(TIMER0_OVF_vect)
{
	time_calc_angle++;		//ett varde på ca 500 bör betyda en tid i storleksordningen 10 ms
}

void Get_Angle()
{
	uint8_t HighData;
	uint8_t LowData;
	GetI2CData(AngularVelocitySlaveAddress, AngularVelocity_OUT_Z_L_Register, &LowData);	//Hämta låg och hög databyte
	GetI2CData(AngularVelocitySlaveAddress, AngularVelocity_OUT_Z_H_Register, &HighData);
	Angle += ConvertToAngles(((uint16_t)HighData << 8) | LowData);	//Lägger till skillnaden i vinkel varje loop
	//LowData = (uint8_t)Angle;
}
