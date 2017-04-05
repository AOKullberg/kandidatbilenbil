#include <wiringPi.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

using namespace std;


/* FUNCTION ping_ultra_myseconds()
 * pingar sensorn och returnerar svarstiden i mikrosekunder
 */
int ping_ultra_myseconds(int outputpin, int inputpin)
{
	int mysec=0;
	

	//Gör sensorn redo för ping
	digitalWrite(outputpin,HIGH);
	delayMicroseconds(50);	

	
	//Skickar ping
	digitalWrite(outputpin,LOW);
	
	//Väntar på positiv flank
	while(! digitalRead(inputpin))
	{
		//cout << "Loop1 \n";
		delayMicroseconds(1);
	}
	
	//Räknar fram tills negativ flank
	while( digitalRead(inputpin))
	{
		//cout << "Loop2 \n";
		++mysec;
		delayMicroseconds(1);
	}
	delay(50);

	return mysec;
}

/* FUNCTION ping_ultra_distance()
 * pingar sensorn 5 gånger, räknar ut medelvärde och
 * returnerar avstånd i mikrometer
 */
int ping_ultra_distance(int outputpin, int inputpin)
{
	int time = 0;
	for(int i=0;i<5;i++)
	{
		time += ping_ultra_myseconds(outputpin, inputpin);
	}

	return (time/5)*340;
}

void ultraljud_setup(){
	pinMode (1, OUTPUT);
	pinMode (21, INPUT);
}
	




