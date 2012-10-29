/*
 * condtable.c
 *
 *  Created on: 16/09/2010
 *      Author: Renato Vimieiro
 */

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "condtable.h"
#include "database.h"

#ifdef _OPENMP
#include <omp.h>
#include <limits.h>
#define THREAD_THRESHOLD 50
#endif

extern Database rep;

#ifdef _OPENMP
static int cmpInt(const void * a, const void * b) { return *((int*)a) - *((int*)b); }
#endif

int* conditionalTable (const int* src, int* sizeSrc, const BitArray* set){
	int* dst;
	int i;
//	int cmp;
	register WORD *a, *b, *end;
	end = (set->data) + (set->length);

#ifdef _OPENMP
	int chunk = 100;
	unsigned short * tmp = (unsigned short *)malloc(*sizeSrc * sizeof(short));
#endif

	int count = 0;
	int size = *sizeSrc;
	register int elem;
	assert(src && set);
	dst = (int*)malloc(sizeof(int)*size);
	//memset(dst,127,sizeof(int)*size);

#ifdef _OPENMP
	//memset(dst,127,sizeof(int)*size);
	if(size > THREAD_THRESHOLD){
#pragma omp parallel for \
	shared(src,dst,set,tmp) private(i,elem,a,b) \
	schedule(static) \
	num_threads(50)
	for(i = 0; i < size; i++){
//		fprintf(stderr,"TID %d -- pn i = %d\n",omp_get_thread_num(),i);
		elem = src[i];
		for(a=rep.data[elem]->data,b=set->data; b < end && !(*a & ~(*b)); a++, b++);
		tmp[i] = (b==end);
	}	
//	qsort(dst,size,sizeof(int),cmpInt);
	for(i=0, count = 0; i < size; i++) if(tmp[i]) dst[count++] = src[i];
	free(tmp);
	}else {
		for(i = 0; i < size; i++){
			elem = src[i];
			for(a=rep.data[elem]->data,b=set->data; b < end && !(*a & ~(*b)); a++, b++);
			if(b == end) dst[count++] = elem;
		}
		
	}
	
#else
	for(i = 0; i < size; i++){
		elem = src[i];
		for(a=rep.data[elem]->data,b=set->data; b < end && !(*a & ~(*b)); a++, b++);
		if(b == end) dst[count++] = elem;

//		elem = src[i];
		//if(elem < 0 || elem >= rep.size) return NULL;
//		cmp = fastCompareSubset(rep.data[elem],set);//compareBitArray(rep.data[elem],set);
//		if(cmp == SUBSET || cmp == EQUAL)
//			dst[count++] = elem;
	}
#endif
	*sizeSrc = count;
	dst = (int*)realloc(dst,count*sizeof(int));
	return dst;
}
