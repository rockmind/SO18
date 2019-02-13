// #include <stdlib.h>
// #include <stdio.h>
// #include <time.h>
// #include <signal.h>
// #include <unistd.h>

// #include <sys/types.h>
// #include <semaphore.h>

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
		fprintf(stderr,"Blad semaforow\n");
	};
}
void odblokuj(sem_t * semid[]){
	if(sem_post(semid[0]) == -1){
		fprintf(stderr,"Blad semaforow\n");
	};
}
void zasnij(sem_t * semid[]){
	if(sem_init(semid[1], 0, 1) == -1){
		fprintf(stderr,"Blad semaforow\n");
	};
}
void obudz(sem_t * semid[]){
	if(sem_init(semid[1], 0, 0) == -1){
		fprintf(stderr,"Blad semaforow\n");
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
void czekajNaStrzyzenie(sem_t * semid[]){
	if (sem_wait(semid[2]) == -1){
		fprintf(stderr,"Blad semaforow\n");
	};
	// int a;
	// sem_getvalue(semid[2], &a);
	// if(a==0){
		// if(sem_post(semid[2]) == -1){
			// fprintf(stderr,"Blad semaforow\n");
		// };
	// }
	// return a;
};

void zaproszenieNaStrzyzenie(sem_t * semid[]){
	if (sem_post(semid[2]) == -1){
		fprintf(stderr,"Blad semaforow\n");
	};
};