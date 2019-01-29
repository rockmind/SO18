#ifndef SEMAFORY
#define SEMAFORY



#define KOL_ID 0
#define SEN_ID 1
#define SE2_ID 2
#define CZEKAJ 0
#define DODAJ  -1
#define ODEJM 1



char * czas();
void zablokuj(int semid);
void odblokuj(int semid);
void zasnij(int semid);
void obudz(int semid);
int czyspi(int semid);
#endif