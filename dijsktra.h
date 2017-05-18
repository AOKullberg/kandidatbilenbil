/*
 * dijkstra.h
 *
 * Created: 2017-04-27 11:37:34
 *  Author: Tobias Fridén
 */ 
#ifndef DIJKSTRA_H_
#define DIJKSTRA_H_

#define F_CPU 14745600UL
#include "map.h"

void dijsktra(int cost[MAX_NODES][MAX_NODES],int source,int target);

#endif