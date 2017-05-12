/*
 * main_styr.h
 *
 * Created: 4/6/2017 1:34:25 PM
 *  Author: kargu357
 */


#ifndef MAIN_STYR_H_
#define MAIN_STYR_H_

#define F_CPU 14745600UL

#define BAUD 115200		//Sätter baudrate (bps)
#define BAUDRATE ((F_CPU)/(BAUD*16UL)-1)

//Macro för att undersöka en specifik bit
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

#define Kp 1.8//definiera Kp och Kd
#define Kd 1.5
#define Kp_speed 0.05
#define Kp_stop 1
#define iteration_time 0.01

#define desired_distance_right 50	//pixlar???
#define desired_distance_left 50		//pixlar??
#define stop_value_forward 50
#define stop_value_backwards 50

extern volatile unsigned char executed_command;
extern volatile unsigned char spi_indata;

///Sensordata///
//Avstånd framåt
extern volatile unsigned char distance_front;
//Avstånd bakåt
extern volatile unsigned char distance_back;
//Avstånd höger
extern volatile unsigned char distance_right;
//Avstånd vänster
extern volatile unsigned char distance_left;
//Hastighet
extern volatile unsigned char velocity;
//avstand till linje
extern volatile unsigned char distance_to_line;
extern volatile unsigned char camera_right;
extern volatile unsigned char camera_left;
extern volatile unsigned char camera_front;


///Funktioner///
void blink_led(int nr);
void get_sensor_data(unsigned char data);
void transmit_uart1(unsigned char data);
void transmit_spi(unsigned char data);
unsigned char spi_tranciever(unsigned char data);
void execute_command(unsigned char newcommand);
void autonomous_command(unsigned char newcommand);

#endif /* STYR_H_ */
