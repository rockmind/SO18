#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/sem.h>


#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#include "kolejka.h"
#include "semafory.h"



kolejka * k;
pid_t mojPid;
union sigval emptyValue;

int shmid;
int semid;
void czekaj(){
	clockid_t clk_id = CLOCK_MONOTONIC;
	struct timespec tp;
	time_t t1=tp.tv_sec;
	time_t t2=tp.tv_sec+2;
	while(t1<t2){
		clock_gettime(clk_id,&tp);
		t1=tp.tv_sec;
	}
}


void wejdzNaKrzeslo(){
	zablokuj(semid);
	kolejkaKrzesloWejdz(k);
	printf("Czas: %s PID=%ld Klient    wchodzi na krzeslo do strzyzenia\n"        ,czas(),(long) mojPid);
	odblokuj(semid);
	sigqueue(k->barber, SIGRTMAX,emptyValue);
};
void zejdzZeKrzeslo(){
	zablokuj(semid);
	kolejkaKrzesloZejdz(k);
	odblokuj(semid);
	sigqueue(k->barber, SIGRTMIN,emptyValue);
}
int myAtoi(char * string){
	char z = string[0];
	int k=atoi(string);
	if(k==0 && (z!='0' || ((int) strlen(string))!= 1)){
		fprintf(stderr,"Blad w argumentach. Pierwszy to liczba klientow, drugi to liczba strzyzen\n");
		exit(EXIT_FAILURE);
	}
	return k;
}
int main(int argc, char *argv[]){
	if(argc!=3){
		fprintf(stderr,"Blad w argumentach. Pierwszy to liczba klientow, drugi to liczba strzyzen\n");
		exit(EXIT_FAILURE);
	}
	int S=myAtoi(argv[2]);
	int K=myAtoi(argv[1])-1;
	
	key_t key = ftok("KluczDoZakladu.key", 'K');
	shmid = shmget(key, 0, 0);
	k = (kolejka *) shmat(shmid, NULL, 0);
	semid = semget(key, 0, S_IRUSR|S_IWUSR);
	if(semid==-1){
		fprintf(stderr,"Nie udalo sie wczytac smaforow\n");
		exit(EXIT_FAILURE);
	}

	struct sigaction act1;
	act1.sa_handler=wejdzNaKrzeslo;
	sigemptyset(&act1.sa_mask);
	// act1.sa_flags = SA_NODEFER;
	sigaction(SIGRTMAX,&act1,NULL);
	// signal(SIGRTMAX, wejdzNaKrzeslo);
	
	struct sigaction act2;
	act2.sa_handler=zejdzZeKrzeslo;
	sigemptyset(&act2.sa_mask);
	// act2.sa_flags = SA_NODEFER;
	sigaction(SIGRTMIN,&act2,NULL);
	// signal(SIGRTMIN, zejdzZeKrzeslo);
	int i;
	pid_t rodzicPid = getpid();
	for(i=0;i<K;i++){
		if(fork()==0){
			break;
		}
	}
	mojPid=getpid();
	while(S>0){
		zablokuj(semid);
		if(kolejkaPusta(k)){
			if(czyspi(semid) && kolejkaKrzesloPuste(k)){
				printf("Czas: %s PID=%ld Klient    budzi Golibrode %ld\n"                     ,czas(),(long) mojPid,(long)k->barber);
				obudz(semid);
				kolejkaKrzesloWejdz(k);
				odblokuj(semid);
				pause();
			}
			else{
				if(kolejkaWejdz(k)==-1){//DO ZMIANY
					printf("Czas: %s PID=%ld Klient    wychodzi z powodu braku miejsc w kolejce\n",czas(),(long) mojPid);
					odblokuj(semid);
					czekaj();
					continue;
				}
				else{
					printf("Czas: %s PID=%ld Klient    zajmuje miejsce w kolejce\n"               ,czas(),(long) mojPid);
				}
				odblokuj(semid);
				pause();
			}
		}
		else{
			if(kolejkaWejdz(k)==-1){
				printf("Czas: %s PID=%ld Klient    wychodzi z powodu braku miejsc w kolejce\n",czas(),(long) mojPid);
				odblokuj(semid);
				czekaj();
				continue;
			};
			printf("Czas: %s PID=%ld Klient    zajmuje miejsce w kolejce\n"               ,czas(),(long) mojPid);
			odblokuj(semid);
			pause();
		}
		pause();
		S--;
		printf("Czas: %s PID=%ld Klient    wychodzi po zakonczeniu strzyzenia %d\n"      ,czas(),(long) mojPid,S);
	}
	shmdt(k);
	if(rodzicPid==mojPid){
		waitpid(0,NULL,WNOHANG);
		int status = 0;
		pid_t wpid;
		while ((wpid = wait(&status)) > 0);
	}
	exit(EXIT_SUCCESS);
}