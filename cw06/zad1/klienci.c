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

clockid_t clk_id = CLOCK_MONOTONIC;

struct queue * k;
pid_t mojPid;

void czekaj(){
	struct timespec tp;
	time_t t1=tp.tv_sec;
	time_t t2=tp.tv_sec+7;
	while(t1<t2){
		clock_gettime(clk_id,&tp);
		t1=tp.tv_sec;
	}
}

void doStrzyrzenia(int sig){
	struct timespec tp;
	clock_gettime(clk_id,&tp);
	printf("Czas: %ld,%010ld PID=%ld Klient    wchodzi na krzeslo do strzyzenia\n",(long)tp.tv_sec,tp.tv_nsec,(long) mojPid);
	kill(k->barber,SIGUSR2);
}
void poStrzyrzeniu(int sig){
	struct timespec tp;
	clock_gettime(clk_id,&tp);
	printf("Czas: %ld,%010ld PID=%ld Klient    wychodzi po zakonczeniu strzyzenia\n",(long)tp.tv_sec,tp.tv_nsec,(long) mojPid);
	kill(k->barber,SIGUSR2);
	time_t t1=tp.tv_sec;
	time_t t2=tp.tv_sec+10;
	while(t1<t2){
		clock_gettime(clk_id,&tp);
		t1=tp.tv_sec;
	}
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
	int K=myAtoi(argv[1]);
	
	key_t key = ftok("KluczDoZakladu.key", 'K');
	int shmid = shmget(key, 0, 0);
	k = (struct queue *) shmat(shmid, NULL, 0);
	int semid = semget(key, 0, S_IRUSR|S_IWUSR);
	if(semid==-1){
		fprintf(stderr,"Nie udalo sie wczytac smaforow\n");
		exit(EXIT_FAILURE);
	}
	signal(SIGUSR1, doStrzyrzenia);
	signal(SIGUSR2, poStrzyrzeniu);

	int i;
	pid_t rodzicPid = getpid();
	for(i=0;i<K;i++){
		if(fork()==0){
			break;
		}
	}
	mojPid=getpid();
	struct timespec tp1;
	while(S>0){
		czekajZajmijKolejke(semid);//Czekaj i zajmij kolejke
		if(semctl(semid,SEN_ID,GETVAL,0)>0 && (k->specialChair)==0)//Sprawdzam czy golibroda spi, >0 spi
		{
			k->specialChair=getpid();
				clock_gettime(clk_id,&tp1);
				printf("Czas: %ld,%010ld PID=%ld Klient    budzi Golibrode %ld\n",(long)tp1.tv_sec,tp1.tv_nsec,(long) mojPid,(long)k->barber);
			
			kill(k->barber,SIGUSR1);
			zwolnijKolejke(semid);//Zwolnij kolejke
			pause();//Czeka na sygnal ze moze wejsc na krzeslo do strzyrzenia
			S--;
			pause();//Czeka na sygnal ze strzyzenie zakonczone
			
		}
		else{
			if(queuePut(k)==-1){
					clock_gettime(clk_id,&tp1);
					printf("Czas: %ld,%010ld PID=%ld Klient    wychodzi z powodu braku miejsc w kolejce\n",(long)tp1.tv_sec,tp1.tv_nsec,(long) mojPid);
				zwolnijKolejke(semid);//Zwolnij kolejke
				czekaj();
				continue;
			}
			else{
					clock_gettime(clk_id,&tp1);
					printf("Czas: %ld,%010ld PID=%ld Klient    zajmuje miejsce w kolejce\n",(long)tp1.tv_sec,tp1.tv_nsec,(long) mojPid);
				zwolnijKolejke(semid);//Zwolnij kolejke
				pause();//Czeka na sygnal ze moze wejsc na krzeslo do strzyrzenia
				S--;
				pause();//Czeka na sygnal ze strzyzenie zakonczone
			}
		}
	}
	shmdt(k);
	if(rodzicPid==mojPid){
		waitpid(0,NULL,WNOHANG);
	}
	exit(EXIT_SUCCESS);
}