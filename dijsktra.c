/*
 * dijkstra.c
 *
 * Created: 2017-04-27 11:37:43
 *  Author: Tobias Fridén
 */ 

#include "dijsktra.h"
#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
/* FUNCTION dijkstra(int **cost, int source, int target)
* uppdaterar route till att innehålla snabbaste rutten från
* noden source till noden target, med hjälp av avståndsinformation
* i N x N matrisen cost. Dijsktras algoritm används.
*/
void dijsktra(int **cost,int source,int target)
{
	int *dist = malloc(no_nodes * sizeof *dist);
	int *prev = malloc(no_nodes * sizeof *prev);
	int *selected = malloc(no_nodes * sizeof *selected);
	int i,m,min,start,d,j;
	
	for (i=0; i < no_nodes; i++)
	{
		selected[i]=0;
	}

	for(i=0;i < no_nodes;i++)
	{
		dist[i] = 99;
		prev[i] = -1;
	}
	start = source;
	selected[start]=1;
	dist[start] = 0;
	while(selected[target] ==0)
	{
		min = 99;
		m = 0;
		for(i=0;i< no_nodes;i++)
		{
			d = dist[start] +cost[start][i];
			if(d< dist[i]&&selected[i]==0)
			{
				dist[i] = d;
				prev[i] = start;
			}
			if(min>dist[i] && selected[i]==0)
			{
				min = dist[i];
				m = i;
			}
		}
		start = m;
		selected[start] = 1;
	}
	start = target;
	j = 0;
	while(start != -1)
	{
		route[j++] = start;
		start = prev[start];
	}
}
