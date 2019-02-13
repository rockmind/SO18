#include <sys/types.h>
#include <sys/wait.h>

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#include "kolejka.h"
#include "semafory.h"


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

	//Tworzenie i inicjalizowanie kolejki
	name="kolejka";
	fd = shm_open(name,O_RDWR,  S_IRUSR|S_IWUSR);
	roz = lseek(fd, 0, SEEK_END) - lseek(fd, 0, SEEK_SET);
	ftruncate(fd, roz);
	k = (kolejka *) mmap(NULL, roz, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	
	//Tworzenie semaforow
	semid[0]=sem_open("semKol",O_RDWR,S_IRUSR|S_IWUSR,1);//0 odpowiada za operacje na kolejce (1 zajeta, 0 wolna)
	semid[1]=sem_open("semSen",O_RDWR,S_IRUSR|S_IWUSR,0);//1 odpowiada za stan golibrody (1 spi, 0 nie spi)
	semid[2]=sem_open("semStr",O_RDWR,S_IRUSR|S_IWUSR,0);


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
	if(rodzicPid==mojPid){
		// printf("Czas: %s PID=%ld Klient    JESTEM RODZICEM\n"      ,czas(),(long) mojPid);
	}
	while(S>0){
		zablokuj(semid);
																		// if(debug) printf("Czas: %s PID=%ld Klient ZABLOKOWAŁ 1\n",czas(),(long) mojPid);
																		// if(debug) printf("Czas: %s PID=%ld Klient TEST 1\n",czas(),(long) mojPid);
		// printf("Czas: %s PID=%ld Klient    wchodzi do zakladu\n",czas(),(long) mojPid);
		if(kolejkaPusta(k)){
																		// if(debug) printf("Czas: %s PID=%ld Klient TEST 2\n",czas(),(long) mojPid);
		// printf("Czas: %s PID=%ld Klient    aaaa	\n"      ,czas(),(long) mojPid);
			if(czyspi(semid) && kolejkaKrzesloPuste(k)){
																		// if(debug) printf("Czas: %s PID=%ld Klient TEST 3\n",czas(),(long) mojPid);
				printf("Czas: %s PID=%ld Klient    budzi Golibrode %ld\n"                     ,czas(),(long) mojPid,(long)k->barber);
				obudz(semid);
				kolejkaKrzesloWejdz(k);
																		// if(debug) printf("Czas: %s PID=%ld Klient ODBLOKOWAŁ 1a semval=%d\n",czas(),(long) mojPid,semctl(semid,0,GETVAL,0));
				odblokuj(semid);
																		// if(debug) printf("Czas: %s PID=%ld Klient ODBLOKOWAŁ 1b semval=%d\n",czas(),(long) mojPid,semctl(semid,0,GETVAL,0));
				pause();
				// czekajNaStrzyzenie(semid);
				// wejdzNaKrzeslo();
			}
			else{
				if(kolejkaWejdz(k)==-1){//DO ZMIANY
					printf("Czas: %s PID=%ld Klient    wychodzi z powodu braku miejsc w kolejce\n",czas(),(long) mojPid);
																		// if(debug) printf("Czas: %s PID=%ld Klient ODBLOKOWAŁ 2a semval=%d\n",czas(),(long) mojPid,semctl(semid,0,GETVAL,0));
					odblokuj(semid);
																		// if(debug) printf("Czas: %s PID=%ld Klient ODBLOKOWAŁ 2b semval=%d\n",czas(),(long) mojPid,semctl(semid,0,GETVAL,0));
					czekaj();
					continue;
				}
				else{
					printf("Czas: %s PID=%ld Klient    zajmuje miejsce w kolejce\n"               ,czas(),(long) mojPid);
				}
																		// if(debug) printf("Czas: %s PID=%ld Klient ODBLOKOWAŁ 3a semval=%d\n",czas(),(long) mojPid,semctl(semid,0,GETVAL,0));
				odblokuj(semid);
																		// if(debug) printf("Czas: %s PID=%ld Klient ODBLOKOWAŁ 3b semval=%d\n",czas(),(long) mojPid,semctl(semid,0,GETVAL,0));
				pause();
				
				// czekajNaStrzyzenie(semid);
				// wejdzNaKrzeslo();
			}
		}
		else{
																		// if(debug) printf("Czas: %s PID=%ld Klient TEST 4\n",czas(),(long) mojPid);
			if(kolejkaWejdz(k)==-1){
				printf("Czas: %s PID=%ld Klient    wychodzi z powodu braku miejsc w kolejce\n",czas(),(long) mojPid);
																		// if(debug) printf("Czas: %s PID=%ld Klient ODBLOKOWAŁ 4a semval=%d\n",czas(),(long) mojPid,semctl(semid,0,GETVAL,0));
				odblokuj(semid);
																		// if(debug) printf("Czas: %s PID=%ld Klient ODBLOKOWAŁ 4b semval=%d\n",czas(),(long) mojPid,semctl(semid,0,GETVAL,0));
				czekaj();
				continue;
			};
			printf("Czas: %s PID=%ld Klient    zajmuje miejsce w kolejce\n"               ,czas(),(long) mojPid);
																		// if(debug) printf("Czas: %s PID=%ld Klient ODBLOKOWAŁ 4a semval=%d\n",czas(),(long) mojPid,semctl(semid,0,GETVAL,0));
			odblokuj(semid);
																		// if(debug) printf("Czas: %s PID=%ld Klient ODBLOKOWAŁ 4b semval=%d\n",czas(),(long) mojPid,semctl(semid,0,GETVAL,0));
			pause();
		}
		pause();
		S--;
		printf("Czas: %s PID=%ld Klient    wychodzi po zakonczeniu strzyzenia %d\n"      ,czas(),(long) mojPid,S);
		// printf("Czas: %s PID=%ld Klient    specialChair=%d\n"      ,czas(),(long) mojPid,k->specialChair);
	}

	munmap(k,roz);
	sem_close(semid[0]);
	sem_close(semid[1]);
	sem_close(semid[2]);
	if(rodzicPid==mojPid){
		printf("Czas: %s PID=%ld Klient    JESTEM RODZICEM\n"      ,czas(),(long) mojPid);
		// waitpid(0,NULL,WNOHANG);
		int status = 0;
		pid_t wpid;
		while ((wpid = wait(&status)) > 0);
	}
	// exit(EXIT_SUCCESS);
	return EXIT_SUCCESS;
}