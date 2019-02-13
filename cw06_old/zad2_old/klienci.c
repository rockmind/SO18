#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <semaphore.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>

#include <sys/mman.h>
#include <sys/sem.h>


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
	printf("Czas: %ld,%09ld PID=%ld Klient    wchodzi na krzeslo do strzyzenia\n",(long)tp.tv_sec,tp.tv_nsec,(long) mojPid);
	kill(k->barber,SIGUSR2);
}
void poStrzyrzeniu(int sig){
	struct timespec tp;
	clock_gettime(clk_id,&tp);
	printf("Czas: %ld,%09ld PID=%ld Klient    wychodzi po zakonczeniu strzyzenia\n",(long)tp.tv_sec,tp.tv_nsec,(long) mojPid);
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
	//Dolaczania pamieci wspolnej
	int fd = shm_open(name,O_RDWR,  S_IRUSR|S_IWUSR);
	int roz = lseek(fd, 0, SEEK_END) - lseek(fd, 0, SEEK_SET);
	ftruncate(fd, roz);
	k = (struct queue *) mmap(NULL, roz, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

	//Tworzenie semaforow
	semKol=sem_open("semKol",O_RDWR,S_IRUSR|S_IWUSR,1);
	semSen=sem_open("semSen",O_RDWR,S_IRUSR|S_IWUSR,0);
	// semKrze=sem_open("semSen",O_RDWR,S_IRUSR|S_IWUSR,0);
	// exit(1);
	
	
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
	int sen;
	while(S>0){
		sem_wait(semKol);//Czekaj i zajmij kolejke
		sem_getvalue(semSen, &sen);
		if(sen==0 && (k->specialChair)==0)//Sprawdzam czy golibroda spi
		{
			k->specialChair=getpid();
				clock_gettime(clk_id,&tp1);
				printf("Czas: %ld,%09ld PID=%ld Klient    budzi Golibrode %ld\n",(long)tp1.tv_sec,tp1.tv_nsec,(long) mojPid,(long)k->barber);
			
			sem_post(semSen);//Budzenie
			sem_post(semKol);//Odpuszczenie
			pause();//Czeka na sygnal ze moze wejsc na krzeslo do strzyrzenia
			S--;
			pause();//Czeka na sygnal ze strzyzenie zakonczone
			
		}
		else{
			if(queuePut(k)==-1){
					clock_gettime(clk_id,&tp1);
					printf("Czas: %ld,%09ld PID=%ld Klient    wychodzi z powodu braku miejsc w kolejce\n",(long)tp1.tv_sec,tp1.tv_nsec,(long) mojPid);
				sem_post(semKol);//Odpuszczenie
				czekaj();
				continue;
			}
			else{
					clock_gettime(clk_id,&tp1);
					printf("Czas: %ld,%09ld PID=%ld Klient    zajmuje miejsce w kolejce\n",(long)tp1.tv_sec,tp1.tv_nsec,(long) mojPid);
				sem_post(semKol);//Odpuszczenie
				pause();//Czeka na sygnal ze moze wejsc na krzeslo do strzyrzenia
				S--;
				pause();//Czeka na sygnal ze strzyzenie zakonczone
			}
		}
	}
	munmap(k,roz);
	sem_close(semKol);
	sem_close(semSen);
	if(rodzicPid==mojPid){
		waitpid(0,NULL,WNOHANG);
	}
	exit(EXIT_SUCCESS);
}