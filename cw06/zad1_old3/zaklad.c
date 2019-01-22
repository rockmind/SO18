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


#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#include "kolejka.h"
#include "semafory.h"

clockid_t clk_id = CLOCK_MONOTONIC;
struct timespec tp1;
pid_t mojPid;
struct queue * k;
int shmid;
int semid;

void strzyrze(){
	time_t t1=tp1.tv_sec;
	time_t t2=tp1.tv_sec+5;
	while(t1<t2){
		clock_gettime(clk_id,&tp1);
		t1=tp1.tv_sec;
	}
}
void klientWchodzi(int sig){
}
void koniec(int sig){
	shmctl(shmid,IPC_RMID,NULL);
	semctl(semid,0,IPC_RMID ,0);
	printf("Odebrano SIGINT\n");
	exit(EXIT_SUCCESS);
}
int main(int argc, char *argv[]){
	
	//Tworzenie i inicjalizowanie kolejki
	int maxSits=1000;
	key_t key = ftok("KluczDoZakladu.key", 'K');
	shmid = shmget(key, sizeof(queue)+(maxSits)*sizeof(int), S_IRUSR|S_IWUSR|IPC_CREAT);
	k = (struct queue *) shmat(shmid, NULL, 0);
	mojPid=getpid();
	queueInit(k,maxSits,mojPid);
	
	//Tworzenie semaforow
	semid = semget(key, 3, S_IRUSR|S_IWUSR|IPC_CREAT);
	semctl(semid,KOL_ID,SETVAL,0);//0 odpowiada za operacje na kolejce (1 zajeta, 0 wolna)
	semctl(semid,SEN_ID,SETVAL,0);//1 odpowiada za stan golibrody (1 spi, 0 nie spi)
	semctl(semid,2,SETVAL,0);//2 stan golibrody
	
	signal(SIGUSR2, klientWchodzi);
	signal(SIGINT, koniec);
	while(1){
		czekajZajmijKolejke(semid);//Czekaj i zajmij kolejke
		// printf("ZAAAA\n");
		if(!queueIsEmpty(k)){
			if(semctl(semid,SEN_ID,GETVAL,1)>0){
				clock_gettime(clk_id,&tp1);
				printf("Czas: %ld,%010ld PID=%ld Golibroda budzi się\n",(long)tp1.tv_sec,tp1.tv_nsec,(long) mojPid);
			}
			int j=0;
			while(j<k->busySize){
				pid_t zaprasza = queueGet(k);
				clock_gettime(clk_id,&tp1);
				printf("Czas: %ld,%010ld PID=%ld Golibroda zaprasza %ld\n",(long)tp1.tv_sec,tp1.tv_nsec,(long) mojPid,(long)zaprasza);
				kill(zaprasza,SIGUSR1);
				pause();
				queuePop(k);
				zwolnijKolejke(semid);//Odpuszczenie kojelki
				clock_gettime(clk_id,&tp1);
				printf("Czas: %ld,%010ld PID=%ld Golibroda rozpozczyna strzyrzenie %ld\n",(long)tp1.tv_sec,tp1.tv_nsec,(long) mojPid,(long)zaprasza);
				strzyrze();
				clock_gettime(clk_id,&tp1);
				printf("Czas: %ld,%010ld PID=%ld Golibroda konczy strzyrzenie %ld\n",(long)tp1.tv_sec,tp1.tv_nsec,(long) mojPid,(long)zaprasza);
			}
		}
		else{
			zasnij(semid);
				clock_gettime(clk_id,&tp1);
				printf("Czas: %ld,%010ld PID=%ld Golibroda zasypia\n",(long)tp1.tv_sec,tp1.tv_nsec,(long) mojPid);
			zwolnijKolejkeCzekaj(semid);
				clock_gettime(clk_id,&tp1);
				printf("Czas: %ld,%010ld PID=%ld Golibroda budzi się\n",(long)tp1.tv_sec,tp1.tv_nsec,(long) mojPid);
			czekajZajmijKolejke(semid);//Czekaj i zajmij kolejke
			pid_t zaprasza = (long) k->specialChair;
			k->specialChair=0;
				// clock_gettime(clk_id,&tp1);
				// printf("Czas: %ld,%010ld PID=%ld Golibroda zaprasza %ld\n",(long)tp1.tv_sec,tp1.tv_nsec,(long) mojPid,(long) zaprasza);
			kill(zaprasza,SIGUSR1);
			zwolnijKolejke(semid);//Odpuszczenie kojelki
			pause(); //Czekaj na sygnak ze klient usiadł
				clock_gettime(clk_id,&tp1);
				printf("Czas: %ld,%010ld PID=%ld Golibroda rozpozczyna strzyrzenie %ld\n",(long)tp1.tv_sec,tp1.tv_nsec,(long) mojPid,(long)zaprasza);
			strzyrze();
				clock_gettime(clk_id,&tp1);
				printf("Czas: %ld,%010ld PID=%ld Golibroda konczy strzyrzenie %ld\n",(long)tp1.tv_sec,tp1.tv_nsec,(long) mojPid,(long)zaprasza);
			kill(zaprasza,SIGUSR2);
			pause();
		}
	};
	shmctl(shmid,IPC_RMID,NULL);
	semctl(semid,0,IPC_RMID ,0);
	semctl(semid,1,IPC_RMID ,0);
	semctl(semid,2,IPC_RMID ,0);
	exit(EXIT_SUCCESS);
}