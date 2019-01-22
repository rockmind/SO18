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

void zwolnijKolejke(int semid){
	// struct sembuf zasnijG[1];
	// zasnijG[0].sem_op =1;
	// zasnijG[0].sem_num=KOL_ID;
	// zasnijG[0].sem_flg=0;
	// semop(semid, zasnijG, 1);
	 semctl(semid,KOL_ID,SETVAL,0);
}
void czekajZajmijKolejke(int semid){
	struct sembuf czekajSem[2];
	czekajSem[0].sem_op =0;//czekaj az sie zwolni
	czekajSem[0].sem_num=KOL_ID;
	czekajSem[0].sem_flg=0;
	czekajSem[1].sem_op =1 ;//zajmij 
	czekajSem[1].sem_num=KOL_ID;
	czekajSem[1].sem_flg=0;
	semop(semid, czekajSem, 2);//Czekaj i zajmij kolejke	
	
	// printf("Zajmuje kolejke\n");	
	// struct sembuf czekajSem;
	// czekajSem.sem_op =-1 ;//zajmij 
	// czekajSem.sem_num=KOL_ID;
	// czekajSem.sem_flg=0;
	// semop(semid, &czekajSem, 1);//Czekaj i zajmij kolejke	
}
void zwolnijKolejkeCzekaj(int semid){
	struct sembuf zasnijG[2];
	zasnijG[0].sem_op =-1;
	zasnijG[0].sem_num=KOL_ID;
	zasnijG[0].sem_flg=0;
	zasnijG[1].sem_op =0;
	zasnijG[1].sem_num=SEN_ID;
	zasnijG[1].sem_flg=0;
	semop(semid, zasnijG, 2);
	
	// struct sembuf zasnijG[1];
	// zasnijG[0].sem_op =CZEKAJ;
	// zasnijG[0].sem_num=SEN_ID;
	// zasnijG[0].sem_flg=0;
	// semctl(semid,KOL_ID,SETVAL,0);
	// semop(semid, zasnijG, 1);
	
}
void zasnij(int semid){
	semctl(semid,SEN_ID,SETVAL,1);
}
void obudz(int semid){
	semctl(semid,SEN_ID,SETVAL,0);
}