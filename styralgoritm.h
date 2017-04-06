/*
 * styralgoritm.h
 *
 * Created: 4/6/2017 2:59:15 PM
 *  Author: kargu357
 */ 


#ifndef STYRALGORITM_H_
#define STYRALGORITM_H_

extern volatile unsigned char prior_error_right;
extern volatile unsigned char prior_error_left;

unsigned char calculate_error(unsigned char desired_distance, unsigned char actual_distance);
unsigned char derivate(unsigned char error, unsigned char prior_error);
void pd_steering_control(unsigned char desired_distance, unsigned char actual_distance, unsigned char prior_error, char direction);
void drive_forward(unsigned char distance_forward);
void drive_backwards(unsigned char distance_backwards);
void autonomous_driving(void);


#endif /* STYRALGORITM_H_ */