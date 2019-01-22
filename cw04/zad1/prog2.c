#define _XOPEN_SOURCE
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<sys/wait.h>
#include<time.h>
//#include <locale.h>

pid_t pid;

void au(int sig_no) {
    signal(SIGTSTP, au);
    if(sig_no==SIGTSTP){
        if(pid > 0){
            pid_t pid2=pid;
            kill(pid, SIGKILL);
            printf("\r\bOdebrano sygnał SIGTSTP i zakończono proces potomny pid=%d\n",pid2);
            fflush(stdout);
        }
    }
    if(sig_no==SIGINT){
        if(pid > 0){kill(pid, SIGKILL);}
        printf("\b\b                               ");
        fflush(stdout);
        printf("\b\rOdebrano sygnał SIGINT\n");
        exit(EXIT_SUCCESS);
    }
}
int main() {
    struct sigaction act; 
    act.sa_handler = au; 
    sigemptyset(&act.sa_mask); 
    act.sa_flags = 0; 
    while(1){
        pid_t pid = fork();
        if (pid == 0){
            printf("Rozpoczęto proces potomny pid=%d\n",getpid());
            execl("./date-loop.sh","./date-loop.sh",NULL);
        }
        else{
            signal(SIGTSTP, au); 
            sigaction(SIGINT, &act, NULL); 
        }
        waitpid(pid, NULL, 0);
        printf("\b\bZakończono proces potomny pid=%d\n",pid);
    }
    exit(EXIT_SUCCESS);
}