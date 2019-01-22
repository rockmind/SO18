
typedef struct chair {
    struct chair * next;
    int client;
} chair;

typedef struct {
  chair * first;
  int maxChairs;
} queue;

void sitDown(queue * kolejka, int client);
int standUp(queue * kolejka);