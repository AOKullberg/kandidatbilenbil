#include <wiringPi.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

/* FUNCTION ping_hall_myseconds( int, int )
 * pingar hallsensorn på pinnen inputpin, samplar max_samples antal
 * gånger och returnerar medelvärdet
 */
int ping_hall_myseconds( int inputpin, int max_samples ){
	
	int mysec = 0;
	int samples = 0;
	
	//Väntar på att pinne skall bli låg (nästa ping från magnet)
	while( digitalRead(inputpin) ){
		delayMicroseconds(1);
	}
	
	//Mäter avstånd mellan två pingar max_samples antal gånger	
	while( samples < max_samples ){
		
		while( ! digitalRead(inputpin) ){
			delayMicroseconds(1);
		}
		
		while( digitalRead(inputpin) ){		
			delayMicroseconds(1);
			++ mysec;
		}
		
		samples +=1;
	}
	return mysec/max_samples;
}

void hallsensor_setup(void){
	pinMode (26, INPUT);
}
