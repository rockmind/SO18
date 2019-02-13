#include <sys/wait.h>

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#include "kolejka.h"
#include "semafory.h"


void wejdzNaKrzeslo(){
	zablokuj(semid);
	kolejkaKrzesloWejdz(k,mojPid);
	printf("Czas: %s PID=%ld Klient    wchodzi na krzeslo do strzyzenia\n"        ,czas(),(long) mojPid);
	odblokuj(semid);
	jestemGotowDoStrzyrzenia(semid);
};
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

	int i;
	pid_t rodzicPid = getpid();
	for(i=0;i<K;i++){
		if(fork()==0){
			break;
		}
	}
	mojPid=getpid();
	int wolne;
	int nr=0;
	while(S>0){
		zablokuj(semid);
		if(kolejkaPusta(k)){
			if(czyspi(semid) && kolejkaKrzesloPuste(k)){
				printf("Czas: %s PID=%ld Klient    budzi Golibrode %ld\n"                     ,czas(),(long) mojPid,(long)k->barber);
				obudz(semid);
				kolejkaKrzesloWejdz(k,mojPid);
				odblokuj(semid);
				czekajNaStrzyzenie1(semid);
				wejdzNaKrzeslo();
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
				wolne=czyJestemNaKrzesle(k,mojPid);
				odblokuj(semid);
				while(wolne){
					zablokuj(semid);
					wolne=czyJestemNaKrzesle(k,mojPid);
					odblokuj(semid);
				};
				czekajNaStrzyzenie2(semid);
				wejdzNaKrzeslo();
			}
		}
		else{
			if(kolejkaWejdz(k)==-1){
				printf("Czas: %s PID=%ld Klient    wychodzi z powodu braku miejsc w kolejce\n",czas(),(long) mojPid);
				odblokuj(semid);
				continue;
			};
			printf("Czas: %s PID=%ld Klient    zajmuje miejsce w kolejce\n"               ,czas(),(long) mojPid);
			wolne=czyJestemNaKrzesle(k,mojPid);
			odblokuj(semid);
			while(wolne){
				zablokuj(semid);
				wolne=czyJestemNaKrzesle(k,mojPid);
				odblokuj(semid);
			};
			czekajNaStrzyzenie2(semid);
			wejdzNaKrzeslo();
		}
		czekajNaKoniecStrzyrzenia(semid);
		zablokuj(semid);
		kolejkaKrzesloZejdz(k);
		odblokuj(semid);
		wychodzeKlient(semid);
		nr++;
		S--;
		printf("Czas: %s PID=%ld Klient    wychodzi po zakonczeniu strzyzenia %d\n"      ,czas(),(long) mojPid,nr);
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