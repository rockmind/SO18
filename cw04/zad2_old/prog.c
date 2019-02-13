#define _XOPEN_SOURCE
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/stat.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/wait.h>

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
volatile pid_t pid_potomek=0;
volatile pid_t pid_rodzic=0;
volatile long int L;
volatile long int Type;
volatile long int odebrane_potomek=0;
volatile long int odebrane_rodzic=0;
volatile long int wyslane_rodzic=0;
void koniec(){
	printf("Liczba wyslanych  sygnalow   do  potomka %ld\n",wyslane_rodzic);
	printf("Liczba odebranych sygnalow przez potomka %ld\n",odebrane_potomek);
	printf("Liczba odebranych sygnalow   od  potomka %ld\n",odebrane_rodzic);
	exit(EXIT_SUCCESS);
}
void odbierzSIGINT( int sig_no){kill(pid_potomek, SIGUSR2 );	printf("\b\bOdebrano SIGINT\n");	koniec();}
void odbierzSIGINT2(int sig_no){kill(pid_potomek, SIGRTMAX);	printf("\b\bOdebrano SIGINT\n");	koniec();}

void odbierzSIGUSR1_potomek( int sig_no){odebrane_potomek++;	kill(pid_rodzic,SIGUSR1); printf("Potomek odebral sygnal: %ld\n",odebrane_potomek);}
void odbierzSIGRTMIN_potomek(int sig_no){odebrane_potomek++;	kill(pid_rodzic,SIGRTMIN);}
void odbierzSIGUSR2_potomek( int sig_no){odebrane_potomek++;	exit(EXIT_SUCCESS);}
void odbierzSIGRTMAX_potomek(int sig_no){odebrane_potomek++;	exit(EXIT_SUCCESS);}

void odbierzSIGRTMIN_rodzic3(int sig_no){odebrane_rodzic++;}
void odbierzSIGUSR1_rodzic1( int sig_no){odebrane_rodzic++;}
void odbierzSIGUSR1_rodzic2( int sig_no){
	odebrane_rodzic++;
	if(wyslane_rodzic <= L){
		wyslane_rodzic=wyslane_rodzic+kill(pid_potomek,SIGUSR1)+1;
	}
	else{
		wyslane_rodzic=wyslane_rodzic+kill(pid_potomek,SIGUSR2)+1;
		koniec();
	}
}

int main(int argc, char **argv) {
	Type = validate_long_digit(argv[2]);
	L    = validate_long_digit(argv[1]);
	if(argc != 3 || (Type!=1 && Type!=2 && Type!=3) || L<0){
		fprintf(stderr,"Type:%ld\nL:%ld\nargc:%d\n",Type,L,argc);
		fprintf(stderr,"Blad w argumentach, powinny byc dwa argumenty:\n Pierwszy to L - liczba wysylanych sygnalow.\n Drugi to Type - sposob wysy;ania sygnalow (1,2 lub 3). \n");
		exit(EXIT_FAILURE);
	}
    pid_rodzic  = getpid();
	pid_potomek = fork();
		printf("Potomek pid: %d\n",pid_potomek);
	if (pid_potomek == 0){
		printf("Potomek -> Potomek pid: %d\n",getpid());
		 switch (Type) {
            case 1:
				signal(SIGUSR1, odbierzSIGUSR1_potomek);
				signal(SIGUSR2, odbierzSIGUSR2_potomek);
				while(1){};
                break;
//            case 2:
//				signal(SIGUSR1, odbierzSIGUSR1_potomek);
//				signal(SIGUSR2, odbierzSIGUSR2_potomek);
//				while(1){};
//                break;
//            case 3:
//				signal(SIGRTMIN, odbierzSIGRTMIN_potomek);
//				signal(SIGRTMAX, odbierzSIGRTMAX_potomek);
//                break;
//            default:
//				fprintf(stderr,"Blad w drugim argumencie\n");
//				exit(EXIT_FAILURE);
          }
	}
	else{
		printf("Rodzic -> Potomek pid: %d\n",pid_potomek);
		 switch (Type) {
            case 1:
				signal(SIGINT , odbierzSIGINT);
				signal(SIGUSR1, odbierzSIGUSR1_rodzic1);
				while(wyslane_rodzic <= L){
					wyslane_rodzic=wyslane_rodzic+kill(pid_potomek,SIGUSR1)+1;
				}
				wyslane_rodzic=wyslane_rodzic+kill(pid_potomek,SIGUSR2)+1;
//				koniec();
                break;
//            case 2:
//				signal(SIGINT , odbierzSIGINT);
//				signal(SIGUSR1, odbierzSIGUSR1_rodzic2);
//				wyslane_rodzic=wyslane_rodzic+kill(pid_potomek,SIGUSR1)+1;
//				while(1);
//                break;
//            case 3:
//				signal(SIGINT , odbierzSIGINT2);
//				while(wyslane_rodzic <= L){
//					wyslane_rodzic=wyslane_rodzic+kill(pid_potomek,SIGRTMIN)+1;
//				}
//				wyslane_rodzic=wyslane_rodzic+kill(pid_potomek,SIGRTMAX)+1;
//				koniec();
//                break;
            default:
				fprintf(stderr,"Blad w drugim argumencie\n");
				exit(EXIT_FAILURE);
				break;
          }
	}
    exit(EXIT_SUCCESS);
}