/*
 * led_kom.h
 *
 * Created: 4/28/2017 9:13:49 AM
 *  Author: tobfr427
 */ 

#define F_CPU 14745600UL

#ifndef LED_KOM_H_
#define LED_KOM_H_

void delay( int mysec );
void led_init( void );
void blink_led(int nr, int mysec);

#endif