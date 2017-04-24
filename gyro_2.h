




#ifndef GYRO_H_
#define GYRO_H_
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <compat/twi.h> 

#define AngularVelocitySlaveAddress 0xD6	//address to gyro
#define AngularVelocity_WHO_AM_I_Register 0x0F  // Identifikationsregister för gyrot
#define AngularVelocity_WHO_AM_I_RegisterDefault 0xD7 //defaultvärde
#define AngularVelocity_CTRL1_Register 0x20     // Enabla axlar, bandbredd och ouput data rate
#define AngularVelocity_OUT_Z_L_Register 0x2C   // Z-axel Gyro Låg byte
#define AngularVelocity_OUT_Z_H_Register 0x2D   // Z-axel Gyro Hög byte

#define SCL_CLOCK 100000L	//Intern klocka gyro
#define G_GAIN 0.00875		//Noggrannhetsmått

extern volatile float Angle;

void I2C_INT();
uint8_t SetUpIMU();
uint8_t SendI2CData(uint8_t SlaveAddress, uint8_t Register, uint8_t Data);
uint8_t GetI2CData(uint8_t SlaveAddress, uint8_t Register, uint8_t *Data);
float ConvertToAngles(uint16_t Data);
void Get_Angle();


#endif /*GYRO_H_ */
