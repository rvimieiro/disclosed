/*
 * carpentor.h
 *
 *  Created on: 20/09/2010
 *      Author: Renato Vimieiro
 */

#ifndef CARPENTOR_H_
#define CARPENTOR_H_

#include <stdio.h>
#include "database.h"
#include "bitarray.h"

void carpentor (FILE * output, int minsup, int maxsup);
void traverse  (FILE * output, int** TTx, int sizeTTx, BitArray * X, int minsup, int maxsup, int index);

#endif /* CARPENTOR_H_ */
