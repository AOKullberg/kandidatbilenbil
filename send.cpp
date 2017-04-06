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
 unsigned char velocity = 0;
 unsigned char distance_front=0;
 unsigned char distance_back=0;
 unsigned char distance_right=0;
 unsigned char distance_left = 0;
 unsigned char ack = 0x61;
 int fd;
 i2cReadWrite i2c;
 
 void read_sensors(void){
	 
	vector <int> ultraljud_data{0};
	 
	if( ping_hall_myseconds(26,5) != 0){
		velocity=6900/ping_hall_myseconds(26,5);
	}
	else{
		velocity = 0;
	}
	
 cout << "Hastighet :" << (int)velocity << endl;
 delayMicroseconds(100);
 
 distance_front=ping_laser_distance(i2c);
 cout << "Fram: " << (int)distance_front << endl;
 delayMicroseconds(100);
 
 ultraljud_data=ping_ultra_distance();
 distance_back = ultraljud_data[1];
 distance_right = ultraljud_data[2];
 distance_left = ultraljud_data[3];
 
 
 cout <<"Bak: " << (int)distance_back << endl;
 cout <<"Höger: " << (int)distance_right << endl;
 cout <<"Vänster:" << (int)distance_left << endl << endl;
 
 

 }
 
 void send_sensor_data(void){
	 
	 serialPutchar(fd, velocity);
	 delay(1);
	 
	 serialPutchar(fd, distance_front);
	 delay(1);
	 
	 serialPutchar(fd, distance_back);
	 delay(1);
	 
	 serialPutchar(fd, distance_right);
	 delay(1);
	 
	 serialPutchar(fd, distance_left);
	 delay(1);
	 
	 serialPutchar(fd, ack);
	 delay(1);
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
		delay(100);
		send_sensor_data();
		delay(100);
	}
	
			
}
