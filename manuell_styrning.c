/*
 * manuell_styrning.c
 *
 * Created: 4/6/2017 2:10:03 PM
 *  Author: kargu357
 */

#include "manuell_styrning.h"
#include "motorstyrning.h"
#include "main.h"

//Utför manuellt kommando
void manual_command(unsigned char newcommand)
{
	if (CHECK_BIT(newcommand,1))
	{
		accelerate(1);
		executed_command |= (1<<1);
	}
	if (CHECK_BIT(newcommand,2))
	{
		retardate(1);
		executed_command |= (1<<2);
	}
	if (CHECK_BIT(newcommand,3))
	{
		turn_left(0x04);
		executed_command |= (1<<3);
	}
	if (CHECK_BIT(newcommand,4))
	{
		turn_right(0x04);
		executed_command |= (1<<4);
	}
}
