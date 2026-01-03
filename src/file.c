#include "file.h"
#include <stdio.h>

Queue* createQueue() {
    Queue *q = (Queue*)malloc(sizeof(Queue));
    if (!q) {
        perror("Failed to create queue");
        exit(EXIT_FAILURE);
    }
    q->head = NULL;
    q->tail = NULL;
    return q;
}

void enqueue(Queue *q, Element e) {
    Cell *new_cell = (Cell*)malloc(sizeof(Cell));
    if (!new_cell) {
        perror("Failed to enqueue");
        exit(EXIT_FAILURE);
    }
    new_cell->data = e;
    new_cell->next = NULL;

    if (q->tail != NULL) {
        q->tail->next = new_cell;
    }
    q->tail = new_cell;
    
    if (q->head == NULL) {
        q->head = new_cell;
    }
}

Element dequeue(Queue *q) {
    if (isQueueEmpty(q)) {
        return NULL; 
    }
    Cell *temp = q->head;
    Element data = temp->data;

    q->head = temp->next;
    if (q->head == NULL) {
        q->tail = NULL;
    }

    free(temp);
    return data;
}

int isQueueEmpty(Queue *q) {
    return (q->head == NULL);
}

void freeQueue(Queue *q) {
    while (!isQueueEmpty(q)) {
        dequeue(q);
    }
    free(q);
}
