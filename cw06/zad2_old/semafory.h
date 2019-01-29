#ifndef SEMAFORY
#define SEMAFORY



#define KOL_ID 0
#define SEN_ID 1
#define SE2_ID 2
#define CZEKAJ 0
#define DODAJ  -1
#define ODEJM 1

clockid_t clk_id;
struct timespec tp1;

void zwolnijKolejke(int semid);
void czekajZajmijKolejke(int semid);
void zwolnijKolejkeCzekaj(int semid);
void zasnij(int semid);
void obudz(int semid);
#endif