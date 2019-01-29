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
// void czekaj(){
	// struct timespec tp;
	// time_t t1=tp.tv_sec;
	// time_t t2=tp.tv_sec+7;
	// while(t1<t2){
		// clock_gettime(clk_id,&tp);
		// t1=tp.tv_sec;
	// }
// }

// void doStrzyrzenia(int sig){
	// struct timespec tp;
	// clock_gettime(clk_id,&tp);
	// kill(k->barber,SIGUSR2);
// }
// void poStrzyrzeniu(int sig){
	// struct timespec tp;
	// clock_gettime(clk_id,&tp);
	// kill(k->barber,SIGUSR2);
	// time_t t1=tp.tv_sec;
	// time_t t2=tp.tv_sec+10;
	// while(t1<t2){
		// clock_gettime(clk_id,&tp);
		// t1=tp.tv_sec;
	// }
// }
void wejdzNaKrzeslo(){
	kolejkaKrzesloWejdz(k);
	printf("Czas: %s PID=%ld Klient    wchodzi na krzeslo do strzyzenia\n"        ,czas(),(long) mojPid);
	// kill(k->barber,SIGUSR1);
	sigqueue(k->barber, SIGUSR1,emptyValue);
};
void zejdzZeKrzeslo(){
	zablokuj(semid);
	kolejkaKrzesloZejdz(k);
	odblokuj(semid);
	// kill(k->barber,SIGUSR2);
	sigqueue(k->barber, SIGUSR2,emptyValue);
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
	// signal(SIGUSR1, wejdzNaKrzeslo);
	signal(SIGUSR2, zejdzZeKrzeslo);

	struct sigaction act;
	act.sa_handler=wejdzNaKrzeslo;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0; 
	sigaction(SIGUSR1,&act,NULL);
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
		printf("Czas: %s PID=%ld Klient    wchodzi do zakladu\n",czas(),(long) mojPid);
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
				continue;
			};
		}
		pause();
		S--;
		printf("Czas: %s PID=%ld Klient    wychodzi po zakonczeniu strzyzenia\n"      ,czas(),(long) mojPid);
		// printf("Czas: %s PID=%ld Klient    specialChair=%d\n"      ,czas(),(long) mojPid,k->specialChair);
	}
	shmdt(k);
	if(rodzicPid==mojPid){
		waitpid(0,NULL,WNOHANG);
	}
	exit(EXIT_SUCCESS);
}