#ifndef KOLEJKA
#define KOLEJKA
#include <sys/types.h>
struct queue{
	int maxSize;
	int busySize;
	int first;
	pid_t barber;
	pid_t specialChair;
	pid_t chairs[];
} queue;

void queueInit( struct queue* q, int maxSize,pid_t barber);

int queuePut(struct queue* q);
pid_t queuePop(struct queue* q);
pid_t queueGet(struct queue* q);
int queueIsEmpty(struct queue* q);

#endif