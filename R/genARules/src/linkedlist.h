/*
 * linkedlist.h
 *
 *  Created on: 15/08/2012
 *      Author: renato
 *  
 * CENTRE FOR BIOINFORMATICS, BIOMARKER-DISCOVERY & INFORMATION-BASED MEDICINE
 * THE UNIVERSITY UNIVERSITY OF NEWCASTLE <http://www.newcastle.edu.au/research-centre/cibm/>
 * HUNTER MEDICAL RESEARCH INSTITUTE <http://hmri.com.au/>
 * Kookaburra Circuit 1, New Lambton Heights, NSW 2305, AUSTRALIA
 */

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include <R.h>
#include <stdlib.h>
#include <Rinternals.h>
#include <Rdefines.h>

typedef struct __linkedlist{
//	SEXP item;
	void * item;
	struct __linkedlist * next;
} T_LinkedList;

typedef struct __condTableLList{
	int * list;
	int size;
	int support;
} T_CondTable;

T_LinkedList* insertLinkedList(T_LinkedList* where, void * elem/*SEXP elem*/);
T_LinkedList* cleanUpLinkedList(T_LinkedList* begin);
SEXP convertToRList(T_LinkedList* begin, int size);

SEXP convertToRItemsets(T_LinkedList* begin /*List storing the patterns*/,
		int size /*Number of elements in the list*/ );


void copyArrayToCondTable(T_CondTable * ct, const int * TTx, const int size);
void* deleteCondTable(T_CondTable * ct);

#endif /* LINKEDLIST_H_ */
