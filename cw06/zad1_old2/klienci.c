#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#include "kolejka.h"

int main(int argc, char *argv[]){
	key_t key = ftok("KluczDoZakladu.key", 'K');
	// struct queue * kolejka=malloc(sizeof(queue));
	// int s1 = shmget(key, sizeof(queue), S_IRUSR|S_IWUSR|IPC_CREAT);
	int shmid = shmget(key, 0, 0);
	struct queue * k = (struct queue *) shmat(shmid, NULL, 0);
	queuePut(k,3);
	queuePut(k,4);
	queuePut(k,5);
	while(1){};
}