﻿/*
 * map.c
 *
 * Created: 2017-04-27 11:35:09
 *  Author: Tobias Fridén
 */ 
#include "dijkstra.h"
#include "sendrecieve.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>
#include <stdlib.h>

/* Globala variabler
*/

int no_nodes=0; //Antal noder i kartan
int** distancemap; //N x N-matris med avståndet mellan nod i och nod j
int** roadmap; //N x N-matris som innehåller vilken nod som ansluter nod i till nod j (högersväng, vänstersväng etc)
int* route;//1 x N-vektor som innehåller den senast beräknade snabbaste vägen från där rutten beräknades. Målnoden ligger först i vektorn.
int current;//Anger på vilket element i route-vektorn som bilen befinner sig
int dist_to_next;//Avstånd till nästa nod

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

/* FUNCTION init_maps()
* allokerar minne för de matriser som innehåller information om nodkartan
*/
void init_maps(void)
{
	int i;

	roadmap = malloc(no_nodes * sizeof *distancemap);
	for (i=0;i<no_nodes;i++)
	{
		roadmap[i]=malloc(no_nodes * sizeof *distancemap[i]);
	}
	
	roadmap = malloc(no_nodes * sizeof *roadmap);
	for (i=0;i<no_nodes;i++)
	{
		roadmap[i]=malloc(no_nodes * sizeof *roadmap[i]);
	}
	
	route = malloc(no_nodes * sizeof *route);
	
}

/* FUNCTION clear_maps()
* frigör minnet som allokerats för information om nodkartan
*/
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
	
}
/* FUNCTION update_distancemap()
* tar emot en ny matris från datorn med information om avstånd mellan
* de olika noderna 
*/
void update_distancemap(void)
{
	int i = 0;
	int j = 0;
	int temp = 0;

	cli();
	
	for (i=0;i<no_nodes;i++)
	for (j=0;j<no_nodes;j++)
	distancemap[i][j]=99;
	
	for (i=0;i<no_nodes;i++)
	{
		for (j=i+1;j<no_nodes;j++)
		{
			while ( !(UCSR0A & (1<<RXC0)) );
			temp=UDR0;
			distancemap[i][j]=temp;
			distancemap[j][i]=temp;
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
	
	for (i=0;i<no_nodes;i++)
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
void check_map(int ** currentmap )
{
	int i=0;
	int j=0;
	
	for (i=0;i<no_nodes;i++)
	{
		for (j=0;j<no_nodes;j++)
		{
			transmit_uart0(currentmap[i][j]);
			_delay_ms(100);
		}
	}
}

/*FUNCTION new_route(int destination)
* uppdaterar path till att visa snabbaste väg från den nod bilen 
* befinner sig på till noden destination
*/
void new_route(int destination)
{
	
	dijsktra(distancemap,route[current],destination);
}

/* FUNCTION next_node()
* uppdaterar globala variabler då bilen kommit till en ny node
*/
void next_node(void)
{
	current -=1;
	dist_to_next = distancemap[route[current]][route[current-1]];
	position = 0;		
}

/* FUNCTION execute_node_end()
* utför korrekt kommando då bilen nått fram till en ny anslutande nod
*/
void execute_node_end(void) //Todo: Om noden inte är en korsning, eller om framme vid målet
{
	int node_type = roadmap[route[current]][route[current-1]];
	transmit_spi(node_type);//Utför sväng
	while(1); //Vänta tills sväng färdig
	next_node();
	transmit_spi(0x0a);//Fortsätt autonom körning
}

