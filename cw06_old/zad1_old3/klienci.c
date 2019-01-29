#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

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
	printf("Czas: %ld,%010ld PID=%ld Klient    wchodzi na krzeslo do strzyrzenia\n",(long)tp.tv_sec,tp.tv_nsec,(long) mojPid);
	kill(k->barber,SIGUSR2);
}
void poStrzyrzeniu(int sig){
	struct timespec tp;
	clock_gettime(clk_id,&tp);
	printf("Czas: %ld,%010ld PID=%ld Klient    wychodzi po zakończeniu strzyrzenia\n",(long)tp.tv_sec,tp.tv_nsec,(long) mojPid);
	kill(k->barber,SIGUSR2);
	time_t t1=tp.tv_sec;
	time_t t2=tp.tv_sec+10;
	while(t1<t2){
		clock_gettime(clk_id,&tp);
		t1=tp.tv_sec;
	}
}

int main(int argc, char *argv[]){
	key_t key = ftok("KluczDoZakladu.key", 'K');
	int shmid = shmget(key, 0, 0);
	k = (struct queue *) shmat(shmid, NULL, 0);
	int semid = semget(key, 0, S_IRUSR|S_IWUSR|IPC_CREAT);
	signal(SIGUSR1, doStrzyrzenia);
	signal(SIGUSR2, poStrzyrzeniu);
	int S=10;
	int i;
	pid_t rodzicPid = getpid();
	for(i=0;i<7;i++){
		if(fork()==0){
			break;
		}
	}
	mojPid=getpid();
	struct timespec tp1;
	while(S>0){
	// printf("PID=%ld sem=%d\n",(long)mojPid,semctl(semid,KOL_ID,GETVAL,1));//////////////////////////////////////////////////////////
		czekajZajmijKolejke(semid);//Czekaj i zajmij kolejke
		if(semctl(semid,SEN_ID,GETVAL,0)>0)//Sprawdzam czy golibroda spi, >0 spi
		{
			k->specialChair=getpid();
				clock_gettime(clk_id,&tp1);
				printf("Czas: %ld,%010ld PID=%ld Klient    budzi Golibrode %ld\n",(long)tp1.tv_sec,tp1.tv_nsec,(long) mojPid,(long)k->specialChair);
			obudz(semid);
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
					printf("Czas: %ld,%010ld PID=%ld Klient    zajmuje miejsc w kolejce\n",(long)tp1.tv_sec,tp1.tv_nsec,(long) mojPid);
				zwolnijKolejke(semid);//Zwolnij kolejke
				pause();//Czeka na sygnal ze moze wejsc na krzeslo do strzyrzenia
				S--;
				pause();//Czeka na sygnal ze strzyzenie zakonczone
			}
		}
	}
	if(rodzicPid==mojPid){
		waitpid(0,NULL,WNOHANG);
	}
	exit(EXIT_SUCCESS);
}