#define _XOPEN_SOURCE
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#include<stdio.h>
#include<stdlib.h>
//#include<sys/types.h>
#include<unistd.h>
#include<signal.h>
#include<time.h>
#include <locale.h>

static volatile sig_atomic_t f = 0;

void au(int sig_no) {
    signal(SIGTSTP, au); 
	if(sig_no==SIGTSTP && f==0){
        printf("\rOczekuję na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu.");
        fflush(stdout);
		f=1;
        while(f){};
	}
	else if(sig_no==SIGTSTP && f==1){
        printf("\r                                                                     ");
		fflush(stdout);
		f=0;
	}
	if(sig_no==SIGINT){
		printf("\b\b  ");
        fflush(stdout);
		printf("\nOdebrano sygnał SIGINT\n");
		exit(EXIT_SUCCESS);
	}
}
int main() { 
    time_t sekund;
    struct tm *wsk_strukt;
    char napis[100];
	struct sigaction act; 
	act.sa_handler = au; 
	sigemptyset(&act.sa_mask); 
	act.sa_flags = 0; 
    signal(SIGTSTP, au); 
	sigaction(SIGINT, &act, NULL); 
   // signal(SIGINT, au); 
	
    setlocale (LC_ALL, "pl_PL");   // przystosowanie do warunków polskich
    // setlocale (LC_ALL, "");   // przystosowanie do warunkow lokalnych

    while(1) { 
        time(&sekund);
        wsk_strukt = localtime(&sekund);
        strftime (napis, 100, "Teraz jest %c", wsk_strukt);   // tutaj %c oznacza datę i czas
        printf("\r%s",napis); 
        fflush(stdout);
        sleep(1); 
    } 
    return 0; 
}