

#include "semafory.h"
char * czas(){
	char * t = malloc (sizeof (char) * 20);
	struct timespec tp;
	clockid_t clk_id = CLOCK_MONOTONIC;
	clock_gettime(clk_id,&tp);
	sprintf(t, "%ld,%09ld",(long)tp.tv_sec,tp.tv_nsec);
	return t;
}
void zablokuj(int semid){
	struct sembuf ins;ins.sem_num=0;ins.sem_op =-1;ins.sem_flg=0;
	if (semop(semid, &ins, 1) == -1){
		fprintf(stderr,"Blad semaforow w funkcji zablokuj\n");
	};	
}
void odblokuj(int semid){
	struct sembuf ins;ins.sem_num=0;ins.sem_op =1;ins.sem_flg=0;
	if(semop(semid, &ins, 1) == -1){
		fprintf(stderr,"Blad semaforow w funkcji odblokuj\n");
	};
}
void zasnij(int semid){
	semctl(semid,1,SETVAL,1);
}
void obudz(int semid){
	semctl(semid,1,SETVAL,0);
}
int czyspi(int semid){
	int a = semctl(semid,1,GETVAL,0);
	if(a>0){
		return 1;
	}
	return 0;
}
void czekajNaStrzyzenie1(int semid){
	struct sembuf ins;ins.sem_num=2;ins.sem_op =-1;ins.sem_flg=0;
	if(semop(semid, &ins, 1) == -1){
		fprintf(stderr,"Blad semaforow w funkcji czekajNaStrzyzenie1\n");
	};
};
void zaproszenieNaStrzyzenie1(int semid){
	struct sembuf ins;ins.sem_num=2;ins.sem_op =1;ins.sem_flg=0;
	if(semop(semid, &ins, 1) == -1){
		fprintf(stderr,"Blad semaforow w funkcji zaproszenieNaStrzyzenie1\n");
	};
};
void czekajNaStrzyzenie2(int semid){
	struct sembuf ins;ins.sem_num=3;ins.sem_op =-1;ins.sem_flg=0;
	if(semop(semid, &ins, 1) == -1){
		fprintf(stderr,"Blad semaforow w funkcji czekajNaStrzyzenie2\n");
	};
};
void zaproszenieNaStrzyzenie2(int semid){
	struct sembuf ins;ins.sem_num=3;ins.sem_op =1;ins.sem_flg=0;
	if(semop(semid, &ins, 1) == -1){
		fprintf(stderr,"Blad semaforow w funkcji zaproszenieNaStrzyzenie2\n");
	};
};
void czekajAzKlientBedzieGotow(int semid){
	struct sembuf ins;ins.sem_num=4;ins.sem_op =-1;ins.sem_flg=0;
	if(semop(semid, &ins, 1) == -1){
		fprintf(stderr,"Blad semaforow w funkcji czekajAzKlientBedzieGotow\n");
	};
}
void jestemGotowDoStrzyrzenia(int semid){
	struct sembuf ins;ins.sem_num=4;ins.sem_op =1;ins.sem_flg=0;
	if(semop(semid, &ins, 1) == -1){
		fprintf(stderr,"Blad semaforow w funkcji jestemGotowDoStrzyrzenia\n");
	};
}
void czekajNaKoniecStrzyrzenia(int semid){
	struct sembuf ins;ins.sem_num=5;ins.sem_op =-1;ins.sem_flg=0;
	if(semop(semid, &ins, 1) == -1){
		fprintf(stderr,"Blad semaforow w funkcji czekajNaKoniecStrzyrzenia\n");
	};
}
void strzyrzenie(int semid){
	struct sembuf ins;ins.sem_num=5;ins.sem_op =1;ins.sem_flg=0;
	if(semop(semid, &ins, 1) == -1){
		fprintf(stderr,"Blad semaforow w funkcji strzyrzenie\n");
	};
}
void czekajAzKlientWyjdzie(int semid){
	struct sembuf ins;ins.sem_num=6;ins.sem_op =-1;ins.sem_flg=0;
	if(semop(semid, &ins, 1) == -1){
		fprintf(stderr,"Blad semaforow w funkcji czekajNaKoniecStrzyrzenia\n");
	};
}
void wychodzeKlient(int semid){
	struct sembuf ins;ins.sem_num=6;ins.sem_op =1;ins.sem_flg=0;
	if(semop(semid, &ins, 1) == -1){
		fprintf(stderr,"Blad semaforow w funkcji czekajNaKoniecStrzyrzenia\n");
	};
}