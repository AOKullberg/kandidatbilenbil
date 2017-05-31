#include <wiringPi.h>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <wiringSerial.h>
#include "sensorer.hpp"
#include "ultraljud.hpp"
#include "hallsensor.hpp"
#include "laser.hpp"
#include "../Bildbehandling/bildbehandling.hpp"

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

unsigned char camera_right;
unsigned char camera_left;

vector<unsigned char> camera_right_buffer {0, 0, 0};
vector<unsigned char> camera_left_buffer {0, 0, 0};

extern int dist_to_right;
extern int dist_to_left;
extern int dist_to_stop;
extern int sign_found;

int counter = 0;

unsigned char sign_found_temp;
unsigned char dist_r;
unsigned char dist_l;

mutex spd_mut;

unsigned char ack = 0x61;

int kom;
int styr; 

i2cReadWrite i2c;

vector <unsigned char> get_camera_median(void)
{
	vector<unsigned char> camera_values{0, 0}; 
	
	camera_right_buffer.pop_back();
	camera_left_buffer.pop_back();
	
	camera_right_buffer.insert(camera_right_buffer.begin(),dist_to_right);
	camera_left_buffer.insert(camera_left_buffer.begin(),dist_to_left);
	
	vector <unsigned char> sort_right (camera_right_buffer);
	vector <unsigned char> sort_left (camera_left_buffer);
	
	sort(sort_right.begin(),sort_right.end());
	sort(sort_left.begin(),sort_left.end());

	camera_values.insert(camera_values.begin(),sort_right[0]);
	camera_values.insert(camera_values.begin(),sort_left[0]);
	
	return camera_values;
}

void measure_speed()
{
	while(1)
	{
		int hall_data = ping_hall_myseconds(26,5);
		if( spd_mut.try_lock() )
		{
			if(hall_data!=0)
			{
				buf_velocity = 2760/hall_data;
			}
			else
			{
				buf_velocity = 0;
			}
			spd_mut.unlock();
		}
		else
		{
			cerr << "Speed being accessed\n";
		}
	}
}

void read_sensors(void)
{
	
	/*
	counter++;
	
	vector <int> ultraljud_data{0};	
	
	
	int hall_data = 0;

	if (counter == 4)
	{
		if( (hall_data=ping_hall_myseconds(26,2)) != 0)
		{
			velocity=2760/hall_data;
		}
		else
		{
			velocity = 0;
		}
		counter = 0;
	}*/
	vector <unsigned char> camera_values (2,0);
	distance_front=ping_laser_distance(i2c);
	//cout << "Fram: " << (int)distance_front << endl;
/*
	ultraljud_data=ping_ultra_distance();
 
	if( ultraljud_data.size() > 1 )
	{
		distance_back = ultraljud_data[1];
		distance_right = ultraljud_data[2];
		distance_left = ultraljud_data[3];
	}*/
 
	camera_values = get_camera_median();
	camera_left = camera_values[0];
	camera_right = camera_values[1];
	//cout << "Kamera vänster: " << (int)camera_left << endl;
 cout <<"Kamera höger: " << (int)camera_right << " "<< endl;
 //cout << "Kamera v real: " << (int)dist_to_left << " ";
 //cout << "Kamera h real: " << (int)dist_to_right << " " << endl;
 
 //cout << "Styrsignal: " << 1.5*((int)dist_to_left-(int)dist_to_right) << " "<< endl;
 //cout << "Styrsignal " << dist_to_left - dist_to_right << endl;
 //cout << "Stop distance: " << dist_to_stop<< endl;
}
 
void send_data(int fd)
{
	if( spd_mut.try_lock() )
	{
		velocity = buf_velocity;
		spd_mut.unlock();
		//cout << "Hastighet: " << (int)velocity << endl;
	}
	else
	{
		cout << "Velocity is updating\n";
	}
	serialPutchar(fd, velocity);
	// delay(1);
	 
	serialPutchar(fd, distance_front);

	serialPutchar(fd,camera_right);
	
	serialPutchar(fd, camera_left);
	
	serialPutchar(fd, dist_to_stop);
	 	 
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
