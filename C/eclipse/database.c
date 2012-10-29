/*
 * database.c
 *
 *  Created on: Sep 14, 2010
 *      Author: Renato Vimieiro
 */

#include "database.h"
#include "bitarray.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>


#ifdef MODEL2
extern int mincut;
#endif

extern int * labelFeatures;

void readData(char * input, Database* rep){
	char * buffer;
	char * end;
	char * cur;
	char * aux;
	size_t size;
	int file;
	int attribute;
	int attsize;
	BitArray * tmp;
	int index;

	file = open(input,O_RDONLY);
	if(file == -1){
		fprintf(stderr,"Couldn't open file %s\n",input);
		exit(EXIT_FAILURE);
	}
    size = lseek (file, 0, SEEK_END);
    lseek(file,0,SEEK_CUR);
	buffer = (char*)mmap(0,size+1,PROT_READ|PROT_WRITE,MAP_PRIVATE,file,(off_t)0);

	if (buffer == MAP_FAILED) {
		fprintf(stderr,"Couldn't map file %s\n",input);
		close(file);
		exit(EXIT_FAILURE);
	}

	cur = buffer;
	end = buffer+size;
	*end = 0;
	attribute = 0;
	index = 0;

	aux = next(end,cur);
	attsize = atoi(cur);
	rep->objsize = attsize;
	cur = aux;
	aux = next(end,cur);
	*aux = 0;
	attsize = atoi(cur);
	rep->size = attsize;
	rep->data = (BitArray **)malloc(sizeof(BitArray*)*rep->size);
	labelFeatures = (int *)malloc(sizeof(int)*rep->size);
	cur = ++aux;

	while(*cur && cur != end){
		tmp = newBitArray(rep->objsize);
		while(*cur != '\n' && *cur){
			aux = next(end,cur);
			if(*aux == '\n') {
				*aux = 0;
				insertBitArray(tmp,atoi(cur));
				*aux = '\n';
			} else insertBitArray(tmp,atoi(cur));
			cur = aux;
		}
#ifdef MODEL2
		if(tmp->nelements >= mincut){
			rep->data[index] = tmp;
			labelFeatures[index] = attribute;
			index++;
		}
#else
		rep->data[index] = tmp;
		labelFeatures[index] = attribute;
		index++;
#endif
//		fprintf(stderr,"\n==============DEBUG==============\n");
//		showBitArray(stderr,tmp);
//		fprintf(stderr,"\n===============END===============\n");
		cur++;
		attribute++;
	}
	if(index < rep->size){
		rep->size = index;
		rep->data = (BitArray **)realloc(rep->data,sizeof(BitArray*)*rep->size);
		labelFeatures = (int*)realloc(labelFeatures,sizeof(int)*rep->size);
	}
	munmap(buffer,size);
	close(file);
}

char * next(char * end, char * ptr){
	int state;
	char * cur = ptr;
	char now;
	state = 0;
	while(cur != end){
		now = *cur;
		switch(state){
		case 0:
			if(isdigit(now)) cur++;
			else if(now == ' ' || now == '\t' || now == ',') state = 1;
			else if(now == '\n') state = 2;
			else state = -1;
			break;
		case 1:
			*cur = 0;
			return ++cur;
		case 2:
			return cur;
		default:
			fprintf(stderr,"Error while processing input file. Unrecognized char (%c)\n",*cur);
			exit(EXIT_FAILURE);
		}
	}
	return cur;
}

//int main(int argc, char ** argv){
//	Database d;
//	d.data = newList();
//	d.size = 0;
//	readData("teste.txt",12,&d);
//	printList(stdout,d.data,showBitArray);
//	return 0;
//}
