#include <wiringPi.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <vector>
#include "ultraljud.hpp"

using namespace std;


/* FUNCTION ping_ultra_myseconds()
 * pingar sensorn och returnerar svarstiden i mikrosekunder
 */
vector<int> ping_ultra_myseconds()
{

	int mysec_back=0;
	int mysec_right=0;
	int mysec_left=0;
	
	int outputpin = 1;
	int input_back=22;
	int input_right=23;
	int input_left=21;
	
	vector<int> data{0};

	//Gör sensorn redo för ping
	digitalWrite(outputpin,HIGH);
	delayMicroseconds(50);	

	
	//Skickar ping
	digitalWrite(outputpin,LOW);
	
	//Väntar på positiv flank
	while(! digitalRead(input_back))
	{
		delayMicroseconds(1);
		++mysec_back;
		if( mysec_back > 2000){
			return {0};
		}
	}
	
	mysec_back = 0;
	
	while( digitalRead(input_back) || digitalRead(input_right) 
			|| digitalRead(input_left)){
				
				if( digitalRead(input_back) ){
					++mysec_back;
				}
				
				if( digitalRead(input_right) ){
					++mysec_right;
				}
				
				if( digitalRead(input_left) ){
					++mysec_left;
				}
				
				delayMicroseconds(1);
				
				if( (mysec_back > 2000) || (mysec_right > 2000) 
					|| (mysec_left > 2000)){
						return {0};
					}
			}
	
	
	data[0]=mysec_back;
	data[1]=mysec_right;
	data[2]=mysec_left;
	
	delay(50);

	return data;
}

/* FUNCTION ping_ultra_distance()
 * pingar sensorn 5 gånger, räknar ut medelvärde och
 * returnerar avstånd i mikrometer
 */
vector<int> ping_ultra_distance()
{
	
	vector <int> back{0};
	vector <int> right{0};
	vector <int> left{0};
	
	vector <int> temp{0};
	vector <int> data{0};
	
	
	for(int i=0;i<5;i++)
	{
		temp = ping_ultra_myseconds();
		back.push_back(temp[0]);
		right.push_back(temp[1]);
		left.push_back(temp[2]);
	}
	
	sort(back.begin(), back.end());
	sort(right.begin(), right.end());
	sort(left.begin(), left.end());
	
	data.push_back(back[3]*340/10000);
	data.push_back(right[3]*340/10000);
	data.push_back(left[3]*340/10000);

	return data;
}
/* FUNCTION ultraljud_setup()
 * Ställer in de inställningar som krävs för ultraljudssensorer
 */
void ultraljud_setup(){
	pinMode (1, OUTPUT); 
	pinMode (21, INPUT);
	pinMode (22, INPUT);
	pinMode (23, INPUT); 
	
}
	




