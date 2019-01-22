#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0
struct msgbuf {
    long mtype;       /* message type, must be > 0 */
    char * mtext;    /* message data */
};
int main(int argc, char **argv){
    const char * klucz = getenv("HOME");
	//printf("%s\n",klucz);
    int sq=msgget(ftok(klucz, 7), IPC_CREAT | 0600);
    struct msgbuf komunikat;
	komunikat.mtype=1;
	char *k1="aaa";
	komunikat.mtext=k1;
	msgsnd(sq, &komunikat, sizeof(komunikat.mtext),IPC_NOWAIT);
	struct msgbuf odebrano;
	msgrcv(sq, &odebrano,  sizeof(komunikat.mtext), 1, 0);
	printf("Komunikat to: %s\n",odebrano.mtext);
    exit(EXIT_SUCCESS);
}
