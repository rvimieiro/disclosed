/*
 * carpentor.c
 *
 *  Created on: 16/09/2010
 *      Author: Renato Vimieiro
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <R.h>
#include <Rdefines.h>
#include <Rinternals.h>

#include "bitarray.h"
#include "database.h"
#include "condtable.h"
#include "disclosed.h"
#include "linkedlist.h"

extern long int numberCandidates;
extern long int totalClosed;
//extern unsigned char showTIDs;
extern Database rep;

extern T_LinkedList *LL_RESULT_HEAD;
extern T_LinkedList	*LL_RESULT_TAIL;
extern int PROTECTED_VARS;

static BitArray * __tidset;

extern SEXP NB_FEATURE_DATASET;

/*inline*/ SEXP __convertToRItemset(const int * TTX, const int sizeTTx);
/*inline*/ unsigned char reducible(BitArray *X, BitArray* tidset, int index);
/*inline*/ void reduce(BitArray *dst, BitArray *src, int index);

void carpentor (int minsup, int maxsup){
	BitArray * X;
	int* TTx;
	int i;
	numberCandidates = 0;

	X = newBitArray(rep.objsize);

	TTx = (int*)malloc(rep.size*sizeof(int));
	for(i = 0; i < rep.size; i++) {
		TTx[i] = i;
		bitArrayInPlaceUnion(X,rep.data[i]);
	}

	if(X->nelements >= minsup) traverse(&TTx,i,X,minsup,maxsup,lastBitSet(X,X->size)+1);
	destroyBitArray(X);
	free(TTx);

}

void traverse  (int** TTx, int sizeTTx, BitArray * X, int minsup, int maxsup, int index){
	int i;
	int minfI;
	int* cond;
	int nelem;
	int sizeNewTTx;
	BitArray * tmp;

	/* R check for Ctrl-C to interrupt execution */
	R_CheckUserInterrupt();

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
		T_CondTable * ct = NULL;
		ct = (T_CondTable*)malloc(sizeof(T_CondTable));
		ct->list=NULL;
		ct->size=0;
		ct->support = __tidset->nelements;
		copyArrayToCondTable(ct,*TTx,sizeTTx);
		LL_RESULT_TAIL = insertLinkedList(LL_RESULT_TAIL,ct);
		if(!LL_RESULT_HEAD) LL_RESULT_HEAD = LL_RESULT_TAIL;
		totalClosed++;
	}
	if(!reducible(X,__tidset,index)){
		destroyBitArray(__tidset);
		for(index = lastBitSet(X,index); index > -1; index = lastBitSet(X,index)){
			removeBitArray(X,index);
			if(X->nelements >= minsup && X->nelements >= minfI){
				sizeNewTTx = sizeTTx;
				cond = conditionalTable(*TTx,&sizeNewTTx,X); 
				if(sizeNewTTx > 0){
					traverse(&cond,sizeNewTTx,X,minsup,maxsup,index);
				}
				free(cond);
			}
			insertBitArray(X,index);
		}
	} else{
		reduce(__tidset,X,index);
		if(__tidset->nelements >= minsup)
			traverse(TTx,sizeTTx,__tidset,minsup,maxsup,index);
	}
}

/*inline*/ unsigned char reducible(BitArray *X, BitArray* tidset, int index){
	unsigned char reduce = 0;
	register int i;
	for(i = lastBitSet(X,index); i >= 0 && !reduce; i = lastBitSet(X,i)) reduce = !containsElem(tidset,i);
	return reduce;
}

/*inline*/ void reduce(BitArray *tidset, BitArray *X, int index){
	register int i;
	for(i=firstBitSet(X,index+1); i > 0; i = firstBitSet(X,i+1)) insertBitArray(tidset,i);
}

SEXP /*inline*/ __convertToRItemset(const int * TTx, const int sizeTTx){
	SEXP ans;
	SEXP SUPPORT;
	SEXP ITEMS;
	SEXP aux;
	int i;

	PROTECT(ans=NEW_OBJECT(MAKE_CLASS("itemset")));	//Instantiate one object in R to store the itemset
	PROTECT(SUPPORT = allocVector(INTSXP, 1));
	PROTECT(ITEMS = NEW_OBJECT(MAKE_CLASS("nsparseVector")));
	PROTECTED_VARS+=3;

	INTEGER(SUPPORT)[0] = __tidset->nelements;

	SET_SLOT(ITEMS,mkChar("length"),NB_FEATURE_DATASET); //Sets the size of the vector
														 //Number of elements that could be present

	PROTECT(aux = allocVector(INTSXP, sizeTTx)); //Sets the elements that are present
	PROTECTED_VARS++;
	for(i=0;i<sizeTTx;i++) INTEGER(aux)[i]=TTx[i];		//Copy TTx to a vector in R
	SET_SLOT(ITEMS,mkChar("i"),aux);			//Set it to the proper slot
	UNPROTECT(1);
	PROTECTED_VARS--;

/*TODO What if the user wants the samples where the pattern occur?
 * The class representation in R should allow this.
 * SUPPORT could either hold an integer or an array.
 */

//	if(showTIDs){
//		showBitArray(output,__tidset);
//	}

	SET_SLOT(ans,mkChar("items"),ITEMS);
	SET_SLOT(ans,mkChar("support"),SUPPORT);
	return ans;
}
