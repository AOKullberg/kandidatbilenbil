/*
 * motorstyrning.h
 *
 * Created: 4/6/2017 3:13:53 PM
 *  Author: kargu357
 */ 


#ifndef MOTORSTYRNING_H_
#define MOTORSTYRNING_H_

//Räknare för pwm-signal till styrservo
extern volatile unsigned short steering_degree;
//Räknare för pwm-signal till motorservo
extern volatile unsigned short motor_speed;

void turn_left(unsigned char data);
void turn_right(unsigned char data);
void accelerate(void);
void retardate(void);

#endif /* MOTORSTYRNING_H_ */