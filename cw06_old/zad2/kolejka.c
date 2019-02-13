#include <sys/types.h>
#include "kolejka.h"
#include "semafory.h"

void kolejkaInitcjalizuj(kolejka* q,int maxSize,pid_t barber){
	q->barber=barber;
	q->maxSize = maxSize;
	q->busySize = 0;
	q->specialChair=0;
	q->first=0;
	int i=0;
	while(i<=maxSize){
		q->chairs[i]=0;
		i++;
	}
	// return q;
};
int czyJestemNaKrzesle(kolejka* q,pid_t mypid){
	if(q->specialChair==mypid){
		return 1;
	}
	return 0;
};
int kolejkaWejdz(kolejka* q){
	if( (q->maxSize) > (q->busySize)){
		pid_t client = getpid();
		int *krzesla=q->chairs;
		int miejsce;
		if(q->busySize==0){
			miejsce=0;
			krzesla[miejsce]=client;
			q->first=miejsce;
		}
		else{
			miejsce=(q->busySize+q->first)%((q->maxSize));
			krzesla[miejsce]=client;
		}
		q->busySize++;
		return miejsce;
	}
	return -1;
};

pid_t kolejkaZdejmij(kolejka* q){
	if(q->busySize==0){
		return -1;
	}
	int f=q->first;
	pid_t pid = q->chairs[f];
	q->first=(q->first+1)%q->maxSize;
	q->chairs[f]=0;
	q->busySize--;
	return pid;
};

pid_t kolejkaSprawdz(kolejka* q){
	if(q->busySize==0){
		return -1;
	}
	return q->chairs[q->first];
};
int kolejkaPusta(kolejka* q){
	if(q->busySize>0){
		return 0;
	}
	else{
		return 1;
	}
};
pid_t kolejkaKrzesloSprawdz(kolejka* q){
	return q->specialChair;
};
void kolejkaKrzesloWejdz(kolejka* q,pid_t mypid){
	q->specialChair=mypid;
};
void kolejkaKrzesloZejdz(kolejka* q){
	q->specialChair=0;
};
int   kolejkaKrzesloPuste(kolejka* q){
	if(q->specialChair!=0){
		return 0;
	}
	return 1;
};
pid_t kolejkaKrzesloZdejmij(kolejka* q){
	pid_t pid = q->specialChair;
	q->specialChair=0;
	return pid;
}