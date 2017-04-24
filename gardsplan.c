/*
 * gardsplan.c
 *
 * Created: 4/11/2017
 *  Author: carra862
 */


 #include "styralgoritm.h"
 #include "main.h"
 #include "motorstyrning.h"
 #include <avr/io.h>
 #include <util/delay.h>
 #include "gyro_2.h"



void back_out_from_garage(void)
{
	OCR1B = 345;
	_delay_ms(3000);
	OCR1B = 328;
	_delay_ms(1000);
	Angle = 0;
	turn_90_degrees('B','R');
	OCR1B = 353;
	/*accelerate(2);*/
	_delay_ms(1000);
	Angle=0;
	turn_90_degrees('F','L');
}



void first_right_turn(void)
{

  uint8_t HighData;
	uint8_t LowData;
	float Angle = 0;
 

  while (Angle < 85)
  {
    GetI2CData(AngularVelocitySlaveAddress, AngularVelocity_OUT_Z_L_Register, &LowData);	//Hämta låg och hög databyte
    GetI2CData(AngularVelocitySlaveAddress, AngularVelocity_OUT_Z_H_Register, &HighData);
    Angle += ConvertToAngles(((uint16_t)HighData << 8) | LowData);	//Lägger till skillnaden i vinkel varje loop
    LowData = (uint8_t)Angle;
    transmit_spi(LowData);	//skickar bara låga bitarna, dvs max 255 grader
    _delay_ms(200);
	blink_led(1);
	OCR1A = 350;
  }
  OCR1A = 320;

}

void drive_forward(void)
{
  
}





void gardsplan(void)
{
  back_out_from_garage();
  first_right_turn();
  for (int i=0; i<177; i++)
  {
    _delay_ms(17);
  }
  brake();
  turn_left(5);




}
