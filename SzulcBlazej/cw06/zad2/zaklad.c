#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#include "kolejka.h"
#include "semafory.h"



void klientWchodzi(int sig){
}
void budziSie(int sig){
		zablokuj(semid);
	obudz(semid);
		odblokuj(semid);
}
void koniec(int sig){
	if(sig==SIGINT){  printf("\nCzas: %s PID=%ld Golibroda - Odebrano SIGINT\n"                 ,czas(),(long) mojPid);}
	if(sig==SIGTERM){ printf("\nCzas: %s PID=%ld Golibroda - Odebrano SIGTERM\n"                ,czas(),(long) mojPid);}
	munmap(k,roz);
	shm_unlink(name);
	sem_close(semid[0]);
	sem_close(semid[1]);
	sem_close(semid[2]);
	sem_close(semid[3]);
	sem_close(semid[4]);
	sem_close(semid[5]);
	sem_close(semid[6]);
	sem_unlink("semKol");
	sem_unlink("semSen");
	sem_unlink("semSt1");
	sem_unlink("semSt2");
	sem_unlink("semKl1");
	sem_unlink("semKl2");
	sem_unlink("semKl3");
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
	// printf("Czas: %s\n",czas());
	// exit(EXIT_SUCCESS);
	if(argc!=2){
		fprintf(stderr,"Blad w argumentach. Powinien byc jeden, liczb krzesel w poczekalni\n");
		exit(EXIT_FAILURE);
	}
	int maxSits=myAtoi(argv[1]);
	//Tworzenie i inicjalizowanie kolejki
	name="kolejka";
	fd = shm_open(name,O_CREAT|O_RDWR,  S_IRUSR|S_IWUSR);
	roz = sizeof(kolejka)+(maxSits)*sizeof(int);
	ftruncate(fd, roz);
	k = (kolejka *) mmap(NULL, roz, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

	mojPid=getpid();
	kolejkaInitcjalizuj(k,maxSits,mojPid);
	
	//Tworzenie semaforow
	semid[0]=sem_open("semKol",O_CREAT|O_RDWR,S_IRUSR|S_IWUSR,1);//0 odpowiada za operacje na kolejce (1 zajeta, 0 wolna)
	semid[1]=sem_open("semSen",O_CREAT|O_RDWR,S_IRUSR|S_IWUSR,0);//1 odpowiada za stan golibrody (1 spi, 0 nie spi)
	semid[2]=sem_open("semSt1",O_CREAT|O_RDWR,S_IRUSR|S_IWUSR,0);
	semid[3]=sem_open("semSt2",O_CREAT|O_RDWR,S_IRUSR|S_IWUSR,0);
	semid[4]=sem_open("semKl1",O_CREAT|O_RDWR,S_IRUSR|S_IWUSR,0);//Klient daje znac ze jest gotow
	semid[5]=sem_open("semKl2",O_CREAT|O_RDWR,S_IRUSR|S_IWUSR,0);
	semid[6]=sem_open("semKl3",O_CREAT|O_RDWR,S_IRUSR|S_IWUSR,0);


	signal(SIGTERM, koniec);
	signal(SIGINT, koniec);
	int flaga=0;
	while(1){
		zablokuj(semid);
		if(czyspi(semid)){ // Czy Golibroda śpi: Tak
			if(kolejkaKrzesloPuste(k) && kolejkaPusta(k)){
				if(flaga==0){// Czy Golibroda spał wcześniej: Nie
					printf("Czas: %s PID=%ld Golibroda zasypia\n"                           ,czas(),(long) mojPid);
					zasnij(semid);
					flaga=1;
				}
				odblokuj(semid);
				continue;
			}
		}
		else{// Czy Golibroda śpi: Nie
			if(kolejkaKrzesloPuste(k) && kolejkaPusta(k)){
				if(flaga==0){// Czy Golibroda spał wcześniej: Nie
					printf("Czas: %s PID=%ld Golibroda zasypia\n"                           ,czas(),(long) mojPid);
					flaga=1;
				}
				zasnij(semid);
				odblokuj(semid);
				continue;
			}
			else{
				if(flaga==1){// Czy Golibroda spał wcześniej: Tak
					printf("Czas: %s PID=%ld Golibroda budzi sie\n"                         ,czas(),(long) mojPid);
					flaga=0;
				}
			}
		}
		pid_t zaprasza;
		if(kolejkaKrzesloPuste(k)){
			zaprasza=kolejkaZdejmij(k);
			printf("Czas: %s PID=%ld Golibroda zaprasza               klienta %ld\n",czas(),(long) mojPid,(long)zaprasza);
			kolejkaKrzesloWejdz(k,zaprasza);
			zaproszenieNaStrzyzenie2(semid);
		}
		else{
			zaprasza=kolejkaKrzesloZdejmij(k);
			printf("Czas: %s PID=%ld Golibroda zaprasza               klienta %ld\n",czas(),(long) mojPid,(long)zaprasza);
			zaproszenieNaStrzyzenie1(semid);
		}
		odblokuj(semid);
		czekajAzKlientBedzieGotow(semid);
		printf("Czas: %s PID=%ld Golibroda rozpozczyna strzyzenie klienta %ld\n",czas(),(long) mojPid,(long)zaprasza);
		printf("Czas: %s PID=%ld Golibroda konczy strzyzenie      klienta %ld\n",czas(),(long) mojPid,(long)zaprasza);
		strzyrzenie(semid);
		czekajAzKlientWyjdzie(semid);

	};
	exit(EXIT_SUCCESS);
}