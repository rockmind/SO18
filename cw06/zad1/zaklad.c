#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#include "kolejka.h"
#include "semafory.h"


pid_t mojPid;
kolejka * k;
// union sigval emptyValue;


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
	shmdt(k);
	shmctl(shmid,IPC_RMID,NULL);
	semctl(semid,0,IPC_RMID ,0);
	semctl(semid,1,IPC_RMID ,0);
	semctl(semid,2,IPC_RMID ,0);
	semctl(semid,3,IPC_RMID ,0);
	semctl(semid,4,IPC_RMID ,0);
	semctl(semid,5,IPC_RMID ,0);
	semctl(semid,6,IPC_RMID ,0);
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
	key_t key = ftok("KluczDoZakladu.key", 'K');
	shmid = shmget(key, sizeof(kolejka)+(maxSits)*sizeof(int), S_IRUSR|S_IWUSR|IPC_CREAT);
	k = (kolejka *) shmat(shmid, NULL, 0);
	mojPid=getpid();
	kolejkaInitcjalizuj(k,maxSits,mojPid);
	
	//Tworzenie semaforow
	semid = semget(key, 7, S_IRUSR|S_IWUSR|IPC_CREAT);
	semctl(semid,0,SETVAL,1);//0 odpowiada za operacje na kolejce (1 zajeta, 0 wolna)
	semctl(semid,1,SETVAL,0);//1 odpowiada za stan golibrody (1 spi, 0 nie spi)
	semctl(semid,2,SETVAL,0);
	semctl(semid,3,SETVAL,0);
	semctl(semid,4,SETVAL,0);//Klient daje znac ze jest gotow
	semctl(semid,5,SETVAL,0);
	semctl(semid,6,SETVAL,0);
	signal(SIGRTMIN, klientWchodzi);
	signal(SIGRTMAX, budziSie);
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
				odblokuj(semid);//
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