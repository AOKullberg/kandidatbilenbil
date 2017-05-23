/*
 * laser.cpp
 *
 * Created: 2017-04-30 15:10:47
 * Author: Caroline Wasing
 * version 1.0
 */ 
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include "laser.hpp"

using namespace std;

/*FUNCTION setup(string)
 * Öppnar en anslutning till i2c-bussen som man sedan kommer åt
 * med hjälp av file_i2c. För att komma åt i2c på rpi används
 * "/dev/i2c-1" som filename. 
 */
	void i2cReadWrite::setup(string filename){
	
		if((file_i2c = open(filename.c_str(), O_RDWR)) < 0){
			printf("Failed to open the i2c bus");
			return;
		}
		else{
			cout << "Successfully connected to i2c bus on port: " << file_i2c << endl;
		}
		
		//Address för i2c-slaven
		int addr = 0x62;
		if (ioctl(file_i2c, I2C_SLAVE, addr) < 0) {
			printf("Failed to aquire bus access and/or talk to slave.\n");
			return;
		}
	}

/*FUNCTION readi2cData(	unsigned char, unsigned char *, int )
 * Läser length antal bytes från addressen RegisterAddress hos slaven.
 * Dessa placeras i RxBuffer.
 */
	void i2cReadWrite::readi2cData(unsigned char RegisterAddress, unsigned char * RxBuffer, int length){
		string res;
		if (write(file_i2c, & RegisterAddress, 1) != 1){
			printf("I2C write error");
		}
		
		if (read(file_i2c, RxBuffer, length) != length){
			printf("Failed to read from i2c bus.\n");
		}
		

	}
/*FUNCTION writei2cData( unsigned char, unsigned char *, int)
 * Skriver length antal bytes till addressen RegisterAdress hos slaven
 * från TxBuffer.
 */
	void i2cReadWrite::writei2cData(unsigned char RegisterAddress, unsigned char * TxBuffer, int length){
		char data[length+1];
		
		data[0]=RegisterAddress;
		
		for (int i = 0; i < length ; i++ ) {
		data[i+1] = TxBuffer[i];
		}

		if (write(file_i2c, data, length+1) != length+1) {
			printf("Failed to write to the i2c bus.\n");
		}
	}




/*FUNCTION ping_laser_distance( i2c )
 * Tar in ett objekt av i2cReadWrite-klassen och pingar lasern samt
 * returnerar avstånd i cm
 */
int ping_laser_distance(i2cReadWrite i2c ){
	
	unsigned char RxBuffer[2]{0};
	unsigned char TxBuffer[1]{0};
	int data;
	
	//Skickas för att påbörja en mätning
	TxBuffer[0]=0x04;
	
	//Nollställ "ready-check"
	RxBuffer[0]=1;
	
	//Påbörja mätning	
	i2c.writei2cData(0x00, TxBuffer, 1);
	
	//Vänta tills mätning slutförd
	while(1 & RxBuffer[0]){
		i2c.readi2cData(0x01, RxBuffer, 1);
		delayMicroseconds(1);
	}
	
	//Läs mätresultat 2 bytes
	i2c.readi2cData(0x8f, RxBuffer, 2);
	
	//Vill endast returnera 1 byte
	if( RxBuffer[0] >= 0x01 ){ 
		data = 255;
	}
	else{
		data = RxBuffer[1];
	}

	
	return data;
	
}

/*FUNCTION reset_i2c( i2c )
 * Återställer lasern genom att skriva till register 0x00
 */
void reset_i2c(i2cReadWrite i2c){

	unsigned char reset[1]{0};
	i2c.writei2cData(0x00, reset, 1);
	delay(100);

}




