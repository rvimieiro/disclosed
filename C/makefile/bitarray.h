/*
 * bitarray.h
 *
 *  Created on: 13/09/2010
 *      Author: Renato Vimieiro
 *    Contents: This file contains the bit array used by the algorithms
 *
 */

#ifndef BITARRAY_H_
#define BITARRAY_H_

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <stdint.h> //Contains the definition of uint64_t (fixed width integer)

#define SIZET unsigned short
#define WORD uint64_t
#define WORD_MAX UINT64_MAX
#define NBITS 64

#define SUBSET -1
#define EQUAL 0
#define SUPSET 1
#define DIFFERENT 90
#define SP1 128
#define SB1 129
#define SP2 130
#define SB2 131

#define cleanBitArray(set) memset(set->data,0,set->length*sizeof(WORD))

#define containsElem(set,elem) (set->data[elem/NBITS] & ((WORD)1 << (elem%NBITS))) > 0
#define insertBitArray(set,elem) { if(!containsElem(set,elem)) {set->data[elem/NBITS] |= ((WORD)1 << (elem%NBITS));set->nelements++;}}

#define removeBitArray(set,elem) {set->data[elem/NBITS] &= ~((WORD)1 << (elem%NBITS));set->nelements--;}

typedef struct __bitarray
{
	 SIZET size;
	 SIZET length;
	 SIZET nelements;
	 WORD* data;
} BitArray;

BitArray* newBitArray(SIZET size); /* BitArray Constructor */
BitArray * newFullBitArray(SIZET size);
//inline void insertBitArray(BitArray * set, int elem); /* Insert element into the BitArray */
//void removeBitArray(BitArray * set, int elem); /* Remove element from the BitArray */
BitArray* bitArrayUnion(BitArray * set1, BitArray * set2); /* Return a new set with the result of the union of set1 and set2 */
inline void bitArrayInPlaceUnion (BitArray * set1, BitArray * set2);
#ifdef __cplusplus
extern "C"
#endif
void showBitArray (FILE * output, const BitArray * ba); /* Print BitArray on the FILE */
//int compareBitArray (BitArray * set1, BitArray * set2);
inline int fastCompareSubset (const BitArray * set1, const BitArray * set2);
inline int equalBitArray (BitArray * set1, BitArray * set2);
void destroyBitArray(BitArray* set);
inline int lastBitSet(BitArray* set, int from);
inline int firstBitSet(BitArray* set, int from);
void initializeBitArray(BitArray * container, SIZET size);
void bitArrayToArray(const BitArray * ba, int ** dest);
//inline unsigned short containsElem(BitArray * set, int elem);
inline unsigned char containsElemGreaterThanIndex(BitArray * set1,BitArray *set2, int index);
//inline int countBits(WORD word);
#endif /* BITARRAY_H_ */
