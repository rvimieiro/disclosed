/*
 * carpentor.c
 *
 *  Created on: 16/09/2010
 *      Author: Renato Vimieiro
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "bitarray.h"
#include "database.h"
#include "condtable.h"
#include "disclosed.h"

#ifdef PROFILING
#include <sys/time.h>
#include <sys/resource.h>
double elapsed;
struct timeval start, stop;
int __count;
#endif

extern long int numberCandidates;
extern long int totalClosed;
extern unsigned char showTIDs;
extern Database rep;
extern int * labelFeatures;

BitArray * __tidset;



inline unsigned char reducible(BitArray *X, BitArray* tidset, int index);
inline void reduce(BitArray *dst, BitArray *src, int index);

void disclosed (FILE * output, int minsup, int maxsup){
	BitArray * X;
	int* TTx;
	int i;
	assert(output);
	numberCandidates = 0;

	X = newBitArray(rep.objsize);
	TTx = (int*)malloc(rep.size*sizeof(int));
	for(i = 0; i < rep.size; i++) {
		TTx[i] = i;
		bitArrayInPlaceUnion(X,rep.data[i]);
	}
//	fprintf(stderr,"\n==============DEBUG==============\n");
//	fprintf(stderr,"Minsup = %d\n",minsup);
//	fprintf(stderr,"\n============END DEBUG============\n");

	if(X->nelements >= minsup) traverse(output,&TTx,i,X,minsup,maxsup,lastBitSet(X,X->size)+1);
	destroyBitArray(X);
	free(TTx);

}

void traverse  (FILE * output, int** TTx, int sizeTTx, BitArray * X, int minsup, int maxsup, int index){
	int i;
	int minfI;
	int* cond;
	int nelem;
	int sizeNewTTx;
	BitArray * tmp;

//		fprintf(stderr,"\n==============DEBUG==============\n");
//		fprintf(stderr,"X = \n");
//		showBitArray(stderr,X);
//		fprintf(stderr,"\n============END DEBUG============\n");

	__tidset = newBitArray(rep.objsize);

	minfI = X->size;

	/* creating the itemset. Read the labels from the list and insert them into the array */
	for(i = 0; i < sizeTTx; i++){
		tmp = rep.data[(*TTx)[i]];
		bitArrayInPlaceUnion(__tidset,tmp);
		nelem = tmp->nelements;
		minfI = minfI>nelem?nelem:minfI;
	}

	if(!containsElemGreaterThanIndex(__tidset,X,index))	return;

	numberCandidates++;

	if(equalBitArray(__tidset,X) == EQUAL && __tidset->nelements < maxsup){
		fprintf(output,"[%d",labelFeatures[(*TTx)[0]]);
		for(i=1;i<sizeTTx;i++) fprintf(output,",%d",labelFeatures[(*TTx)[i]]);
		fprintf(output,"]");
		if(showTIDs){
			fprintf(output,"\t");
			showBitArray(output,X);
		}
		fprintf(output,"\t%d,%d\n",X->nelements,__tidset->nelements);
		totalClosed++;
	}
	if(!reducible(X,__tidset,index)){
		destroyBitArray(__tidset);
//		cleanBitArray(__tidset);
		for(index = lastBitSet(X,index); index > -1; index = lastBitSet(X,index)){
			removeBitArray(X,index);
			if(X->nelements >= minsup && X->nelements >= minfI){

				sizeNewTTx = sizeTTx;

#ifdef PROFILING
	gettimeofday(&start,NULL);
#endif

				cond = conditionalTable(*TTx,&sizeNewTTx,X); 

#ifdef PROFILING
	gettimeofday(&stop,NULL);
	elapsed = (stop.tv_sec-start.tv_sec)*1000.0;
	elapsed += (stop.tv_usec-start.tv_usec)/1000.0;
	fprintf(stderr,"%s -- CondTable elapsed time (%f) -- sizeTTx %d\n",__func__,elapsed,sizeTTx);
#endif

				if(sizeNewTTx > 0){
					traverse(output,&cond,sizeNewTTx,X,minsup,maxsup,index);
				}
				free(cond);
			}
			insertBitArray(X,index);
		}
	} else{
		reduce(__tidset,X,index);
		if(__tidset->nelements >= minsup)
			traverse(output,TTx,sizeTTx,__tidset,minsup,maxsup,index);
	}
}

inline unsigned char reducible(BitArray *X, BitArray* tidset, int index){
	unsigned char reduce = 0;
	register int i;
	for(i = lastBitSet(X,index); i >= 0 && !reduce; i = lastBitSet(X,i)) reduce = !containsElem(tidset,i);
	return reduce;
}

inline void reduce(BitArray *tidset, BitArray *X, int index){
	register int i;
	for(i=firstBitSet(X,index+1); i > 0; i = firstBitSet(X,i+1)) insertBitArray(tidset,i);
}
