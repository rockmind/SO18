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

void czekajZajmijKolejke(int semid){
	struct sembuf czekajSem[2];
	czekajSem[0].sem_num=KOL_ID;
	czekajSem[0].sem_op =0;
	czekajSem[0].sem_flg=0;
	czekajSem[1].sem_num=KOL_ID;
	czekajSem[1].sem_op =1;
	czekajSem[1].sem_flg=0;
	if (semop(semid, czekajSem, 2) == -1){
		printf("EEEEEEEEEEEEEEEEEEEEEEEEEEEeee\n");
	};	
}
void zwolnijKolejke(int semid){
	struct sembuf buf;
	buf.sem_num=KOL_ID;
	buf.sem_op =-1;
	buf.sem_flg=0;
	if(semop(semid, &buf, 1) == -1){
		printf("EEEEEEEEEEEEEEEEEEEEEEEEEEEeee\n");
	};
}
void zwolnijKolejkeCzekaj(int semid){
	printf("KOL_ID=%d  SEN_ID=%d\n",semctl(semid,KOL_ID,GETVAL,1),semctl(semid,SEN_ID,GETVAL,1));
	struct sembuf zasnijG[2];
	zasnijG[0].sem_num=KOL_ID;
	zasnijG[0].sem_op =-1;
	zasnijG[0].sem_flg=0;
	// zasnijG[1].sem_num=SEN_ID;
	// zasnijG[1].sem_op =0;
	// zasnijG[1].sem_flg=0;
	if (semop(semid, zasnijG, 1	) == -1){
		printf("EEEEEEEEEEEEEEEEEEEEEEEEEEEeee\n");
	};

	
}
void zasnij(int semid){
	struct sembuf buf;
	buf.sem_num=SEN_ID;
	buf.sem_op =1;
	buf.sem_flg=0;
	if (semop(semid, &buf, 1) == -1){
		printf("EEEEEEEEEEEEEEEEEEEEEEEEEEEeee\n");
	};
}
void obudz(int semid){
	struct sembuf buf;
	buf.sem_num=SEN_ID;
	buf.sem_op =-1;
	buf.sem_flg=0;
	if (semop(semid, &buf, 1) == -1){
		printf("EEEEEEEEEEEEEEEEEEEEEEEEEEEeee\n");
	};
}