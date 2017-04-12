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



void back_out_from_garage(void)
{
  while(distance_forward < 45)
  {
    retardate(1);
  }
}



void first_right_turn(void)
{

  /*uint8_t HighData;
	uint8_t LowData;
	float Angle = 0;
  SetUpIMU();

  while (Angle < 85)
  {
    GetI2CData(AngularVelocitySlaveAddress, AngularVelocity_OUT_Z_L_Register, &LowData);	//Hämta låg och hög databyte
  	GetI2CData(AngularVelocitySlaveAddress, AngularVelocity_OUT_Z_H_Register, &HighData);
    Angle += ConvertToAngles(((uint16_t)HighData << 8) | LowData);	//Lägger till skillnaden i vinkel varje loop
    turn_right(5);
  }*/
  turn_right(5);

}

void drive_forward(void)
{
  
}





void gardsplan(void)
{
  drive_out_from_garage();
  first_right_turn();
  for (int i=0; i<177; i++)
  {
    _delay_ms(17);
  }
  break();
  turn_left(5);




}
