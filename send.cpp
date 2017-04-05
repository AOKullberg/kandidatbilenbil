#include <wiringPi.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <wiringSerial.h>
#include "ultraljud.cpp"
#include "hallsensor.cpp"
#include "laser.cpp"

using namespace std;

/* Global variables
 */
 unsigned char velocity=0;
 unsigned char distance_front=0;
 unsigned char distance_back=0;
 unsigned char ack = 0x61;
 int fd;
 i2cReadWrite i2c;
 
 void read_sensors(void){
 /*
 velocity=ping_hall_myseconds(26,5);
 delayMicroseconds(100);
 */
 distance_front=ping_laser_distance(i2c);
 delayMicroseconds(100);
 
 //cout << "Bak innan :" << ping_ultra_distance(1, 21) << endl;
 distance_back=ping_ultra_distance(1, 21);
 delayMicroseconds(100);

 }
 
 void send_sensor_data(void){
	 
	/* serialPutchar(fd, velocity);
	 delay(100); */
	 //cout << "Fram: " << (int)distance_front;
	 serialPutchar(fd, distance_front);
	 delay(100);
	 
	 //cout << "   Bak: " << (int)distance_back << endl;
	 serialPutchar(fd, distance_back);
	 delay(100);
	 
	 serialPutchar(fd, ack);
	 delay(100);
 }

int main(void){


	
	if((fd =serialOpen("/dev/ttyUSB0",115200)) < 0){
		printf("Error opening serial port");
	}
	else{
		cout << "Sucessfully opened on port :" << fd << endl;
	}
	
	wiringPiSetup();
	ultraljud_setup();
	hallsensor_setup();

	i2c.setup("/dev/i2c-1");
	
	while(1){
		
		
		read_sensors();
		send_sensor_data();
	}
	
			
}
