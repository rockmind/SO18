#ifndef KOLEJKA
#define KOLEJKA
#include <sys/types.h>
typedef struct kolejka{
	int maxSize;
	int busySize;
	int first;
	pid_t barber;
	pid_t specialChair;
	pid_t chairs[];
} kolejka;

void  kolejkaInitcjalizuj(kolejka * q,int maxSize,pid_t barber);

int   kolejkaWejdz         (kolejka* q);
pid_t kolejkaZdejmij       (kolejka* q);
pid_t kolejkaSprawdz       (kolejka* q);
int   kolejkaPusta         (kolejka* q);
int   kolejkaKrzesloPuste  (kolejka* q);
pid_t kolejkaKrzesloSprawdz(kolejka* q);
pid_t kolejkaKrzesloZdejmij(kolejka* q);
void  kolejkaKrzesloWejdz  (kolejka* q);
void  kolejkaKrzesloZejdz  (kolejka* q);
#endif