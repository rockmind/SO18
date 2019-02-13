// #define _XOPEN_SOURCE

	// #define EXIT_FAILURE 1
	// #define EXIT_SUCCESS 0

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
// #include<fcntl.h>
// #include<sys/stat.h>
#include<sys/wait.h>
#include<signal.h>

//konwertuje liczbe zapisana w stringu na long int, jeśli się nie da zwraca błąd konwersji
long int validate_long_digit(char *string){
	int n = strlen(string);
	int i;
	const char * err_konwersja = "Blad w argumentach, powinny byc dwa argumenty:\n Pierwszy to L - liczba wysylanych sygnalow.\n Drugi to Type - sposob wysy;ania sygnalow (1,2 lub 3). \n";
	if(n>10){
			fprintf(stderr,"%s",err_konwersja);
			exit(EXIT_FAILURE);
	}
	if(n==10){
		char * max_uint = "4294967295";
		for(i = 0; i < strlen(string); i++){
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
	for(i = strlen(string)-1; i >= 0 ; i--){
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
long int Type;
pid_t pid_potomek;
pid_t pid_rodzic;
int L;
int potomekOdebraneSIGUSR1=0;
int potomekOdebraneSIGUSR2=0;
int rodzic_WyslaneSIGUSR1=0;
int rodzic_WyslaneSIGUSR2=0;
int rodzic_OdebraneSIGUSR1=0;
void koniec();
void odbierzPotomekSIGUSR1(int sygnal){
		potomekOdebraneSIGUSR1++;
		if(Type==3){kill(pid_rodzic,SIGRTMIN);}
		else       {kill(pid_rodzic,SIGUSR1);}
}
void odbierzPotomekSIGUSR2(int sygnal){
		potomekOdebraneSIGUSR2++;
		if(Type==3){
			printf("Liczba odebranych sygnalow przez potomka: %d\t(w tym:\t%d x SIGRTMIN\ti\t%d x SIGRTMAX)\n",
			        potomekOdebraneSIGUSR1+potomekOdebraneSIGUSR2,
						  potomekOdebraneSIGUSR1,
							potomekOdebraneSIGUSR2
						);}
		else{
			printf("Liczba odebranych sygnalow przez potomka: %d\t(w tym:\t%d x SIGUSR1\ti\t%d x SIGUSR2)\n",
		        potomekOdebraneSIGUSR1+potomekOdebraneSIGUSR2,
					  potomekOdebraneSIGUSR1,
						potomekOdebraneSIGUSR2
					);
		}

		exit(EXIT_SUCCESS);
}
void odbierzRodzicSIGUSR1(int sygnal){
	rodzic_OdebraneSIGUSR1++;
}
void odbierzRodzicSIGINT(int sygnal){
	printf("Odebrano sygnal SIGINT\n");
	koniec();
}
int main(int argc, char **argv) {
	Type = validate_long_digit(argv[2]);
	L    = (int) validate_long_digit(argv[1]);
	if(argc != 3 || (Type!=1 && Type!=2 && Type!=3) || L<0){
		fprintf(stderr,"Type:%ld\nL:%d\nargc:%d\n",Type,L,argc);
		fprintf(stderr,"Blad w argumentach, powinny byc dwa argumenty:\n Pierwszy to L - liczba wysylanych sygnalow.\n Drugi to Type - sposob wysy;ania sygnalow (1,2 lub 3). \n");
		exit(EXIT_FAILURE);
	}
	pid_rodzic=getpid();
	pid_potomek = fork();
	if (pid_potomek != 0){
		// printf("Start rodzic %ld\n",(long) getpid());
		if(Type==3){signal(SIGRTMIN,odbierzRodzicSIGUSR1);}
		else       {signal(SIGUSR1,odbierzRodzicSIGUSR1);}
		signal(SIGINT,odbierzRodzicSIGINT);
		sleep(1);
		int i;
		for(i=0;i<L;i++){
			if(Type==3){rodzic_WyslaneSIGUSR1=rodzic_WyslaneSIGUSR1+1+kill(pid_potomek,SIGRTMIN);}
			else       {rodzic_WyslaneSIGUSR1=rodzic_WyslaneSIGUSR1+1+kill(pid_potomek,SIGUSR1);}
			if(Type==2){pause();}
	  }
		if(Type==3){rodzic_WyslaneSIGUSR2=rodzic_WyslaneSIGUSR2+1+kill(pid_potomek,SIGRTMAX);}
		else       {rodzic_WyslaneSIGUSR2=rodzic_WyslaneSIGUSR2+1+kill(pid_potomek,SIGUSR2);}

	}
	else{
		// printf("Start podtomek %ld, pid_rodzic=%ld\n",(long) getpid(),(long) pid_rodzic);
		struct sigaction  actSIGUSR1;
		struct sigaction  actSIGUSR2;
		actSIGUSR1.sa_handler = odbierzPotomekSIGUSR1;
		actSIGUSR2.sa_handler = odbierzPotomekSIGUSR2;
		actSIGUSR1.sa_flags = 0;
		actSIGUSR2.sa_flags = 0;
		sigfillset(&actSIGUSR1.sa_mask);
    sigfillset(&actSIGUSR2.sa_mask);
		// sigaction(SIGINT, &act, NULL);
		if(Type==3){
			sigaction(SIGRTMIN, &actSIGUSR1, NULL);
			sigaction(SIGRTMAX, &actSIGUSR2, NULL);
			// signal(SIGRTMIN,odbierzPotomekSIGUSR1);
			// signal(SIGRTMAX,odbierzPotomekSIGUSR2);
		}
		else{
			sigaction(SIGUSR1, &actSIGUSR1, NULL);
			sigaction(SIGUSR2, &actSIGUSR2, NULL);
			// signal(SIGUSR1,odbierzPotomekSIGUSR1);
			// signal(SIGUSR2,odbierzPotomekSIGUSR2);
		}
		while (1) {
		}
	}
	koniec();
	exit(EXIT_SUCCESS);
}

void koniec(){
	waitpid(pid_potomek,NULL,0);
	if(Type==3){
		printf("Liczba wyslanych  sygnalow    do potomka: %d\t(w tym:\t%d x SIGRTMIN\ti\t%d x SIGRTMAX)\n",
						rodzic_WyslaneSIGUSR1+rodzic_WyslaneSIGUSR2,
						rodzic_WyslaneSIGUSR1,
						rodzic_WyslaneSIGUSR2
				 );
	}
	else{
		printf("Liczba wyslanych  sygnalow    do potomka: %d\t(w tym:\t%d x SIGUSR1\ti\t%d x SIGUSR2)\n",
						rodzic_WyslaneSIGUSR1+rodzic_WyslaneSIGUSR2,
						rodzic_WyslaneSIGUSR1,
						rodzic_WyslaneSIGUSR2
				 );
	}
	printf("Liczba odebranych sygnalow    od potomka: %d\n",rodzic_OdebraneSIGUSR1);
	exit(EXIT_SUCCESS);
}
