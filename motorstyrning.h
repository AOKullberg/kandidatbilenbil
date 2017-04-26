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

void turn_left(short data);
void turn_right(short data);
void accelerate(unsigned char data); 
void retardate(unsigned char data);
void brake(void);

#endif /* MOTORSTYRNING_H_ */
