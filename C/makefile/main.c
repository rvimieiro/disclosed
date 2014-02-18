/*
 * main.c
 *
 *  Created on: 20/09/2010
 *      Author: Renato Vimieiro
 */

#include "disclosed.h"
#include "database.h"
#include "bitarray.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>



#ifdef MODEL2
int mincut;
#endif

#define author "Renato Vimieiro"
#define version "0.9"
#define release "2014-02-18"
#define program "Disclosed"

#define BUFFERSIZE 2<<21
//PrefixTree* FCI;
long int numberCandidates = 0;
long int totalClosed = 0;
Database rep;
int * labelFeatures;
//extern WORD* data;
FILE* output;

int numberAttributes;
unsigned char showTIDs;

struct timeval startglobal, start, stop;
double timespent;


void SIG_HANDLER(int sig){
	gettimeofday(&stop,NULL);
	timespent = (stop.tv_sec-start.tv_sec);
	timespent += (stop.tv_usec-start.tv_usec)/1000000.0;
	fprintf(stderr, "Execution interrupted by signal %d\n", sig);
	fprintf(stderr, "Aborting...\n");
	fprintf(stderr, "Found %ld closed sets out of %ld candidates so far in %4.2f sec\n",totalClosed,numberCandidates,timespent);
	fclose(output);
	exit(sig);
}


void showIntro(){
	fprintf(stderr,"%s\t(GPL)\tversion %s  %s\t%s\n",program,version,release,author);
}

void showUsage(char * com){
	fprintf(stderr,"\nProgram usage:\n%s <input> <minsup> <maxsup> <print transactions> [<output>]\n",com);
	exit(EXIT_FAILURE);
}

void initializeRepository(Database* rep){
	rep->size = 0;
	rep->objsize = 0;
}

int main(int argc, char **argv) {

//int i;

	signal(SIGINT,SIG_HANDLER);
	signal(SIGTERM,SIG_HANDLER);
	signal(SIGABRT,SIG_HANDLER);
	signal(SIGSEGV,SIG_HANDLER);

	showTIDs = 0;

	output = stdout;
	showIntro();
	if(argc < 5) showUsage(argv[0]);
	if(argc > 5) output = fopen(argv[5],"w");
	showTIDs = atoi(argv[4]);
	initializeRepository(&rep);

#ifdef MODEL2
	mincut = atoi(argv[2]);
#endif

	gettimeofday(&startglobal,NULL);
	gettimeofday(&start,NULL);

	readData(argv[1],&rep);

//	fprintf(stderr,"\n==============DEBUG==============\n");
//	fprintf(stderr,"DB Size = %d\n",rep.size);
//	fprintf(stderr,"lastSet 15-16 = %d\n",/*NBITS - __builtin_clzll((0x000000000000000F &*/ ~(-((WORD)1<<64))/*))*/ );
//
//	for(i=0; i < rep.size; i++) { showBitArray(stderr,rep.data[i]); fprintf(stderr,"\n"); } exit(0);
//	fprintf(stderr,"\n===============END===============\n");

	gettimeofday(&stop,NULL);
	numberAttributes = rep.size;


	timespent = (stop.tv_sec-start.tv_sec);
	timespent += (stop.tv_usec-start.tv_usec)/1000000.0;

	fprintf(stderr,"Database read. Time spent [%4.2f (sec)]\n",timespent);

	if(argc>5)	fprintf(stderr,"Printing patterns in [%s]...\n",argv[5]);
	else fprintf(stderr,"Printing patterns... \n");

	gettimeofday(&start,NULL);
	disclosed(output,atoi(argv[2]),atoi(argv[3]));
	gettimeofday(&stop,NULL);

	timespent = (stop.tv_sec-start.tv_sec);
	timespent += (stop.tv_usec-start.tv_usec)/1000000.0;

	fprintf(stderr,"Patterns discovered. Time spent [%4.2f (sec)]\n",timespent);

	fprintf(stderr,"Done.\n");
	gettimeofday(&stop,NULL);

	timespent = (stop.tv_sec-startglobal.tv_sec);
	timespent += (stop.tv_usec-startglobal.tv_usec)/1000000.0;

	fprintf(stderr, "Found %ld closed sets out of %ld candidates in %4.2f sec\n",totalClosed,numberCandidates,timespent);

	return 0;
}
