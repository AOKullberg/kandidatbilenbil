#include <wiringPi.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <vector>
#include "hallsensor.hpp"

/* FUNCTION ping_hall_myseconds( int, int )
 * pingar hallsensorn på pinnen inputpin, samplar max_samples antal
 * gånger och returnerar medianen
 */
int ping_hall_myseconds( int inputpin, int max_samples ){
	
	vector<int> values{0};
	int msec = 0;
	int samples = 0;
	
	//Väntar på att pinne skall bli låg (nästa ping från magnet)
	while( digitalRead(inputpin) ){
		delay(1);
		++msec;
		if(msec >= 300 ){
			return 0;
		}
	}
	
	msec = 0;
	
	//Mäter avstånd mellan två pingar max_samples antal gånger	
	while( samples < max_samples ){
		
		while( ! digitalRead(inputpin) ){
			delay(1);
			++msec;
			if(msec >= 300 ){
				return 0;
			}
			
		}
		
		msec = 0;
		
		while( digitalRead(inputpin) ){		
			delay(1);
			++ msec;
			
			if(msec >= 300 ){
				return 0;
			}
		}
		values.push_back(msec);
		samples++;
		msec=0;
	}
	
	sort(values.begin(), values.end());
		
	return values[max_samples/2+1];
}
/* FUNCTION hallsensor_setup(void)
 * Ställer in de inställningar som krävs för hallsensor
 */
void hallsensor_setup(void){
	pinMode (26, INPUT);
}
