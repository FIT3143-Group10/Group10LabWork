#ifndef MUTEX_INTQUEUE_H
#define MUTEX_INTQUEUE_H

#define OK 0
#define INVALID_SIZE -1
#define NULL_QUEUE -2
#define QUEUE_FULL -3
#define QUEUE_EMPTY -4
#define QUEUE_OCCUPIED -5

typedef struct IntQueue {
    int SIZE;
    int head;
    int num_el;
    int* items;
} IntQueue;

int queue_init(IntQueue** q, int size);
int queue_free(IntQueue* q);
int queue_append(IntQueue* q, int i);
int queue_pop(IntQueue* q, int* out);
int queue_count_elements(IntQueue* q, int* out);

#endif
