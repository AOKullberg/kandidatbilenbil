/*
 * map.c
 *
 * Created: 2017-04-27 11:35:09
 *  Author: Tobias Fridén
 */ 
#include "main.h"
#include "dijsktra.h"
#include "sendrecieve.h"
#include "led_kom.h"
#include "map.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>
#include <stdlib.h>

/* Globala variabler
*/

int no_nodes=6; //Antal noder i kartan
int distancemap[MAX_NODES][MAX_NODES]; //N x N-matris med avståndet mellan nod i och nod j
/*
int	roadmap[6][6] = {
		{99, 99, 2, 99, 99,3},
		{99, 99, 99, 99, 99, 99},
		{99, 99, 99, 99, 4, 99},
		{99, 1, 99, 99, 99, 2},
		{99, 3, 1, 99, 99, 99},
		{99, 99, 99, 4, 99, 99}
	};*/ 
/*
int roadmap[4][4] = {
		{99, 99, 9, 99},
		{99, 99, 99, 6},
		{5, 99, 99, 99},
		{99, 9, 99, 99}
};*/
int roadmap[MAX_NODES][MAX_NODES];//N x N-matris som innehåller vilken nod som ansluter nod i till nod j (högersväng, vänstersväng etc)
/*int	route[7] = {0, 2, 0, 2, 0, 2, 0};*/
int route[MAX_NODES];//1 x N-vektor som innehåller den senast beräknade snabbaste vägen från där rutten beräknades. Målnoden ligger först i vektorn.
int current=0;//Anger på vilket element i route-vektorn som bilen befinner sig
int dist_to_next;//Avstånd till nästa nod
int destination;
int current_node;

/* FUNCTION get_no_nodes()
* Tar emot ett nytt antal noder från datorn
*/
void get_no_nodes(void)
{
	cli();
	while ( !(UCSR0A & (1<<RXC0)) );
	no_nodes = UDR0;
	sei();
}

void get_destination(void)
{
	cli();
	while ( !(UCSR0A & (1<<RXC0)) );
	destination = UDR0;
	sei();	
}

void get_start(void)
{
	cli();
	while ( !(UCSR0A & (1<<RXC0)) );
	current_node = UDR0;
	sei();
}

/* FUNCTION init_maps()
* allokerar minne för de matriser som innehåller information om nodkartan
*/
void init_maps(void)
{
	int i;
	int j;

	for (i=0; i<MAX_NODES; i++)
	for (j=0; j<MAX_NODES; j++)
	distancemap[i][j] = 99;
	
	for (i=0; i<MAX_NODES; i++)
	for (j=0; j<MAX_NODES; j++)
	roadmap[i][j] = 99;
	
	for (i=0; i<MAX_NODES; i++)
	route[i] = 99;
	
}

/* FUNCTION clear_maps()
* frigör minnet som allokerats för information om nodkartan
*/
/*
void clear_maps(void)
{
	int i;
	

	for (i=0; i<no_nodes; i++)
	{
		free(roadmap[i]);
	}
	free(roadmap);
	
	for (i=0; i<no_nodes; i++)
	{
		free(distancemap[i]);
	}
	free(distancemap);
	
	free(route);
}*/
/* FUNCTION update_distancemap()
* tar emot en ny matris från datorn med information om avstånd mellan
* de olika noderna 
*/
void update_distancemap(void)
{
	int i = 0;
	int j = 0;

	cli();
	
	for (i=0; i<MAX_NODES; i++)
	for (j=0; j<MAX_NODES; j++)
	distancemap[i][j] = 99;
	

	for (i=0; i<no_nodes; i++)
	{
		for (j=0;j<no_nodes;j++)
		{
			while ( !(UCSR0A & (1<<RXC0)) );
			distancemap[i][j]=UDR0;
		}
	}
	
	
	sei();
}

/* FUNCTION update_roadmap()
* tar emot en ny matris från datorn med information om 
* vilken sorts noder som ansluter till varandra
*/
void update_roadmap(void)
{
	int i = 0;
	int j = 0;
	cli();

	for (i=0; i<MAX_NODES; i++)
	for (j=0; j<MAX_NODES; j++)
	roadmap[i][j] = 99;	

	for (i=0; i<no_nodes; i++)
	{
		for (j=0;j<no_nodes;j++)
		{
			while ( !(UCSR0A & (1<<RXC0)) );
			roadmap[i][j]=UDR0;
		}
	}
	
	sei();
}

/* FUNCTION check_map(int ** currentmap)
* skickar tillbaks ett element i taget i currentmap för att 
* datorn skall kunna verifiera att rätt karta tagits emot
*/
void check_map(int currentmap[MAX_NODES][MAX_NODES] )
{
	int i=0;
	int j=0;
	
	for (i=0;i<no_nodes;i++)
	{
		for (j=0;j<no_nodes;j++)
		{
			transmit_uart0(currentmap[i][j]);
		}
	}
}

/*FUNCTION new_route(int destination)
* uppdaterar path till att visa snabbaste väg från den nod bilen 
* befinner sig på till noden destination
*/
void new_route(int destination)
{

	for (int i=0; i<no_nodes; i++)
	{
		route[i]=0;
	}
	
	dijsktra(distancemap,current_node,destination);
	
	transmit_uart0(current+1);
		
	for(int i=0; i<no_nodes; i++)
	{
		transmit_uart0(route[i]);
	}
}

/* FUNCTION next_node()
* uppdaterar globala variabler då bilen kommit till en ny node
*/
void next_node(void)
{
	current -=1;
	//dist_to_next = distancemap[route[current]][route[current-1]];
	//position = 0;		
}

/* FUNCTION send_node_command(int node_type)
* skickar till styr vilket kommando som skall utföras beroende
* på nodtyp
*/
void send_node_command(int node_type)
{
	switch(node_type)
	{
		case 1 : //Korsning vänstersväng
		transmit_spi(0xd0);
		while (steering_decision!=0xff)
		{
			transmit_spi(0xd0);
			send_data();
			_delay_ms(10);
		}
		transmit_spi(0xc0);
		next_node();
		break;
		
		case 2 : //Korsning högersväng
		transmit_spi(0xe0);
		while (steering_decision!=0xff)
		{
			transmit_spi(0xe0);
			send_data();
			_delay_ms(10);
		}
		transmit_spi(0xc0);
		next_node();
		break;
		
		case 3 : //Korning rakt fram
		transmit_spi(0x81);
		while (steering_decision!=0xff)
		{
			transmit_spi(0x81);
			send_data();
			_delay_ms(10);
		}
		transmit_spi(0xc0);
		next_node();
		break;
		
		case 4 : //Stoppplats
		transmit_spi(0x82);
		while (steering_decision!=0xff)
		{
			transmit_spi(0x82);
			send_data();
			_delay_ms(10);
		}
		transmit_spi(0xc0);
		next_node();
				
		case 5 : //Parkeringsficka
		transmit_spi(0x83);
		while (steering_decision!=0xff)
		{
			transmit_spi(0x83);
			send_data();
			_delay_ms(10);
		}
		transmit_spi(0xc0);
		next_node();
		break;
		
		
	}
}

/* FUNCTION execute_node_end()
* utför korrekt kommando då bilen nått fram till en ny anslutande nod
*/
void execute_node_end(void) //Todo: Om noden inte är en korsning, eller om framme vid målet
{
	blink_led(4,10);
	if (current > 0)
	{
		int node_type = roadmap[route[current]][route[current-1]];
		send_node_command(node_type);
	}
	else if (current==0)
	{
		transmit_spi(0xaa);
		steering_decision=0xaa;
		computer_indata=0;
		mode=1;
	}

}

