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
	if (newcommand==0x00)
	{
		turn_both_directions(319*8);
	}
	if (newcommand == 0x01)
	{
		accelerate(1);
	}
	if (newcommand == 0x02)
	{
		retardate(1);
	}
	if (newcommand == 0x04)
	{
		turn_both_directions(10000);
	}
	if (newcommand == 0x08)
	{
		turn_both_directions(0);
	}
	if (newcommand == 0x05)
	{
		accelerate(1);
		turn_both_directions(10000);
	}
	if (newcommand == 0x09)
	{
		accelerate(1);
		turn_both_directions(0);
	}
	if (newcommand == 0x06)
	{
		retardate(1);
		turn_both_directions(10000);
	}
	if (newcommand == 0x0a)
	{
		retardate(1);
		turn_both_directions(0);
	}
	if (newcommand == 0x0b)
	{
		brake();
	}
}
