#include "motorstyrning.h"
#include "main.h"
#include "gyro_2.h"
#include "styralgoritm.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "parkeringsficka.h"


void parkeringsficka(void)
{
  brake();
  _delay_ms(100);
  turn_x_degrees('F','R',45);
  turn_x_degrees('F','L',45);
  _delay_ms(1000);
  turn_x_degrees('F','L',45);
  turn_x_degrees('F','R',45);
  accelerate(2);
  _delay_ms(100);
}
