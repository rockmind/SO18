#include <sys/types.h>
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

	//Tworzenie i inicjalizowanie kolejki
	name="kolejka";
	fd = shm_open(name,O_RDWR,  S_IRUSR|S_IWUSR);
	roz = lseek(fd, 0, SEEK_END) - lseek(fd, 0, SEEK_SET);
	ftruncate(fd, roz);
	k = (kolejka *) mmap(NULL, roz, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	
	//Tworzenie semaforow
	semid[0]=sem_open("semKol",O_RDWR,S_IRUSR|S_IWUSR,1);//0 odpowiada za operacje na kolejce (1 zajeta, 0 wolna)
	semid[1]=sem_open("semSen",O_RDWR,S_IRUSR|S_IWUSR,0);//1 odpowiada za stan golibrody (1 spi, 0 nie spi)
	semid[2]=sem_open("semSt1",O_RDWR,S_IRUSR|S_IWUSR,0);
	semid[3]=sem_open("semSt2",O_RDWR,S_IRUSR|S_IWUSR,0);
	semid[4]=sem_open("semKl1",O_RDWR,S_IRUSR|S_IWUSR,0);//Klient daje znac ze jest gotow
	semid[5]=sem_open("semKl2",O_RDWR,S_IRUSR|S_IWUSR,0);
	semid[6]=sem_open("semKl3",O_RDWR,S_IRUSR|S_IWUSR,0);

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
				if(kolejkaWejdz(k)==-1){
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

	munmap(k,roz);
	sem_close(semid[0]);
	sem_close(semid[1]);
	sem_close(semid[2]);
	sem_close(semid[3]);
	sem_close(semid[4]);
	sem_close(semid[5]);
	sem_close(semid[6]);
	if(rodzicPid==mojPid){
		int status = 0;
		pid_t wpid;
		while ((wpid = wait(&status)) > 0);
	}
	return EXIT_SUCCESS;
}