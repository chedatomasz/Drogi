#include "priority_queue.h"
#include <stdlib.h>

#define FIRST_ALLOC_SIZE 4

struct PriorityQueue{
    int* heap;
    void** objHeap;
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
static void objSwap(void* *first, void* *second){
    void* buff = *first;
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
    result->objHeap = malloc(sizeof(void*)*FIRST_ALLOC_SIZE);
    if(!result->objHeap){
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
        objSwap(&(queue->objHeap[position]), &(queue->objHeap[position]));
        MinHeapify(queue, min);
    }
}

void* popMin(PriorityQueue queue){
    if(isEmpty(queue)){
        return NULL;
    }
    if(queue->size == 1){
        void* answer = queue->objHeap[0];
        queue->size = 0;
        return answer;
    }
    void* answer = queue->objHeap[0];
    queue->heap[0]=queue->heap[queue->size-1];
    queue->objHeap[0]=queue->objHeap[queue->size-1];
    queue->size--;
    MinHeapify(queue, 0);
    return answer;
}


bool insertPriorityQueue(PriorityQueue queue, void* obj, int key){
    if (queue->size == queue->allocatedSize)
    {
        int* newHeap = realloc(queue->heap, sizeof(int)*(queue->allocatedSize)*2);
        if(!newHeap){
            return false;
        }
        void** newObjHeap =realloc(queue->objHeap, sizeof(void*)*(queue->allocatedSize)*2);
        if(!newObjHeap){
            free(newHeap);
            return false;
        }
        queue->heap=newHeap;
        queue->objHeap=newObjHeap;
        queue->allocatedSize*=2;
    }
    queue->size++;
    int position = queue->size - 1;
    queue->heap[position] = key;
    queue->objHeap[position] = obj;

    while (position != 0 && queue->heap[parent(position)] > queue->heap[position]) {
        swap(&(queue->heap[position]), &(queue->heap[parent(position)]));
        objSwap(&(queue->objHeap[position]), &(queue->objHeap[parent(position)]));
        position = parent(position);
    }
    return true;
}

void removePriorityQueue(PriorityQueue queue){
    while(!isEmpty(queue)){
        popMin(queue);
    }
    free(queue->heap);
    free(queue->objHeap);
    free(queue);
}


