/*
 * bitarray.c
 *
 *  Created on: 13/09/2010
 *      Author: Renato Vimieiro
 */

#include "bitarray.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include <R.h>
#include <Rdefines.h>
#include <Rinternals.h>

#ifdef __SSE4_2__
#include <smmintrin.h>
#define popcount(a) _mm_popcnt_u64(a)
#else
#define popcount(a) __builtin_popcountll(a)
#endif

//Convert from bit index to array index. index = bit in number; base = array base address; cur = current position of the array
#define getIndex(__index,__base,__cur) (__index>=0)?__index+NBITS*(__base-__cur):-1
//Last bit set in number before index
#define lastSet(__nb,__index) (__index>=NBITS)?(NBITS-__builtin_clzll(__nb)-1): \
							((__nb & ~(-((WORD)1<<__index)))?NBITS-__builtin_clzll(__nb & ~(-((WORD)1<<__index)))-1:-1)
//First bit set in number after index
#define firstSet(__nb,__index) __builtin_ffsll(__nb & -((WORD)1<<__index))-1


/**
 * Checks whether set1 has all elements from set2 greater than index
 */
unsigned char containsElemGreaterThanIndex(BitArray * set1,BitArray *set2, int index){
	register WORD v;
	register WORD *a, *b, *end;
	if(index >= set1->size) return 1;
	a = set1->data + index/NBITS;
	b = set2->data + index/NBITS;
	end = set1->data + set1->length;
	v= (*b & -((WORD)1<<(index%NBITS)));
	for(;!(v && (~(*a) & v)) && a < end; v = *(++b), a++);
	return a == end;
}

BitArray * newBitArray(SIZET size)
{
	BitArray * tmp = (BitArray *) malloc(sizeof(BitArray));
	if(! tmp) {
		error("Disclosed: ran out of memory. Consider increasing the min. support threshold\n");
//		fprintf(stderr,"Out of memory while creating new BitArray\n");
//		exit(EXIT_FAILURE);
	}

	tmp->size = size;
	tmp->length = (SIZET) ceil(((float)size)/(sizeof(WORD)<<3));
	tmp->data = (WORD *) calloc(tmp->length,sizeof(WORD));
	memset(tmp->data,0,tmp->length*sizeof(WORD));
	tmp->nelements = 0;

	return tmp;
}

void initializeBitArray(BitArray * container, SIZET size)
{
	if(! container) {
		return;
	}

	container->size = size;
	container->length = (SIZET) ceil(((float)size)/(sizeof(WORD)<<3));
	container->data = (WORD *) calloc(container->length,sizeof(WORD));
	memset(container->data,0,container->length*sizeof(WORD));
	container->nelements = 0;
}

BitArray * newFullBitArray(SIZET size)
{
	BitArray * tmp = (BitArray *) malloc(sizeof(BitArray));
	int i;
	int offset;
	int pos;
	if(! tmp) {
		error("Disclosed: ran out of memory. Consider increasing the min. support threshold\n");
//		fprintf(stderr,"Out of memory while creating new BitArray\n");
//		exit(EXIT_FAILURE);
	}

	tmp->size = size;
	tmp->length = (SIZET) ceil(((float)size)/(sizeof(WORD)<<3));
	tmp->data = (WORD *) calloc(tmp->length,sizeof(WORD));
	if(tmp->length > 1)
		memset(tmp->data,UCHAR_MAX,(tmp->length-1)*sizeof(WORD));
	offset = size%(sizeof(WORD)<<3);
	pos = tmp->length-1;
	for(i = 0; i < offset; i++)
		tmp->data[pos] |= ((WORD)1) << i;

	tmp->nelements = size;
	return tmp;
}

//#ifdef __cplusplus
//extern "C"
//#endif
//void showBitArray (FILE * output, const BitArray * ba)
//{
//	SIZET i, count;
//	register int j;
//
//	fprintf(output,"[");
//
//	for (i = 0, count = 0; i < ba->length; i++){
//		int nbits = sizeof(WORD)<<3;
//		if(ba->data[i]) {
//			const WORD one = 1;
//			for(j = 0; j < nbits; j++,count++){
//				if( (one<<j) & ba->data[i] )
//					fprintf(output,"%d,",count);
//			}
//		}
//		else{
//			count+= nbits;
//		}
//	}
//	if(ba->nelements)	fprintf(output,"\b]");
//	else fprintf(output,"]");
//}

/*
 * Do the bit sets union and return a new set with the result
 */
BitArray * bitArrayUnion (BitArray * set1, BitArray * set2) {
	SIZET i;
	BitArray * result;
	assert( set1!= NULL && set2 != NULL );
	result = newBitArray(set1->size);
	for (i = 0; i < set1->length; i++){
		result->data[i] = set1->data[i] | set2->data[i];
	}
	return result;
}

/*
 * Do the bit sets union and write the result in set1
 */
void bitArrayInPlaceUnion (BitArray * set1, BitArray * set2) {
	register WORD *a, *b;
	register const unsigned short size = set1->length;
	register SIZET i;
	register int count = 0;
	
	a = set1->data;
	b = set2->data;	

	for (i = 0; i < size; i++, a++, b++){
		*a |= *b;
		count += popcount(*a);
	}
	set1->nelements = count;
}

int equalBitArray (BitArray * set1, BitArray * set2){
	register const unsigned short size = set1->length;
	register unsigned short i;	
	register WORD *a, *b;
	a = set1->data;
	b = set2->data;	
	for(i=0; i < size; i++, a++, b++ ){
		if(*a!=*b) return DIFFERENT;
	}
	return EQUAL;
}

int fastCompareSubset (const BitArray * set1, const BitArray * set2){
	register WORD *a, *b, *end;
	a = set1->data;
	b = set2->data;
	end = set1->data + set1->length;
	while(!(*a & ~(*b)) && a < end) {a++; b++;}
	return a == end?SUBSET:DIFFERENT;
}

void destroyBitArray(BitArray* set){
	if(set){
		free(set->data);
		free(set);
	}
}

//Gets the last bit set before *from*
int lastBitSet(BitArray* set, int from){
	register char i;
	register WORD *a, *end;
	if(from < 0) return -1;
	a = set->data + from/NBITS;
	end = set->data;
	for(i = lastSet(*a,(from%NBITS)); i < 0 && --a >= end; i = lastSet(*a,NBITS));
	return getIndex(i,a,set->data);
}

int firstBitSet(BitArray* set, int from){
	register char i;
	register unsigned char start;
	register WORD *a, *end;
	register unsigned short pos;
	assert(set);
	
	if ((from < 0) || (from >= set->size)) return -1;
	start = from%NBITS;
	pos = (from/NBITS);
	a = set->data + pos;
	end = set->data + set->length;

	for(i=firstSet(*a,start); i < 0 && ++a < end; i = firstSet(*a,0));
	return getIndex(i,a,set->data);
}

void bitArrayToArray(const BitArray * ba, int ** dest){
	int i, j;
	int count;
	int index;
	int nbits = sizeof(WORD)<<3;
	if(!ba) { *dest = NULL; return; }
	if(!*dest) *dest = (int*)malloc(sizeof(int)*ba->nelements);
	for (i = 0, count = 0, index= 0; i < ba->length; i++){
		if(ba->data[i]) {
			const WORD one = 1;
			for(j = 0; j < nbits; j++,count++){
				if( (one<<j) & ba->data[i] )
					(*dest)[index++] = count;
			}
		}
		else{
			count+= nbits;
		}
	}
}
