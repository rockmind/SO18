#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include "kolejka.h"
#include "semafory.h"

void queueInit( struct queue* q, int maxSize,pid_t barber){
	q->barber=barber;
	q->maxSize = maxSize;
	q->busySize = 0;
	q->specialChair=0;
	// q->chairs=chairs;
	int i=0;
	while(i<maxSize){
		q->chairs[i]=0;
		i++;
	}
};

int  queuePut(struct queue* q){
	if( (q->maxSize) > (q->busySize)){
		pid_t client = getpid();
		int *k=q->chairs;
		int miejsce;
		if(q->busySize==0){
			miejsce=0;
			k[miejsce]=client;
			q->first=miejsce;
		}
		else{
			miejsce=(q->busySize+q->first)%((q->maxSize));
			k[miejsce]=client;
		}
		q->busySize++;
		return miejsce;
	}
	return -1;
};
pid_t queuePop(struct queue* q){
	if(q->busySize==0){
		return -1;
	}
	int f=q->first;
	pid_t pid = q->chairs[f];
	q->first++;
	q->chairs[f]=0;
	q->busySize--;
	return pid;
};

pid_t queueGet(struct queue* q){
	if(q->busySize==0){
		return -1;
	}
	return q->chairs[q->first];
};
int queueIsEmpty(struct queue* q){
	if(q->busySize>0){
		return 0;
	}
	else{
		return 1;
	}
};