/*
 * sendrecieve.h
 *
 * Created: 2017-04-27 10:03:39
 *  Author: Tobias Fridén
 */ 

#ifndef SENDRECIEVE_H_
#define SENDRECIEVE_H_

#define F_CPU 14745600UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define BAUD 115200		//Sätter baudrate (bps)
#define BAUDRATE ((F_CPU)/(BAUD*16UL)-1)

extern int change_mode;

extern volatile unsigned char computer_indata;

extern volatile unsigned char velocity;
extern volatile unsigned char distance_front;

extern volatile unsigned char steering_decision;

extern volatile unsigned char position;

void uart_init(void);
void spi_init(void);
void transmit_uart0(unsigned char data);
void transmit_uart1(unsigned char data);
void transmit_spi(unsigned char data);
void send_data(void);
void get_sensor_data(unsigned char data);


#endif