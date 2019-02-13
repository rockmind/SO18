#define _XOPEN_SOURCE

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<signal.h>

//konwertuje liczbe zapisana w stringu na long int, jeśli się nie da zwraca błąd konwersji
long int validate_long_digit(char *string){
	int n = strlen(string);
	const char * err_konwersja = "Blad w argumentach, powinny byc dwa argumenty:\n Pierwszy to L - liczba wysylanych sygnalow.\n Drugi to Type - sposob wysy;ania sygnalow (1,2 lub 3). \n";
	if(n>10){
			fprintf(stderr,"%s",err_konwersja);
			exit(EXIT_FAILURE);
	}
	if(n==10){
		char * max_uint = "4294967295";
		for(int i = 0; i < strlen(string); i++){
			if(string[i]<max_uint[i]){
				break;
			}
			if(string[i]>max_uint[i]){
				fprintf(stderr,"%s",err_konwersja);
				exit(EXIT_FAILURE);
			}
		}
	}
	unsigned int t=0;
	unsigned int mult=1;
	for(int i = strlen(string)-1; i >= 0 ; i--){
		if((string[i] >= 48 && string[i] <= 57)){
			t += mult*(string[i]-48);
			mult = 10*mult;
		}
		else{
			fprintf(stderr,"%s",err_konwersja);
			exit(EXIT_FAILURE);
		}
	}
	return t;
}
pid_t pid_potomek;
pid_t pid_rodzic;
int L;
int potomekOdebrane=0;
int rodzic_Odebrane=0;
void odbierzPotomekSIGUSR1(int sygnal){
    if(SIGUSR1==sygnal){potomekOdebrane++;printf("Potomek odebrał sygnał SIGUSR1 %d\n",potomekOdebrane);if(potomekOdebrane<=L){kill(pid_rodzic,SIGUSR1);}}
  //if(SIGUSR1==sygnal){potomekOdebrane++;printf("Potomek odebrał sygnał SIGUSR1 %d\n",potomekOdebrane);                       kill(pid_rodzic,SIGUSR1);}
	if(SIGUSR2==sygnal){potomekOdebrane++;printf("Potomek odebrał %d sygnalow\n",potomekOdebrane);exit(0);raise(SIGKILL);}
	}
void odbierzRodzicSIGUSR1(int sygnal){
	if(SIGUSR1==sygnal){rodzic_Odebrane++;printf("Rodzic  odebrał sygnał SIGUSR1 %d\n",rodzic_Odebrane);kill(pid_potomek,SIGUSR1);}
}

int main(int argc, char **argv) {
	long int Type = validate_long_digit(argv[2]);
	L    = (int) validate_long_digit(argv[1]);
	if(argc != 3 || (Type!=1 && Type!=2 && Type!=3) || L<0){
		fprintf(stderr,"Type:%ld\nL:%d\nargc:%d\n",Type,L,argc);
		fprintf(stderr,"Blad w argumentach, powinny byc dwa argumenty:\n Pierwszy to L - liczba wysylanych sygnalow.\n Drugi to Type - sposob wysy;ania sygnalow (1,2 lub 3). \n");
		exit(EXIT_FAILURE);
	}
	pid_potomek = fork();
	sleep(1);
	if (pid_potomek == 0){
		struct sigaction act1;
		memset(&act1, 0, sizeof(act1));
		act1.sa_handler = odbierzPotomekSIGUSR1;
		sigemptyset(&act1.sa_mask);
		sigaction(SIGUSR1,  &act1, 0);
		pid_rodzic=getppid();
		printf("Dziecko wysyła\n");
		kill(pid_rodzic,SIGUSR1);
		while(1){};
	}
	else{
		struct sigaction act3;
		memset(&act3, 0, sizeof(act3));
		act3.sa_handler = odbierzRodzicSIGUSR1;
		sigemptyset (&act3.sa_mask);
		sigaction(SIGUSR1,  &act3, 0);
		printf("Rodzic wysyła\n");
		kill(pid_potomek,SIGUSR1);
		while(1);
	}
	printf("Koniec PID=%d\n",getpid());
}