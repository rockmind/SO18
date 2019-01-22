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
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

int main(int argc, char **argv){
    int fd;
    char * myfifo = "ZakladGolibrody.fifo";
    key_t k = ftok("KluczDoZakladu.key", 'K');
    int semid = semget(k,3,IPC_CREAT);
    int Kolejka=1;
    int CzySpi =2;
    int semSen = semctl(semid,CzySpi ,GETVAL,0);
    int semKol = semctl(semid,Kolejka,GETVAL,0);
    pid_t pid = getpid();
    char str[50];
    sprintf(str, "%ld",pid);
//    char * str="dupa tam";
    if(
    fd=open(myfifo,O_WRONLY);
    write(fd, str, strlen(str)+1);
    close(fd);
	exit(EXIT_SUCCESS);
}
