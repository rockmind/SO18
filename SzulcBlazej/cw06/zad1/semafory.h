#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>

#ifndef SEMAFORY
#define SEMAFORY

pid_t mojPid;

int shmid;
int semid;

char * czas();
void zablokuj(int semid);
void odblokuj(int semid);
void zasnij(int semid);
void obudz(int semid);
int czyspi(int semid);

void czekajNaStrzyzenie1(int semid);
void zaproszenieNaStrzyzenie1(int semid);
void czekajNaStrzyzenie2(int semid);
void zaproszenieNaStrzyzenie2(int semid);
void czekajAzKlientBedzieGotow(int semid);
void jestemGotowDoStrzyrzenia(int semid);
void czekajNaKoniecStrzyrzenia(int semid);
void strzyrzenie(int semid);
void czekajAzKlientWyjdzie(int semid);
void wychodzeKlient(int semid);
#endif