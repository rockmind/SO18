#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <time.h>

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#include "kolejka.h"

int main(int argc, char *argv[]){
    // clockid_t clk_id;
    // clk_id = CLOCK_REALTIME;
	// struct timespec tp1;
	// clock_gettime(clk_id,&tp1);
int maxSits=15;
	key_t key = ftok("KluczDoZakladu.key", 'K');
	// struct queue * kolejka=malloc(sizeof(queue));
	int shmid = shmget(key, sizeof(queue)+maxSits*sizeof(chair), S_IRUSR|S_IWUSR|IPC_CREAT);
	// int shmid = shmget(key, sizeof(queue), S_IRUSR|S_IWUSR|IPC_CREAT);
	struct queue * k = (struct queue *) shmat(shmid, NULL, 0);
	k=malloc(sizeof(queue));
	struct chair * krzesla;
	krzesla= (struct chair *) (&k+1);
	// k->maxSize=1;
	// k->chairs=krzesla;
	queueInit(k,krzesla,maxSits);
	// printf("AAAAAAAA\n");
	queuePut(k,1);
	queuePut(k,2);
	queuePut(k,3);
	queuePut(k,4);
	queuePut(k,5);
	// queuePut(kolejka,3);
	// queuePut(kolejka,4);
	// queuePut(kolejka,5);
	// queuePop(kolejka);
	// queuePop(kolejka);
	// queuePop(kolejka);
	// queuePop(kolejka);
	// queueIsEmpty(kolejka);

	// int i=0;
	// while(1){
		// printf("busySists %d, isEmpty %d\n",k->busySize,queueIsEmpty(k));
		// if(!queueIsEmpty(k)){
			// struct chair * tmpChair = k->first;
			// while(tmpChair){
				// printf("Pozycja %d\n",tmpChair->pid);
				// tmpChair=tmpChair->next;
			// }
		// }
		// clock_gettime(clk_id,&tp1);
		// time_t t1=tp1.tv_sec;
		// time_t t2=tp1.tv_sec+5;
		
		// while(t1<t2){
			// clock_gettime(clk_id,&tp1);
			// t1=tp1.tv_sec;
		// }
		// if(i>50){
			// break;
		// }
		// printf("%d\n",i);
		// i++;
	// };
	shmdt(&shmid);
	exit(EXIT_SUCCESS);
}