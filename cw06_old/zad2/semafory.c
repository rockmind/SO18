#include "semafory.h"

char * czas(){
	char * t = malloc (sizeof (char) * 20);
	struct timespec tp;
	clockid_t clk_id = CLOCK_MONOTONIC;
	clock_gettime(clk_id,&tp);
	sprintf(t, "%ld,%09ld",(long)tp.tv_sec,tp.tv_nsec);
	return t;
}
void zablokuj(sem_t * semid[]){
	if (sem_wait(semid[0]) == -1){
		fprintf(stderr,"Blad semaforow w funkcji zablokuj\n");
	};
}
void odblokuj(sem_t * semid[]){
	if(sem_post(semid[0]) == -1){
		fprintf(stderr,"Blad semaforow w funkcji odblokuj\n");
	};
}
void zasnij(sem_t * semid[]){
	if(sem_init(semid[1], 0, 1) == -1){
		fprintf(stderr,"Blad semaforow w funkcji zasnij\n");
	};
}
void obudz(sem_t * semid[]){
	if(sem_init(semid[1], 0, 0) == -1){
		fprintf(stderr,"Blad semaforow w funkcji obudz\n");
	};
}
int czyspi(sem_t * semid[]){
	int a;
	sem_getvalue(semid[1], &a);
	if(a>0){
		return 1;
	}
	return 0;
}
void czekajNaStrzyzenie1(sem_t * semid[]){
	if (sem_wait(semid[2]) == -1){
		fprintf(stderr,"Blad semaforow w funkcji czekajNaStrzyzenie1\n");
	};
};
void zaproszenieNaStrzyzenie1(sem_t * semid[]){
	if (sem_post(semid[2]) == -1){
		fprintf(stderr,"Blad semaforow w funkcji zaproszenieNaStrzyzenie1\n");
	};
};
void czekajNaStrzyzenie2(sem_t * semid[]){
	if (sem_wait(semid[3]) == -1){
		fprintf(stderr,"Blad semaforow w funkcji czekajNaStrzyzenie2\n");
	};
};
void zaproszenieNaStrzyzenie2(sem_t * semid[]){
	if (sem_post(semid[3]) == -1){
		fprintf(stderr,"Blad semaforow w funkcji zaproszenieNaStrzyzenie2\n");
	};
};
void czekajAzKlientBedzieGotow(sem_t * semid[]){
	if (sem_wait(semid[4]) == -1){
		fprintf(stderr,"Blad semaforow w funkcji czekajAzKlientBedzieGotow\n");
	};
}
void jestemGotowDoStrzyrzenia(sem_t * semid[]){
	if (sem_post(semid[4]) == -1){
		fprintf(stderr,"Blad semaforow w funkcji jestemGotowDoStrzyrzenia\n");
	};
}
void czekajNaKoniecStrzyrzenia(sem_t * semid[]){
	if (sem_wait(semid[5]) == -1){
		fprintf(stderr,"Blad semaforow w funkcji czekajNaKoniecStrzyrzenia\n");
	};
}
void strzyrzenie(sem_t * semid[]){
	if (sem_post(semid[5]) == -1){
		fprintf(stderr,"Blad semaforow w funkcji strzyrzenie\n");
	};
}
void czekajAzKlientWyjdzie(sem_t * semid[]){
	if (sem_wait(semid[6]) == -1){
		fprintf(stderr,"Blad semaforow w funkcji czekajNaKoniecStrzyrzenia\n");
	};
}
void wychodzeKlient(sem_t * semid[]){
	if (sem_post(semid[6]) == -1){
		fprintf(stderr,"Blad semaforow w funkcji czekajNaKoniecStrzyrzenia\n");
	};
}