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

City popMin(PriorityQueue queue);

void decreasePriority(PriorityQueue queue, int position, int val);

bool insertPriorityQueue(PriorityQueue queue, City city, int key);

#endif //DROGI_PRIORITY_QUEUE_H
