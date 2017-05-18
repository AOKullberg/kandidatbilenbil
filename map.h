/*
 * map.h
 *
 * Created: 2017-04-27 11:34:55
 *  Author: Tobias Fridén
 */ 

#ifndef MAP_H_
#define MAP_H_

#define F_CPU 14745600UL
#define MAX_NODES 20
#include <math.h>

extern int no_nodes;
extern int distancemap[MAX_NODES][MAX_NODES];
extern int roadmap[MAX_NODES][MAX_NODES];
extern int route[MAX_NODES];
extern int current;
extern int dist_to_next;
extern int destination;
extern int current_node;


void init_maps(void);
void clear_maps(void);
void get_no_nodes(void);
void get_destination(void);
void get_start(void);
void update_distancemap(void);
void update_roadmap(void);
void check_map(int currentmap[MAX_NODES][MAX_NODES]);

void new_route(int destination);
void update_position(void);

void next_node(void);
void execute_node_end(void);
#endif
