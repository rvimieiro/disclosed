/*
 * linkedlist.c
 *
 *  Created on: 15/08/2012
 *      Author: renato
 *  
 * CENTRE FOR BIOINFORMATICS, BIOMARKER-DISCOVERY & INFORMATION-BASED MEDICINE
 * THE UNIVERSITY UNIVERSITY OF NEWCASTLE <http://www.newcastle.edu.au/research-centre/cibm/>
 * HUNTER MEDICAL RESEARCH INSTITUTE <http://hmri.com.au/>
 * Kookaburra Circuit 1, New Lambton Heights, NSW 2305, AUSTRALIA
 */

#include <stdlib.h>
#include <assert.h>
#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>
#include <R_ext/Parse.h>

#include "linkedlist.h"

extern int PROTECTED_VARS;

T_LinkedList* insertLinkedList(T_LinkedList* where, void * elem/*SEXP elem*/){
	T_LinkedList * tmp;
	tmp = (T_LinkedList*)malloc(sizeof(T_LinkedList));
	if(tmp==NULL) {
		cleanUpLinkedList(where);
		error("Ran out of memory");
	}
	tmp->item = elem;
	tmp->next = NULL;
	if(where != NULL) where->next = tmp;
	return tmp;
}

T_LinkedList* cleanUpLinkedList(T_LinkedList* begin){
	if(begin==NULL) return NULL;
	T_LinkedList* tmp;
	do{
		tmp = begin;
		begin = tmp->next;
		tmp->item = deleteCondTable(((T_CondTable*)(tmp->item)));
		tmp->next = NULL;
		free(tmp);
	}while(begin!=NULL);
	return begin;
}

SEXP convertToRList(T_LinkedList* begin /*List storing the patterns*/,
		int size /*Number of elements in the list*/ ){

	SEXP ans;
	PROTECT(ans=allocVector(VECSXP,size));
	PROTECTED_VARS++;
	int i;
	for(i=0; begin != NULL && i < size; begin = begin -> next, i++) SET_VECTOR_ELT(ans,i,begin->item);
	return ans;
}

SEXP convertToRItemsets(T_LinkedList* begin /*List storing the patterns*/,
		int size /*Number of elements in the list*/ ){
	SEXP ans = R_NilValue;
	SEXP cmdSexp,cmdexpr;
	SEXP mrows, mcols;

	//compute length of arrays to build sparseMatrix in R
	int tlength=0;
	int *lengths = (int*)malloc(sizeof(int)*size);
	int i;
	int tmpsize;
	T_LinkedList * tmp=begin;
	for(i=0; tmp != NULL && i < size; tmp = tmp -> next, i++) {
		tmpsize=((T_CondTable*)(tmp->item))->size;
		lengths[i] = tmpsize;
		tlength+=tmpsize;
	}

	//Instantiating arrays of indices of items in itemsets for creating sparseMatrix in R
	PROTECT(mrows=allocVector(INTSXP,tlength));
	PROTECT(mcols=allocVector(INTSXP,tlength));

	//copy elements in the list to the arrays
	int index=0,j;
	tmp =begin;
	/* TODO Store support info in an R array to build
	 * disjunctive_itemsets object in the end.
	 * The support array will be put as the quality measure.
	 */
	for(i=0; tmp != NULL && i < size; tmp = tmp -> next, i++){
		for(j=0; j<lengths[i]; j++){
			INTEGER(mcols)[index]=((T_CondTable*)(tmp->item))->list[j]+1;
			INTEGER(mrows)[index]=i+1;
			index++;
		}
	}

	//Call R to create sparseMatrix
	defineVar(install("mrows"), mrows, R_GlobalEnv);
	defineVar(install("mcols"), mcols, R_GlobalEnv);
	ParseStatus status;
	PROTECT(cmdSexp = allocVector(STRSXP, 1));
	SET_STRING_ELT(cmdSexp, 0, mkChar("sparseMatrix(j=mrows,i=mcols)"));
	cmdexpr = PROTECT(R_ParseVector(cmdSexp, -1, &status, R_NilValue));
	if (status != PARSE_OK) {
	                 UNPROTECT(4);
	                 error("invalid call %s", "sparseMatrix(j=mrows,i=mcols)");
	}

	//Instantiating itemMatrix object in R
	PROTECT(ans = NEW_OBJECT(MAKE_CLASS("itemMatrix")));
	SET_SLOT(ans,install("data"),eval(VECTOR_ELT(cmdexpr, 0), R_GlobalEnv));

	//Some housekeeping
	//freeing auxiliary arrays ...
	free(lengths);

	UNPROTECT(5);
	/* TODO Answer/returned value should belong to class disjunctive_itemsets
	 * and not to class itemMatrix. This allows pretty print in R.
	 */
	return ans;
}

/*inline*/ void copyArrayToCondTable(T_CondTable * ct, const int * TTx, const int size){
	assert(ct!=NULL);
	//ct is empty?
	if(ct->list == NULL) { ct->list = (int*)malloc(sizeof(int)*size); ct->size = size; }
	//ct is not empty, but size is different
	else if (ct->size != size) { ct->list = (int*)realloc(ct->list,sizeof(int)*size); ct->size = size; }
	if(ct->list == NULL) error("Ran out of memory"); //assuring allocation went well
	int i =0;
	for(i=0;i<size;i++) ct->list[i]=TTx[i];
}

/*inline*/ void* deleteCondTable(T_CondTable * ct){
	if(ct == NULL) return NULL;
	free(ct->list);
	ct->list=NULL;
	free(ct);
	return NULL;
}
