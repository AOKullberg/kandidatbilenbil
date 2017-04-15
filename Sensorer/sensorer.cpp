#include <wiringPi.h>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <wiringSerial.h>
#include "ultraljud.hpp"
#include "hallsensor.hpp"
#include "laser.hpp"
#include "../Bildbehandling/bildbehandling.hpp"
#include "sensorer.hpp"

using namespace std;
/*Globala variabler
 */
extern condition_variable dist_cv;
extern mutex mut_dist;
extern bool dist_ready;
extern bool end_program;
 
unsigned char velocity = 0;
unsigned char buf_velocity = 0;
unsigned char distance_front=0;
unsigned char distance_back=0;
unsigned char distance_right=0;
unsigned char distance_left = 0;

extern int dist_to_right;
extern int dist_to_left;
extern int sign_found;

unsigned char sign_found_temp;
unsigned char dist_r;
unsigned char dist_l;

mutex spd_mut;

unsigned char ack = 0x61;

int kom;
int styr; 

i2cReadWrite i2c;

void measure_speed()
{
	while(1)
	{
		int hall_data = ping_hall_myseconds(26,3);
		if( spd_mut.try_lock() )
		{
			if(hall_data!=0)
			{
				buf_velocity = 6900/hall_data;
			}
			else
			{
				buf_velocity = 0;
			}
			spd_mut.unlock();
		}
		else
		{
			cerr << "Velocity is being sent\n";
		}
		
		if(end_program)
		{
			break;
		}
	}
}

void read_sensors(void)
{
	vector <int> ultraljud_data{0};
	
	/*
	int hall_data = 0;
	
	if( (hall_data=ping_hall_myseconds(26,3)) != 0)
	{
		velocity=6900/hall_data;
	}
	else
	{
		velocity = 0;
	}*/
	
 //cout << "Hastighet :" << (int)velocity << endl;
 
	distance_front=ping_laser_distance(i2c);
	cout << "Fram: " << (int)distance_front << endl;

	ultraljud_data=ping_ultra_distance();
 
	if( ultraljud_data.size() > 1 )
	{
		distance_back = 0; //ultraljud_data[1];
		distance_right = 0; //ultraljud_data[2];
		distance_left = 0; //ultraljud_data[3];
	}
	
	unique_lock<mutex> lk(mut_dist);
	while(!dist_ready)
	{
		dist_cv.wait(lk);
	}
	sign_found_temp = static_cast<unsigned char>(sign_found);
	dist_r = static_cast<unsigned char>(dist_to_right);
	dist_l = static_cast<unsigned char>(dist_to_left);
	dist_ready = false;
	dist_cv.notify_one();
 
 //cout <<"Bak: " << (int)distance_back << endl;
 //cout <<"Höger: " << (int)distance_right << endl;
 //cout <<"Vänster: " << (int)distance_left << endl;
 
 //cout <<"Kamera höger: " << dist_to_right << endl;
 //cout << "Kamera vänster: " << dist_to_left << endl << endl;
}
 
 void send_data(int fd){
	 if( spd_mut.try_lock() )
	 {
		velocity = buf_velocity;
		spd_mut.unlock();
	 }
	 else
	 {
		cerr << "Velocity is updating\n"; 
	 }
	 serialPutchar(fd, velocity);
	// delay(1);
	 
	 serialPutchar(fd, distance_front);
	 //delay(1);
	 
	 serialPutchar(fd, distance_back);
	 //delay(1);
	 
	 serialPutchar(fd, distance_right);
	 //delay(1);
	 
	 serialPutchar(fd, distance_left);
	 //delay(1);
	 
	if( dist_r > 0 )
	{
		 serialPutchar(fd, dist_r);
		 // delay(1);
	}
	else
	{
		serialPutchar(fd, 0);
	}

	if( dist_l > 0 )
	{
		serialPutchar(fd, dist_l);
		// delay(1);
	} 
	else
	{
		serialPutchar(fd, 0);
	}
	 	 
	serialPutchar(fd, ack);
	// delay(1);
 }

 
void send_sensor_data(void)
{
	 send_data(kom);
	 send_data(styr);
}

int sensor_setup(void)
{	
	if((kom =serialOpen("/dev/ttyUSB0",115200)) < 0){
		return -1;
	}
	else{
		cout << "Successfully opened on port :" << kom << endl;
	}
	
	if((styr = serialOpen("/dev/ttyUSB1",115200)) < 0){
		return -1;
	}
	else{
		cout << "Successfully opened on port :" << styr << endl;
	}
	
	wiringPiSetup();
	ultraljud_setup();
	hallsensor_setup();
	i2c.setup("/dev/i2c-1");
	 
	return 0;

}
