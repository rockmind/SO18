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
volatile pid_t pid_potomek;
volatile pid_t pid_rodzic;
volatile long int L;
volatile long int Type;
volatile long int odebrane_potomek=0;
volatile long int odebrane_rodzic=0;
volatile long int wyslane_rodzic=0;
void koniec(){
	printf("Liczba wyslanych  sygnalow   do  potomka %ld\n",wyslane_rodzic);
	printf("Liczba odebranych sygnalow   od  potomka %ld\n",odebrane_rodzic);
	exit(EXIT_SUCCESS);
}
void odbierzSygnalPotomek( int sig_no){
		odebrane_potomek++;
		switch(sig_no){
			case SIGUSR1:	kill(pid_rodzic,SIGUSR1);	break;
			case SIGUSR2:	printf("Liczba odebranych sygnalow przez potomka %ld\n",odebrane_potomek); exit(EXIT_SUCCESS);	break;
		}
		if(SIGRTMIN==sig_no){kill(pid_rodzic,SIGUSR1);}
		if(SIGRTMAX==sig_no){exit(EXIT_SUCCESS);}
}
void odbierzSygnalRodzic(int sig_no){
		switch(sig_no){
			case SIGINT:
				if(Type<3){kill(pid_potomek,SIGUSR2);	printf("\b\bOdebrano SIGINT\n");	koniec();}
				else{      kill(pid_potomek,SIGRTMAX);	printf("\b\bOdebrano SIGINT\n");	koniec();}
				break;
			case SIGUSR1:
				odebrane_rodzic++;
				if(Type==2){
					if(wyslane_rodzic<L){wyslane_rodzic=wyslane_rodzic+1+kill(pid_potomek,SIGUSR1);}
					else{wyslane_rodzic=wyslane_rodzic+1+kill(pid_potomek,SIGUSR2);koniec();}
				}
		}
		if(sig_no==SIGRTMIN){odebrane_rodzic++;}
}

int main(int argc, char **argv) {
	Type = validate_long_digit(argv[2]);
	L    = validate_long_digit(argv[1]);
	if(argc != 3 || (Type!=1 && Type!=2 && Type!=3) || L<0){
		fprintf(stderr,"Type:%ld\nL:%ld\nargc:%d\n",Type,L,argc);
		fprintf(stderr,"Blad w argumentach, powinny byc dwa argumenty:\n Pierwszy to L - liczba wysylanych sygnalow.\n Drugi to Type - sposob wysy;ania sygnalow (1,2 lub 3). \n");
		exit(EXIT_FAILURE);
	}
	pid_potomek = fork();
		printf("Potomek pid: %d\n",pid_potomek);
		printf("Potomek pid: %d\n",pid_potomek);
	if (pid_potomek == 0){
		printf("Potomek -> Potomek pid: %d\n",getpid());
		pid_rodzic=getppid();
		printf("PID Rodzica= %d\n",pid_rodzic);
		kill(SIGINT,pid_rodzic);
		if(Type<3){
			signal(SIGUSR1,odbierzSygnalPotomek);
			signal(SIGUSR2,odbierzSygnalPotomek);
		}
		else{
			signal(SIGRTMIN,odbierzSygnalPotomek);
			signal(SIGRTMAX,odbierzSygnalPotomek);
		}
		while(1);
	}
	else{
		printf("Rodzic -> Potomek pid: %d\n",pid_potomek);
		signal(SIGINT ,odbierzSygnalRodzic);
//		 switch (Type) {
//            case 1:
//				signal(SIGUSR1 ,odbierzSygnalRodzic);
//				while(wyslane_rodzic <= L){
//					wyslane_rodzic=wyslane_rodzic+kill(pid_potomek,SIGUSR1)+1;
//				}
//				wyslane_rodzic=wyslane_rodzic+kill(pid_potomek,SIGUSR2)+1;
//				koniec();
//                break;
//            case 2:
//				signal(SIGUSR1 ,odbierzSygnalRodzic);
//				wyslane_rodzic=wyslane_rodzic+kill(pid_potomek,SIGUSR1)+1;
//				while(1);
//                break;
//            case 3:
//				signal(SIGRTMIN ,odbierzSygnalRodzic);
//				while(wyslane_rodzic <= L){
//					wyslane_rodzic=wyslane_rodzic+kill(pid_potomek,SIGRTMIN)+1;
//				}
//				wyslane_rodzic=wyslane_rodzic+kill(pid_potomek,SIGRTMAX)+1;
//				koniec();
//                break;
//          }
	}
    koniec();
}