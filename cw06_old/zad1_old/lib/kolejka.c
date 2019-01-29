#include <signal.h>
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
#include <sys/wait.h>
#include <time.h>
#include <limits.h>
typedef struct chair {
    struct chair * next;
    int client;
} chair;

typedef struct {
  chair * first;
  int maxChairs;
} queue;

void sitDown(queue * kolejka, int client){
  chair * krzeslo = (chair *) malloc(sizeof(chair));
  krzeslo->client=client;
  chair * tmp = kolejka->first;
  if(tmp){
      while(1){
          if(tmp->next){tmp=tmp->next;i++;}
          else{break;}
      }
      tmp->next=krzeslo;
  }
  else{
      kolejka->first=krzeslo;
  }
}
int standUp(queue * kolejka){
	chair * k =kolejka->first;
	if(k){
		chair * f = kolejka->first;
		int c = kolejka->first->client;
		if(k->next){
			kolejka->first=k->next;
		}
		free(f);
		return c;
	}
	return -1;
}
int main(){
  int liczbaKrzesel=2;
  key_t key = ftok("KluczDoZakladu.key", 'K');
  int s1 = shmget(key, sizeof(queue), S_IRUSR|S_IWUSR|IPC_CREAT); 
  queue * k = (queue *)shmat(s1, NULL, 0);
  printf("AAAAA %d\n",(int *) k);
  k->max=1;
  printf("aaaa\n");
  sitDown(liczbaKrzesel,k,2);
  sitDown(liczbaKrzesel,k,3);
  int st=standUp(k);
  printf("Klient: z%d\n",st);
  sitDown(liczbaKrzesel,k,4);
  chair * tmp;
  tmp = k->first;
  while(1){
        printf("Klient: %d\n",tmp->client);
      if(tmp->next){
        tmp=tmp->next;
      }
      else{break;}
  }
  shmctl(s1, IPC_RMID , NULL);
  return 1;

}
