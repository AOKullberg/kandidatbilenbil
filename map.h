/*
 * map.h
 *
 * Created: 2017-04-27 11:34:55
 *  Author: Tobias Fridén
 */ 

#ifndef MAP_H_
#define MAP_H_

#define F_CPU 14745600UL
#include <math.h>

extern int no_nodes;
extern int **distancemap;
extern int **roadmap;
extern int *route;
extern int current;
extern int dist_to_next;


void init_maps(void);
void clear_maps(void);
void get_no_nodes(void);
void update_distancemap(void);
void update_roadmap(void);
void check_map(int** currentmap );

void new_route(int destination);
void update_position(void);

void next_node(void);
void execute_node_end(void);
#endif
