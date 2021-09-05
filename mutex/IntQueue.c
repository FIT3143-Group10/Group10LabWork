#include <stdlib.h>
#include "IntQueue.h"

int queue_init(IntQueue** q, int size) {
    if (*q) {
        return QUEUE_OCCUPIED;
    }
    if (size <= 0) {
        return INVALID_SIZE;
    }
    *q = malloc(sizeof(IntQueue));
    (**q).SIZE = size;
    (**q).head = 0;
    (**q).num_el = 0;
    (**q).items = malloc(size * sizeof(int));
    return OK;
}

int queue_free(IntQueue* q) {
    if (!q) {
        return NULL_QUEUE;
    }
    free(q->items);
    free(q);
    q = NULL;
    return OK;
}

int queue_append(IntQueue* q, int i) {
    if (!q) {
        return NULL_QUEUE;
    }
    if (q->num_el >= q->SIZE) {
        return QUEUE_FULL;
    }
    q->items[(q->head + q->num_el) % q->SIZE] = i;
    q->num_el++;
    return OK;
}

int queue_pop(IntQueue* q, int* out) {
    if (!q) {
        return NULL_QUEUE;
    }
    if (q->num_el <= 0) {
        return QUEUE_EMPTY;
    }
    *out = q->items[q->head];
    q->head = (q->head + 1) % q->SIZE;
    q->num_el--;
    return OK;
}

int queue_count_elements(IntQueue* q, int* out) {
    if (!q) {
        return NULL_QUEUE;
    }
    *out = q->num_el;
    return OK;
}
