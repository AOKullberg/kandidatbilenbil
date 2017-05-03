/*
 * autonom_drive.c
 *
 * Created: 4/6/2017 3:04:17 PM
 *  Author: kargu357
 */

#include "autonom_drive.h"
#include "main.h"
#include "styralgoritm.h"
#include "motorstyrning.h"
#include <avr/io.h>
#include <util/delay.h>

void autonomous_driving(void)
{
	accelerate(1);
	two_line_control();
	_delay_ms(10);
}

void korsning(void)
{

}
