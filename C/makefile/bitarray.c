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

#ifdef __SSE4_2__
#include <smmintrin.h>
#define popcount(a) _mm_popcnt_u64(a)
#else
#define popcount(a) __builtin_popcountll(a)
#endif

#ifdef PROFILING
#include <sys/time.h>

double elapsed;
struct timeval start, stop;
#endif

//Convert from bit index to array index. index = bit in number; base = array base address; cur = current position of the array
#define getIndex(__index,__base,__cur) (__index>=0)?__index+NBITS*(__base-__cur):-1
//Last bit set in number before index
#define lastSet(__nb,__index) (__index>=NBITS)?(NBITS-__builtin_clzll(__nb)-1): \
							((__nb & ~(-((WORD)1<<__index)))?NBITS-__builtin_clzll(__nb & ~(-((WORD)1<<__index)))-1:-1)
//First bit set in number after index
#define firstSet(__nb,__index) __builtin_ffsll(__nb & -((WORD)1<<__index))-1


/*
inline unsigned short containsElem(BitArray * set, int elem){
	int pos;
	WORD mask;
//	assert(set && elem <= set->size);
	pos = elem/NBITS;
	mask = ((WORD)1) << (elem%NBITS);
	return (set->data[pos] & mask) > 0;
}
*/

/**
 * Checks whether set1 has all elements from set2 greater than index
 */
inline unsigned char containsElemGreaterThanIndex(BitArray * set1,BitArray *set2, int index){
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
		fprintf(stderr,"Out of memory while creating new BitArray\n");
		exit(EXIT_FAILURE);
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
		fprintf(stderr,"Out of memory while creating new BitArray\n");
		exit(EXIT_FAILURE);
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

#ifdef __cplusplus
extern "C"
#endif
void showBitArray (FILE * output, const BitArray * ba)
{
	SIZET i, count;
	register int j;

	fprintf(output,"[");

	for (i = 0, count = 0; i < ba->length; i++){
		int nbits = sizeof(WORD)<<3;
		if(ba->data[i]) {
			const WORD one = 1;
			for(j = 0; j < nbits; j++,count++){
				if( (one<<j) & ba->data[i] )
					fprintf(output,"%d,",count);
			}
		}
		else{
			count+= nbits;
		}
	}
	if(ba->nelements)	fprintf(output,"\b]");
	else fprintf(output,"]");
}

/*
 * Do the bit sets union and return a new set with the result
 */
BitArray * bitArrayUnion (BitArray * set1, BitArray * set2) {
//#ifdef _OPENMP
//#else
	SIZET i;
	BitArray * result;
	assert( set1!= NULL && set2 != NULL );
	result = newBitArray(set1->size);
	for (i = 0; i < set1->length; i++){
		result->data[i] = set1->data[i] | set2->data[i];
	}
	return result;
//#endif
}

//inline int countBits(WORD word){
////	  unsigned int ahi = ((unsigned int)(word >> 32));
////	  unsigned int alo = ((unsigned int)(word & 0xffffffffULL));
////	  alo = alo - ((alo >> 1) & 0x55555555);
////	  alo = (alo & 0x33333333) + ((alo >> 2) & 0x33333333);
////	  ahi = ahi - ((ahi >> 1) & 0x55555555);
////	  ahi = (ahi & 0x33333333) + ((ahi >> 2) & 0x33333333);
////	  alo = alo + ahi;
////	  alo = (alo & 0x0f0f0f0f) + ((alo >> 4) & 0x0f0f0f0f);
////	  alo = ((__umul24(alo, 0x808080) << 1) + alo) >> 24;
//
//	  return __;
//}

/*
 * Do the bit sets union and write the result in set1
 */
inline void bitArrayInPlaceUnion (BitArray * set1, BitArray * set2) {
	register WORD *a, *b;
	register const unsigned short size = set1->length;
	register SIZET i;
	register int count = 0;
//	assert( set1 );
//	assert( set2 );
	
	a = set1->data;
	b = set2->data;	

	for (i = 0; i < size; i++, a++, b++){
		*a |= *b;
		count += popcount(*a);//__builtin_popcountll(*a);
	}
	set1->nelements = count;
}
/*
inline void insertBitArray (BitArray * set, int elem) {
//	int nbits;
	assert(set != NULL);
	assert(elem < set->size);
//	nbits = sizeof(WORD)<<3;
	int pos = elem/NBITS;
	WORD mask = ((WORD)1) << (elem%NBITS);
	set->data[pos] |= mask;
	set->nelements++;
}


void removeBitArray (BitArray * set, int elem){
	assert(set != NULL);
	assert(elem < set->size);
	int pos = elem/NBITS;
	WORD mask = ((WORD)1) << (elem%NBITS);
	set->data[pos] &= ~mask;
	set->nelements--;
}
*/
inline int equalBitArray (BitArray * set1, BitArray * set2){
	register const unsigned short size = set1->length;
	register unsigned short i;	
	register WORD *a, *b;
	a = set1->data;
	b = set2->data;	
//	assert(set1 && set2);
//	if(set1 == set2) return EQUAL;
//	if(set1->size != set2->size) return DIFFERENT;
	for(i=0; i < size; i++, a++, b++ ){
		if(*a!=*b) return DIFFERENT;
	}
	return EQUAL;
}

inline int fastCompareSubset (const BitArray * set1, const BitArray * set2){
	register WORD *a, *b, *end;
//	register unsigned short count;
//	register const unsigned short size = set1->length;
//	assert(set1 && set2);
//	if(set1->size != set2->size) return DIFFERENT;
//	if(set1 == set2) return EQUAL;
	a = set1->data;
	b = set2->data;
	end = set1->data + set1->length;
//	count = 0;
//	fprintf(stderr,"before\n");
	while(!(*a & ~(*b)) && a < end) {a++; b++;}
//	fprintf(stderr,"after,count==size %d\n",count==size);
	return a == end?SUBSET:DIFFERENT;
}

/*
int compareBitArray (BitArray * set1, BitArray * set2){
	int i;
	int count;
	WORD *a, *b;
	int nbits;
	int state;


//	fprintf(stderr,"COMPARING BITSETS : \n");
//	showBitArray(stderr,set1);
//	fprintf(stderr,"\n");
//	showBitArray(stderr,set2);
//	fprintf(stderr,"\n__________________________ \n");

	assert(set1 && set2);
	if(set1->size != set2->size) return DIFFERENT;
	if(set1 == set2) return EQUAL;
	a = set1->data;
	b = set2->data;
	count = 0;
	i = 0;
	nbits = sizeof(WORD)<<3;
	state = EQUAL;
	while(count < set1->length){
		switch(state){
		case EQUAL:
			if(*a == *b) { a++; b++; count++; }
			else if(*a < *b) state = SB1;
			else state = SP1;
			break;
		case SB1:
			for(; i < nbits && (((WORD)1<<i)&*b); i++);
			if(i == nbits) { state = SUBSET; a++; b++; count++; }
			else state = SB2;
			break;
		case SB2:
			if((((WORD)1<<i)&*a)) state = DIFFERENT;
			else { state = SB1; i++; }
			break;
		case SUBSET:
			i = 0;
			if(*a == *b) { a++; b++; count++; }
			else if(*a < *b) state = SB1;
			else state = DIFFERENT;
			break;
		case SP1:
			for(; i < nbits && (((WORD)1<<i)&*a); i++);
			if(i == nbits) { state = SUPSET; a++; b++; count++; }
			else state = SP2;
			break;
		case SP2:
			if((((WORD)1<<i)&*b)) state = DIFFERENT;
			else { state = SP1; i++; }
			break;
		case SUPSET:
			i = 0;
			if(*a == *b) { a++; b++; count++; }
			else if(*a > *b) state = SP1;
			else state = DIFFERENT;
			break;
		case DIFFERENT:
			return DIFFERENT;
		}
	}
	return state;
}
*/
void destroyBitArray(BitArray* set){
	if(set){
		free(set->data);
		free(set);
	}
}

//inline int lastBitSet(BitArray* set, int from){
//	int index;
//	int pos;
//	WORD mask;
//	int result = -1;
//
//#ifdef PROFILING
//		gettimeofday(&start,NULL);
//#endif
//
//	assert(set && from <= set->size);
//	for(index = from-1; index >= 0; index--){
//		pos = index/NBITS;
//		mask = ((WORD)1) << (index%NBITS);
//		if(set->data[pos] & mask) { result = index; break; }
//	}
//
//#ifdef PROFILING
//	gettimeofday(&stop,NULL);
//	elapsed = (stop.tv_sec-start.tv_sec)*1000.0;
//	elapsed += (stop.tv_usec-start.tv_usec)/1000.0;
//	fprintf(stderr,"%s elapsed time (%f)\n",__func__,elapsed);
//#endif
//
//	return result;
//}

//Gets the last bit set before *from*
inline int lastBitSet(BitArray* set, int from){
	register char i;
	register WORD *a, *end;

//#ifdef PROFILING
//	gettimeofday(&start,NULL);
//#endif

//	assert(set && from <= set->size);
	if(from < 0) return -1;
	a = set->data + from/NBITS;
	end = set->data;
	for(i = lastSet(*a,from%NBITS); i < 0 && --a >= end; i = lastSet(*a,NBITS));


//#ifdef PROFILING
//	gettimeofday(&stop,NULL);
//	elapsed = (stop.tv_sec-start.tv_sec)*1000.0;
//	elapsed += (stop.tv_usec-start.tv_usec)/1000.0;
//	fprintf(stderr,"%s elapsed time (%f)\n",__func__,elapsed);
//#endif


	return getIndex(i,a,set->data);
}

/*
inline int firstBitSet(BitArray* set, int from){
	int index;
	int pos;
	WORD mask;
	int nbits;
	assert(set);
	if ((from < 0) || (from >= set->size)) return -1;
	nbits = (sizeof(WORD)<<3);
	for(index = from; index < set->size; index++){
		pos = index/nbits;
		mask = ((WORD)1) << (index%nbits);
		if(set->data[pos] & mask) return index;
	}
	return -1;
}
*/

inline int firstBitSet(BitArray* set, int from){
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

//int main(int argc, char **argv) {
//	BitArray * test = newBitArray(200);
//	BitArray * test2 = newBitArray(200);
//	insertBitArray(test,128);
//	insertBitArray(test,3);
//	insertBitArray(test,7);
//
//	insertBitArray(test2,128);
//	insertBitArray(test2,3);
//	insertBitArray(test2,7);
//	insertBitArray(test2,199);
//
////	fprintf(stdout,"%d\n",compareBitArray(test,test2));
//	fprintf(stdout,"%d\n",lastBitSet(test,0));
////	showBitArray(stdout,test);
//	return 0;
//}
