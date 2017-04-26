/*
 * parkering.c
 *
 * Created: 4/26/2017
 *  Author: carra862
 */

#include "motorstyrning.h"
#include "main.h"
#include "gyro_2c.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>



void parking_1(void)
{
  accelerate(1);
  _delay_ms(100); // ändra här
  turn_90_degrees('F', 'R');
  while (distance_front > 5)
  {
    accelerate(1);
  }
  break();
  _delay_ms(1000);// justera
  retardate(2);
  _delay_ms(500); //justera
  break();
  turn_90_degrees('F', 'R');
  turn_x_degrees('F', 'R', 45);// måste justeras
  turn_x_degrees('F', 'L', 45); måste justeras
  accelerate(1);
  _delay_ms(100);

}


void parking_2(void)
{
  accelerate(1);
  _delay_ms(50); // ändra här
  turn_90_degrees('F', 'R');
  while (distance_front > 5)
  {
    accelerate(1);
  }
  break();
  _delay_ms(1000);// justera
  retardate(1);
  _delay_ms(100); //justera
  turn_x_degrees('B','L',45);
  turn_x_degrees('B', 'R', 45);
  retardate(1);
  _delay_ms(100);
  turn_90_degrees('F', 'R');
  turn_x_degrees('F', 'R', 45);// måste justeras
  turn_x_degrees('F', 'L', 45); //måste justeras
  accelerate(1);
  _delay_ms(100);

}


void parking_3(void)
{
  turn_90_degrees('F', 'R');
  while (distance_front > 5)
  {
    accelerate(1);
  }
  break();
  _delay_ms(1000);// justera
  turn_90_degrees('B','L');
  turn_90_degrees('F','R');
  turn_90_degrees('F','L');
  accelerate(1);
  _delay_ms(100);

}
