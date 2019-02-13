#include <semaphore.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
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
sem_t *semid[3];
char * name;
size_t roz;
int fd;

char * czas();
void zablokuj(sem_t * semid[]);
void odblokuj(sem_t * semid[]);
void zasnij(sem_t * semid[]);
void obudz(sem_t * semid[]);
int czyspi(sem_t * semid[]);

void czekajNaStrzyzenie(sem_t * semid[]);
void zaproszenieNaStrzyzenie(sem_t * semid[]);

#endif