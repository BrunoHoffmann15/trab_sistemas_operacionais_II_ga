#ifndef QUEUE_H_INCLUDED
#define QUEUE_H_INCLUDED

#include "torta.h"

typedef struct Node
{
  Torta *data;
  struct Node *next;
}node;

typedef struct QueueList
{
    int sizeOfQueue;
    size_t memSize;
    node *head;
    node *tail;
}Queue;

void queueInit(Queue *q, size_t memSize);
int enqueue(Queue *, const Torta *);
void dequeue(Queue *, Torta *);
void queuePeek(Queue *, Torta *);
void clearQueue(Queue *);
int getQueueSize(Queue *);

#endif /* QUEUE_H_INCLUDED */
