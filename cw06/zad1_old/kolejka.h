#include "kolejka.h"

int sitDown(int maxChairs,queue * kolejka, int client){
  chair * krzeslo = (chair *) malloc(sizeof(chair));
  krzeslo->client=client;
  chair * tmp = kolejka->first;
  if(maxChairs==0){return -2;}
  if(tmp){
      int i=1;
      while(1){
          if(tmp->next){tmp=tmp->next;i++;}
          else{break;}
      }
      if(i>=maxChairs){return -2;}
      tmp->next=krzeslo;
  }
  else{
      kolejka->first=krzeslo;
  }
  return 2;
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