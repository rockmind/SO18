#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <time.h>
#include <limits.h>
#include "kolejka.h"
#include "semafory.h"
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0
int main(int argc, char **argv){
    int result;
    struct timespec tp1;
    struct timespec tp2;
    clockid_t clk_id;
    int fd;
    clk_id = CLOCK_REALTIME;
    key_t k = ftok("KluczDoZakladu.key", 'K');
	int liczbaKrzesel=2;
	int s1 = shmget(key, sizeof(queue), S_IRUSR|S_IWUSR|IPC_CREAT); 
	queue * k = (queue *)shmat(s1, NULL, 0);
	
	
	
    int semid = semget(k,3,IPC_CREAT);
    int Kolejka  =1;
    int CzySpi   =2;
    int MaxMiejsc=3;
    semctl(semid,Kolejka  ,SETVAL,0);
    semctl(semid,CzySpi   ,SETVAL,0);
    semctl(semid,MaxMiejsc,SETVAL,5);
    int semSen = semctl(semid,CzySpi ,GETVAL,0);
    int semKol = semctl(semid,Kolejka,GETVAL,0);

	
    while(1){
//      if(semctl(semid,Kolejka,GETVAL,0)){
//        printf("Golibroda: Zasypiam\n");
//        while(semctl(semid,Kolejka,GETVAL,0));
//	printf("Golibroda: Budze sie\n");
//      }
//      fd = open(myfifo, O_RDONLY);
//      read(fd, buffKlient, sizeof(buffKlient));
//      close(fd);
//      printf("Golibroda: Zaprasza %s\n", buffKlient);
//      printf("Golibroda: Rozpoczynam strzyzenie %s\n", buffKlient);
//      clock_gettime(clk_id, &tp1);
//      while(tp2.tv_sec < tp1.tv_sec+5){
//        printf("%lld\r",tp2.tv_sec);
//        clock_gettime(clk_id, &tp2);
//      }
//      printf("Golibroda: Zakonczyl strzyzenie %s\n", buffKlient);
    }
	shmctl(s1, IPC_RMID , NULL);
    exit(EXIT_SUCCESS);
}
