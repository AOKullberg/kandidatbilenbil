﻿/*
 * main.h
 *
 * Created: 2017-04-27 10:04:34
 *  Author: Tobias Fridén
 */ 


#ifndef MAIN_KOM_H_
#define MAIN_KOM_H_


#define F_CPU 14745600UL

extern int executing_node;
extern int node_finished;
extern int mode;


void manual_main(void);
void autonom_main(void);
void prog_main(void);
int main(void);

#endif