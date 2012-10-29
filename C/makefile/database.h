/*
 * database.h
 *
 *  Created on: Sep 14, 2010
 *      Author: Renato Vimieiro
 */

#ifndef DATABASE_H_
#define DATABASE_H_

#include <stdio.h>
#include "bitarray.h"


typedef struct __database {
	BitArray ** data;
	int size; //Number of attributes
	int objsize; //Number of objects
} Database;

void readData(char * input, Database* rep);
char * next(char * end, char * ptr);

#endif /* DATABASE_H_ */
