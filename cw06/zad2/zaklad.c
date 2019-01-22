#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <semaphore.h>

#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/mman.h>


#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#include "kolejka.h"
#include "semafory.h"

clockid_t clk_id = CLOCK_MONOTONIC;
char * name="kolejka";
struct queue * k;
pid_t mojPid;
sem_t * semKol;
sem_t * semSen;
size_t roz;
struct timespec tp1;

void strzyrze(){
	clock_gettime(clk_id,&tp1);
	time_t t1=tp1.tv_sec;
	time_t t2=tp1.tv_sec+5;
	while(t1<t2){
		clock_gettime(clk_id,&tp1);
		t1=tp1.tv_sec;
	}
}
void klientWchodzi(int sig){
}
void budziSie(int sig){
	sem_wait(semSen);
}
void koniec(int sig){
	munmap(k,roz);
	shm_unlink(name);
	sem_close(semKol);
	sem_close(semSen);
	sem_unlink("semKol");
	sem_unlink("semSen");
	printf("Odebrano SIGINT\n");
	exit(EXIT_SUCCESS);
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
		if(argc!=2){
		fprintf(stderr,"Blad w argumentach. Powinien byc jeden, liczb krzesel w poczekalni\n");
		exit(EXIT_FAILURE);
	}
	int maxSits=myAtoi(argv[1]);
	//Tworzenie i inicjalizowanie kolejki
	int fd = shm_open(name,O_CREAT|O_RDWR,  S_IRUSR|S_IWUSR);
	roz=sizeof(queue)+(maxSits)*sizeof(int);
	ftruncate(fd, roz);
	k = (struct queue *) mmap(NULL, roz, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	mojPid=getpid();
	queueInit(k,maxSits,mojPid);

	//Tworzenie semaforow
	semKol=sem_open("semKol",O_CREAT|O_RDWR,S_IRUSR|S_IWUSR,1);
	semSen=sem_open("semSen",O_CREAT|O_RDWR,S_IRUSR|S_IWUSR,0);

	signal(SIGUSR2, klientWchodzi);
	signal(SIGUSR1, budziSie);
	signal(SIGINT, koniec);
	int sen;
	while(1){
		sem_wait(semKol);//Czekaj i zajmij kolejke
		if(!queueIsEmpty(k)){
			sem_getvalue(semSen, &sen);
			if(sen==0){
				clock_gettime(clk_id,&tp1);
				printf("Czas: %ld,%010ld PID=%ld Golibroda budzi sie\n",(long)tp1.tv_sec,tp1.tv_nsec,(long) mojPid);
			}
			int j=0;
			while(j<k->busySize){
				pid_t zaprasza = queueGet(k);
				clock_gettime(clk_id,&tp1);
				printf("Czas: %ld,%010ld PID=%ld Golibroda zaprasza               klienta %ld\n",(long)tp1.tv_sec,tp1.tv_nsec,(long) mojPid,(long)zaprasza);
				kill(zaprasza,SIGUSR1);
				pause();
				queuePop(k);
				sem_post(semKol);//Odpuszczenie
					clock_gettime(clk_id,&tp1);
					printf("Czas: %ld,%010ld PID=%ld Golibroda rozpozczyna strzyzenie klienta %ld\n",(long)tp1.tv_sec,tp1.tv_nsec,(long) mojPid,(long)zaprasza);
				strzyrze();
					clock_gettime(clk_id,&tp1);
					printf("Czas: %ld,%010ld PID=%ld Golibroda konczy strzyzenie      klienta %ld\n",(long)tp1.tv_sec,tp1.tv_nsec,(long) mojPid,(long)zaprasza);
				kill(zaprasza,SIGUSR2);
				pause();
				sem_wait(semKol);//Czekaj i zajmij kolejke
			}
			sem_post(semKol);//Odpuszczenie
		}
		else{
			sem_trywait(semSen);// zasnij
				clock_gettime(clk_id,&tp1);
				printf("Czas: %ld,%010ld PID=%ld Golibroda zasypia\n",(long)tp1.tv_sec,tp1.tv_nsec,(long) mojPid);
			sem_post(semKol);//Odpuszczenie
			sem_wait(semSen);// zasnij
			sem_wait(semKol);//Czekaj i zajmij kolejke
				clock_gettime(clk_id,&tp1);
				printf("Czas: %ld,%010ld PID=%ld Golibroda budzi sie\n",(long)tp1.tv_sec,tp1.tv_nsec,(long) mojPid);
			// sem_wait(semKol);//Czekaj i zajmij kolejke
			pid_t zaprasza = (long) k->specialChair;
			k->specialChair=0;
			kill(zaprasza,SIGUSR1);
			sem_post(semKol);//Odpuszczenie kojelki
			pause(); //Czekaj na sygnak ze klient usiadł
				clock_gettime(clk_id,&tp1);
				printf("Czas: %ld,%010ld PID=%ld Golibroda rozpozczyna strzyzenie klienta %ld\n",(long)tp1.tv_sec,tp1.tv_nsec,(long) mojPid,(long)zaprasza);
			strzyrze();
				clock_gettime(clk_id,&tp1);
				printf("Czas: %ld,%010ld PID=%ld Golibroda konczy strzyzenie      klienta %ld\n",(long)tp1.tv_sec,tp1.tv_nsec,(long) mojPid,(long)zaprasza);
			kill(zaprasza,SIGUSR2);
			pause();
		}
	};
}