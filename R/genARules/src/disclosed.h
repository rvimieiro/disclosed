/*
 * disclosed.h
 *
 *  Created on: 20/09/2010
 *      Author: Renato Vimieiro
 */

#ifndef DISCLOSED_H_
#define DISCLOSED_H_

#include <stdio.h>
#include "database.h"
#include "bitarray.h"

void carpentor (int minsup, int maxsup);
void traverse  (int** TTx, int sizeTTx, BitArray * X, int minsup, int maxsup, int index);

#endif /* DISCLOSED_H_ */
