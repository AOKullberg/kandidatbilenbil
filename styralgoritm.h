/*
 * styralgoritm.h
 *
 * Created: 4/6/2017 2:59:15 PM
 *  Author: kargu357
 */


#ifndef STYRALGORITM_H_
#define STYRALGORITM_H_

extern volatile short prior_right;
extern volatile short prior_left;

extern volatile short prior_placement;

int lost_right;
int lost_left;

signed char calculate_error(char desired, unsigned char actual);
signed char derivate(char error, signed char prior_error);
void pd_steering_control(char desired_distance, unsigned char actual_distance, signed char prior_error, char direction);
void one_line_control(char desired_distance, unsigned char actual_distance, signed char prior_error, char direction);
void two_line_control(void);
void cruise_control(unsigned char wanted_velocity);
void drive_forward_distance(float distance_forward);
void drive_backwards(unsigned char distance_backwards);
void autonomous_driving(void);
void turn_90_degrees(char,char);
void turn_x_degrees(char,char,char,unsigned char);
void drive_for_time(char, int, unsigned char, unsigned char);
void drive_to_stopline(void);
void crossroad_left(void);
void crossroad_right(void);
void crossroad_forward(void);
void stopline(void);





#endif /* STYRALGORITM_H_ */
