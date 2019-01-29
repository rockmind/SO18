#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "kolejka.h"


// void queueInit( struct queue* q, int size){
	// q->maxSize = size;
	// q->busySize = 0;
	// q->first=NULL;
	// };

void queueInit( struct queue* q,struct chair *k, int size){
	q->maxSize = size;
	q->busySize = 0;
	q->first=NULL;
	q->chairs=k;
	int i=0;
	while(i<size){
		k[i].pid=0;
		k[i].next=NULL;
		i++;
	}
};

int  queuePut(struct queue* q, int client){
	if( (q->maxSize) > (q->busySize)){
		// int pid = getpid();
		struct chair * krzeslo;
		struct chair * k = q->chairs;
		int i=0;
		while(k[i].pid!=0){
			printf("%d PID = %d\n",i,k[i].pid);
			i++;
		}
		printf("Aaaaa\n");
		krzeslo=k;
		// krzeslo = malloc(sizeof(struct chair));
		krzeslo->pid=client;
		
		struct chair * tmpChair = q->first;
		
		if(q->first){
			while(tmpChair->next){
				tmpChair=tmpChair->next;
			}
			tmpChair->next = krzeslo;
		}
		else{
			q->first = krzeslo;
		}
		q->busySize++;
		return 1;
	}
	return -1;
};
int queuePop(struct queue* q){
	int pid;
	if(q->busySize > 0){
		if(q->first->next){
			struct chair * tmpChair = q->first;
			while(tmpChair->next->next){
				tmpChair=tmpChair->next;
			}
			pid=tmpChair->pid;
			tmpChair->pid=0;
			tmpChair->next=NULL;
			// free(tmpChair->next);
			tmpChair->next=NULL;
		}
		else{
			pid=q->first->pid;
			q->first->pid=0;
			// free(q->first);
			q->first=NULL;
		}
		q->busySize--;
	};
	return pid;
};
int queueIsEmpty(struct queue* q){
	if(q->busySize>0){
		return 0;
	}
	else{
		return 1;
	}
};