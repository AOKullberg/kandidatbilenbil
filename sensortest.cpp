#include <wiringPi.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

using namespace std;


/* FUNCTION ping_sensor_myseconds()
 * pingar sensorn och returnerar svarstiden i mikrosekunder
 */
int ping_sensor_myseconds(int outputpin, int inputpin)
{
	int mysec=0;

	//Gör sensorn redo för ping
	digitalWrite(outputpin,HIGH);
	delay(100);	
	
	//Skickar ping
	digitalWrite(outputpin,LOW);
	
	//Väntar på positiv flank
	while(! digitalRead(inputpin))
	{
		delayMicroseconds(1);
	}
	
	//Räknar fram tills negativ flank
	while( digitalRead(inputpin))
	{
		++mysec;
		delayMicroseconds(1);
	}
	delay(100);	
	
	return mysec;
}

/* FUNCTION ping_sensor_distance()
 * pingar sensorn 5 gånger, räknar ut medelvärde och
 * returnerar avstånd i mikrometer
 */
int ping_sensor_distance(int outputpin, int inputpin)
{
	int time = 0;
	for(int i=0;i<10;i++)
	{

		time += ping_sensor_myseconds(outputpin, inputpin);
	}
	
	return (time/10)*340;
}


int main (void)
{
	wiringPiSetup();
	pinMode (0, OUTPUT);
	pinMode (1, INPUT);
	
	while(1)
	{

		cout << "Avstånd: " << ping_sensor_distance(0,1) << "\n";
	}
	
	
	

}
