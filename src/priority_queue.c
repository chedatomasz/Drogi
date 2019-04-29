#include "priority_queue.h"
#include <stdlib.h>

#define FIRST_ALLOC_SIZE 4

struct PriorityQueue{
    int* heap;
    City* cityHeap;
    int size;
    int allocatedSize;
};


static int parent(int i) { return (i-1)/2; }

static int left(int i) { return (2*i + 1); }

static int right(int i) { return (2*i + 2); }

static void swap(int* first, int* second){
    int buff = *first;
    *first = *second;
    *second = buff;
}
static void citySwap(City* first, City* second){
    City buff = *first;
    *first = *second;
    *second = buff;
}

PriorityQueue newQueue(){
    PriorityQueue result = malloc(sizeof(struct PriorityQueue));
    if(!result){
        return NULL;
    }
    result->heap = malloc(sizeof(int)*FIRST_ALLOC_SIZE);
    if(!result->heap){
        free(result);
        return NULL;
    }
    result->cityHeap = malloc(sizeof(City)*FIRST_ALLOC_SIZE);
    if(!result->cityHeap){
        free(result->heap);
        free(result);
    }
    result->allocatedSize = FIRST_ALLOC_SIZE;
    result->size = 0;
    return result;
}

bool isEmpty(PriorityQueue queue){
    return (queue->size == 0);
}

static void MinHeapify(PriorityQueue queue, int position){
    int l = left(position);
    int r = right(position);
    int min = position;
    if(l<queue->size && queue->heap[l]<queue->heap[min]){
        min = l;
    }
    if(r<queue->size && queue->heap[r]<queue->heap[min]){
        min = r;
    }
    if(min != position){
        swap(&(queue->heap[position]), &(queue->heap[min]));
        citySwap(&(queue->cityHeap[position]), &(queue->cityHeap[position]));
        MinHeapify(queue, min);
    }
}

City popMin(PriorityQueue queue){
    if(isEmpty(queue)){
        return NULL;
    }
    if(queue->size == 1){
        City answer = queue->cityHeap[0];
        queue->size = 0;
        return answer;
    }
    City answer = queue->cityHeap[0];
    queue->heap[0]=queue->heap[queue->size-1];
    queue->cityHeap[0]=queue->cityHeap[queue->size-1];
    queue->size--;
    MinHeapify(queue, 0);
    return answer;
}

void decreasePriority(PriorityQueue  queue, int position, int val){
    queue->heap[position] = val;
    while (position != 0 && queue->heap[parent(position)] > queue->heap[position])
    {
        swap(&(queue->heap[position]), &(queue->heap[parent(position)]));
        citySwap(&(queue->cityHeap[position]), &(queue->cityHeap[parent(position)]));
        position = parent(position);
    }
}

bool insertPriorityQueue(PriorityQueue queue, City city, int key){
    if (queue->size == queue->allocatedSize)
    {
        int* newHeap = realloc(queue->heap, sizeof(int)*(queue->allocatedSize)*2);
        if(!newHeap){
            return false;
        }
        City* newCityHeap =realloc(queue->cityHeap, sizeof(City)*(queue->allocatedSize)*2);
        if(!newCityHeap){
            free(newHeap);
            return false;
        }
        queue->heap=newHeap;
        queue->cityHeap=newCityHeap;
        queue->allocatedSize*=2;
    }
    queue->size++;
    int position = queue->size - 1;
    queue->heap[position] = key;
    queue->cityHeap[position] = city;

    while (position != 0 && queue->heap[parent(position)] > queue->heap[position]) {
        swap(&(queue->heap[position]), &(queue->heap[parent(position)]));
        citySwap(&(queue->cityHeap[position]), &(queue->cityHeap[parent(position)]));
        position = parent(position);
    }
}


