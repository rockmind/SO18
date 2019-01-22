#ifndef KOLEJKA
#define KOLEJKA

struct chair{
	int pid;
	struct chair * next;
} chair;

// struct queue{
	// int  maxSize;
	// int busySize;
	// struct chair * first;
// } queue;


struct queue{
	int  maxSize;
	// int busySize;
	int first;
	// struct chair * first;
	struct chair * chairs;
} queue;

// void queueInit( struct queue* q, int size);
void queueInit( struct queue* q,struct chair * k, int size);

int queuePut(struct queue* q, int client);
int queuePop(struct queue* q);
int queueIsEmpty(struct queue* q);

#endif