#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include "semafory.h"
char * czas(){
	char * t = malloc (sizeof (char) * 20);
	struct timespec tp;
	clockid_t clk_id = CLOCK_MONOTONIC;
	clock_gettime(clk_id,&tp);
	sprintf(t, "%ld,%09ld",(long)tp.tv_sec,tp.tv_nsec);
	return t;
}
void zablokuj(int semid){
	struct sembuf ins;
	ins.sem_num=0;
	ins.sem_op =-1;
	ins.sem_flg=0;
	if (semop(semid, &ins, 1) == -1){
		printf("EEEEEEEEEEEEEEEEEEEEEEEEEEEeee\n");
	};	
}
void odblokuj(int semid){
	struct sembuf ins;
	ins.sem_num=0;
	ins.sem_op =1;
	ins.sem_flg=0;
	if(semop(semid, &ins, 1) == -1){
		printf("EBBBBBBBBBBBBBBBBEEEEEEEeee\n");
	};
}
void zasnij(int semid){
	semctl(semid,1,SETVAL,1);
}
void obudz(int semid){
	semctl(semid,1,SETVAL,0);
}
int czyspi(int semid){
	int a = semctl(semid,1,GETVAL,0);
	if(a>0){
		return 1;
	}
	return 0;
}