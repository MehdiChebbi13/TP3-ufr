#ifndef FILE_H
#define FILE_H

#include <stdlib.h>

// Opaque pointer for data (can be TrieNode*)
typedef void* Element;

typedef struct _cell {
    Element data;
    struct _cell *next;
} Cell;

typedef struct {
    Cell *head;
    Cell *tail;
} Queue;

// Create an empty queue
Queue* createQueue();

// Enqueue: O(1)
void enqueue(Queue *q, Element e);

// Dequeue: O(1)
Element dequeue(Queue *q);

// Check if empty
int isQueueEmpty(Queue *q);

// Free queue
void freeQueue(Queue *q);

#endif
