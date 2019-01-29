#define _GNU_SOURCE
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

clockid_t clk_id = CLOCK_MONOTONIC;


int K;
int P;
int N;
int L;
char * fileName;
char searchTyp;
char printTyp;
int nk;

char ** buffer;
int bufferN;
int descrpNp;
int descrpNk;

pthread_t *thread;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t conditionBufforProducer = PTHREAD_COND_INITIALIZER;
pthread_cond_t conditionBufforKonsumer = PTHREAD_COND_INITIALIZER;


FILE * fp;
char * line = NULL;
size_t len = 0;
ssize_t readBits;



void print(char kto,char * co,char *co2,int nDescrp){
	struct timespec tp;
	clock_gettime(clk_id,&tp);
	if(kto=='P') {printf("Czas: %ld,%09ld ProducentID: %ld Buffor: %d %s%s\n",(long)tp.tv_sec,tp.tv_nsec,pthread_self(),nDescrp,co,co2);}
	if(kto=='K') {printf("Czas: %ld,%09ld  KonsumerID: %ld Buffor: %d %s%s\n",(long)tp.tv_sec,tp.tv_nsec,pthread_self(),nDescrp,co,co2);}
}

void koniec(){
	fprintf(stdout,"Wywolano koniec programu\n");
	fclose(fp);
    if (line){
		free(line);
	}
	int i;
	for(i=0; i<(P+K); ++i){
		if(pthread_equal(pthread_self(), thread[i])){
			continue;
		}
		pthread_cancel(thread[i]);
    }
	pthread_cond_destroy(&conditionBufforProducer); 
	pthread_cond_destroy(&conditionBufforKonsumer); 
	exit(EXIT_SUCCESS);
}
void* producent(){
	while(1){
		pthread_mutex_lock(&mutex);
	    if(bufferN>=N){
			if(printTyp){print('P',"Czekam na miejsca w buforze","",descrpNp);}
			pthread_cond_wait(&conditionBufforProducer,&mutex);
	    }
		else{
			if(printTyp){print('P',"Czytam kolejna linie w pliku.","",descrpNp);}
			readBits = getline(&line, &len, fp);
			if(readBits==-1){
				koniec();
			};
			if(printTyp){print('P',"Wczytalem kolejna linie z plik. Napis: ",line,descrpNp);}
			char *napis=malloc(sizeof(char)*(readBits + 1));
			napis=strcpy(napis,line);
			buffer[descrpNp]=napis;
			if(printTyp){print('P',"Umieściłem napis w buforze w miejscu. Napis: ",napis,descrpNp);}
			descrpNp=(descrpNp+1)%N;
			bufferN++;
			pthread_cond_broadcast(&conditionBufforProducer);
		}
		pthread_mutex_unlock(&mutex);
	}
    return NULL;
}
void* konsumer(){
	while(1){
		pthread_mutex_lock(&mutex);
	    if(bufferN<=0){
			if(printTyp){print('K',"Czekam na wypelnienie bufora","",descrpNk);}
			pthread_cond_wait(&conditionBufforProducer,&mutex);
	    }
		else{
			char * napisK = buffer[descrpNk];
			int sl = sizeof(napisK)-1;
			if( (searchTyp=='=' && sl==L ) || (searchTyp=='>' && sl>L) || (searchTyp=='<' && sl<L) ){
				print('K',"Pobrałem napis z bufora. Napis: ",napisK,descrpNk);
			}
			free(napisK);
			buffer[descrpNk]=NULL;
			descrpNk=(descrpNk+1)%N;
			bufferN--;
			pthread_cond_broadcast(&conditionBufforProducer);
		}
		pthread_mutex_unlock(&mutex);
	}
    return NULL;
}
int main(int argc, char *argv[]){

	if(argc!=2){
		fprintf(stderr,"Blad w argumentach. Powinien byc jeden, wskazujacy sciezke do pliku konfigaracyjnego.\n");
		fprintf(stderr,"Plik konfiuracyjny powinien kolejno zawierac:\n");
		fprintf(stderr,"  K         - liczba konsumentow,\n");
		fprintf(stderr,"  P         - liczba producentow,\n");
		fprintf(stderr,"  N         - liczba miejsc w buforze,\n");
		fprintf(stderr,"  searchTyp - tryb wypisywania przez konsumentow w zaleznosci od L (moze byc >,<,=\n");
		fprintf(stderr,"  L         - parametr,\n");
		fprintf(stderr,"  fileName  - sciezka do pliku z tekstem,\n");
		fprintf(stderr,"  printTyp  - tryb wypisywania informacji: 1 - tryb opisowy, 0 - tryb uproszczony,\n");
		fprintf(stderr,"  nk        - liczba sekunt po jakiej ma zakonczyc program, gdy 0 to czeka na CTRL-C lub koniec czytanego pliku.\n");
		exit(EXIT_FAILURE);
	}
	    
	FILE * configFp;
	char * configLine;
	size_t configLen = 0;
	ssize_t configReadBits;
    configFp = fopen(argv[1], "r");
	if (configFp == NULL){fprintf(stderr,"Blad przy otwarciu pliku konfigaracyjnego %s\n",argv[1]);exit(EXIT_FAILURE);}
	configReadBits = getline(&configLine, &configLen, configFp); if(configReadBits==-1){	fprintf(stderr,"Blad przy czytaniu pliku konfigaracyjnego\n");	exit(EXIT_FAILURE);	};
/*1 */	K         = atoi(configLine); configReadBits = getline(&configLine, &configLen, configFp); if(configReadBits==-1){	fprintf(stderr,"Blad przy czytaniu pliku konfigaracyjnego\n");	exit(EXIT_FAILURE);	};
/*2 */	P         = atoi(configLine); configReadBits = getline(&configLine, &configLen, configFp); if(configReadBits==-1){	fprintf(stderr,"Blad przy czytaniu pliku konfigaracyjnego\n");	exit(EXIT_FAILURE);	};
/*3 */	N         = atoi(configLine); configReadBits = getline(&configLine, &configLen, configFp); if(configReadBits==-1){	fprintf(stderr,"Blad przy czytaniu pliku konfigaracyjnego\n");	exit(EXIT_FAILURE);	};
/*4 */	searchTyp = configLine[0]   ; configReadBits = getline(&configLine, &configLen, configFp); if(configReadBits==-1){	fprintf(stderr,"Blad przy czytaniu pliku konfigaracyjnego\n");	exit(EXIT_FAILURE);	};
/*5 */	L         = atoi(configLine); configReadBits = getline(&configLine, &configLen, configFp); if(configReadBits==-1){	fprintf(stderr,"Blad przy czytaniu pliku konfigaracyjnego\n");	exit(EXIT_FAILURE);	};

/*6 */	fileName=malloc(sizeof(char)*(configReadBits));
    	fileName=strcpy(fileName,configLine); 
    	fileName[configReadBits-1]='\0'; configReadBits = getline(&configLine, &configLen, configFp); if(configReadBits==-1){	fprintf(stderr,"Blad przy czytaniu pliku konfigaracyjnego\n");	exit(EXIT_FAILURE);	};
/*7 */	printTyp  = atoi(configLine);    configReadBits = getline(&configLine, &configLen, configFp); if(configReadBits==-1){	fprintf(stderr,"Blad przy czytaniu pliku konfigaracyjnego\n");	exit(EXIT_FAILURE);	};
/*8*/	nk        = atoi(configLine); 

	
	fclose(configFp);
	
	bufferN=0;
	descrpNp=0;
	descrpNk=0;
	
    fp = fopen(fileName, "r");
    if (fp == NULL){
		fprintf(stderr,"Blad przy otwarciu pliku %s\n",fileName);
        exit(EXIT_FAILURE);
	}
	thread=malloc(sizeof(pthread_t)*(K+P));
	buffer=malloc(sizeof(char*)*N);
	
	signal(SIGALRM,koniec);
	signal(SIGINT,koniec);
	
	int i;
	for(i=P;i<P+K;i++){
		pthread_create(&thread[i], NULL, konsumer, NULL );	
	}
	for(i=0;i<P;i++){
		pthread_create(&thread[i], NULL, producent, NULL );
	}
	
	alarm(nk);
	while(1);
}