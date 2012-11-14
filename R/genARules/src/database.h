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

#endif /* DATABASE_H_ */
