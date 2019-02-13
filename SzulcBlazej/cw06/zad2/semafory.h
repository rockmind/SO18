#include <semaphore.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#ifndef SEMAFORY
#define SEMAFORY

/*
Semafor 0 - blokada sekcji krytycznej
Semafor 1 - sen golibrody
Semafor 2 - czekanie na strzyrzenie
*/

union sigval emptyValue;

pid_t mojPid;
sem_t *semid[6];
char * name;
size_t roz;
int fd;

char * czas();
void zablokuj(sem_t * semid[]);
void odblokuj(sem_t * semid[]);
void zasnij(sem_t * semid[]);
void obudz(sem_t * semid[]);
int czyspi(sem_t * semid[]);

void czekajNaStrzyzenie1(sem_t * semid[]);
void zaproszenieNaStrzyzenie1(sem_t * semid[]);
void czekajNaStrzyzenie2(sem_t * semid[]);
void zaproszenieNaStrzyzenie2(sem_t * semid[]);
void czekajAzKlientBedzieGotow(sem_t * semid[]);
void jestemGotowDoStrzyrzenia(sem_t * semid[]);
void czekajNaKoniecStrzyrzenia(sem_t * semid[]);
void strzyrzenie(sem_t * semid[]);
void czekajAzKlientWyjdzie(sem_t * semid[]);
void wychodzeKlient(sem_t * semid[]);
#endif