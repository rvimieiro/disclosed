/*
 * Rutils.c
 *
 *  Created on: 10/08/2012
 *      Author: Renato Vimieiro
 *  
 * CENTRE FOR BIOINFORMATICS, BIOMARKER-DISCOVERY & INFORMATION-BASED MEDICINE
 * THE UNIVERSITY UNIVERSITY OF NEWCASTLE <http://www.newcastle.edu.au/research-centre/cibm/>
 * HUNTER MEDICAL RESEARCH INSTITUTE <http://hmri.com.au/>
 * Kookaburra Circuit 1, New Lambton Heights, NSW 2305, AUSTRALIA
 *
 *
 * This file contains the API between Disclosed and R
 *
 */

//Internal headers
#include "database.h"
#include "bitarray.h"
#include "disclosed.h"
#include "errorHandling.h"
#include "linkedlist.h"

//External headers
#include <stdio.h>
#include <R.h>
#include <Rdefines.h>
#include <Rinternals.h>
#include <R_ext/Parse.h>

//Global variables
#ifdef MODEL2
int mincut;
#endif
long int numberCandidates = 0;
long int totalClosed = 0;
Database rep;

T_LinkedList *LL_RESULT_HEAD = NULL,
			*LL_RESULT_TAIL = NULL;
SEXP NB_FEATURE_DATASET;

int PROTECTED_VARS;

//Local functions
void /*inline*/ __convertDataToDisclosed(int * dataset, SEXP nrows, SEXP ncols);
void /*inline*/ __cleanUpDisclosed(void);
void /*inline*/ __housekeeping(void);

SEXP __R_Call_disclosed(SEXP __dataset, SEXP nrows, SEXP ncols, SEXP minsup, SEXP maxsup){
	SEXP ans;

	int * dataset = INTEGER(__dataset);
	signal(SIGINT,SIG_HANDLER);
	signal(SIGTERM,SIG_HANDLER);
	signal(SIGSEGV,SIG_HANDLER);

	__housekeeping();

	//Populating the rep
	//Rows are samples
	__convertDataToDisclosed(dataset,nrows,ncols);

	int ms = INTEGER(minsup)[0],
		mx = INTEGER(maxsup)[0];
	carpentor(ms,mx);

	ans = convertToRItemsets(LL_RESULT_HEAD,totalClosed);
	//CLEANING UP
	__cleanUpDisclosed();

	return ans;
}


void /*inline*/ __housekeeping(void){
	totalClosed=0;
	LL_RESULT_HEAD=NULL;
	LL_RESULT_TAIL=NULL;
	PROTECTED_VARS=0;
	NB_FEATURE_DATASET=NULL;
}

void /*inline*/ __convertDataToDisclosed(int * dataset, SEXP nrows, SEXP ncols){
	NB_FEATURE_DATASET = ncols;
	rep.size = INTEGER(ncols)[0];
	rep.objsize = INTEGER(nrows)[0];
	rep.data = (BitArray **)malloc(sizeof(BitArray*)*rep.size);
	int feat, obj, index=0;
	for(feat=0; feat < rep.size; feat++){
		rep.data[feat] = newBitArray(rep.objsize);
		for(obj=0; obj < rep.objsize; obj++){
				if(dataset[index++]) insertBitArray(rep.data[feat],obj);
		}
	}
}

void /*inline*/ __cleanUpDisclosed(void){
	int feat;
	for(feat=0; feat < rep.size; feat++){
		free(rep.data[feat]);
		rep.data[feat] = NULL;
	}
	free(rep.data);
	rep.data=NULL;
	rep.size=0;
	rep.objsize=0;
	LL_RESULT_HEAD=LL_RESULT_TAIL=cleanUpLinkedList(LL_RESULT_HEAD);
	UNPROTECT(PROTECTED_VARS);
	PROTECTED_VARS=0;
}
