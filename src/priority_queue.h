//
// Created by chedat on 29.04.19.
//

#include <stdbool.h>
#include "connection_tree.h"

#ifndef DROGI_PRIORITY_QUEUE_H
#define DROGI_PRIORITY_QUEUE_H

typedef struct PriorityQueue* PriorityQueue;
bool isEmpty(PriorityQueue queue);

PriorityQueue newQueue();

void* popMin(PriorityQueue queue);

bool insertPriorityQueue(PriorityQueue queue, void* obj, int key);

void removePriorityQueue(PriorityQueue queue);

int getPriotityQueueSize(PriorityQueue queue);
#endif //DROGI_PRIORITY_QUEUE_H
