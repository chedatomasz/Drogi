/** @file
 * Implementacja klasy oferującej kolejkę priorytetową void*
 *
 * @author Tomasz Cheda <tomasz.cheda@students.mimuw.edu.pl>
 * @date 29.04.2019
 */

#include "priority_queue.h"
#include <stdlib.h>

#define FIRST_ALLOC_SIZE 4///<Liczba początkowo aloowanych elementów

/**
 * Implementacja struktury przechowującej kolejkę priorytetową.
 */
struct PriorityQueue {
    unsigned *heap; ///<Tablica przechowująca klucze
    void **objHeap;///<Tablica przechowująca elementy
    size_t size; ///<Rzeczywisty rozmiar tablicy
    size_t allocatedSize; ///<Zaalokowany rozmiar tablicy dynamicznej;
};


static size_t parent(size_t i) { return (i - 1) / 2; }///<Pozycja rodzica danego elementu w tablicy

static size_t left(size_t i) { return (2 * i + 1); }///<Pozycja lewego dziecka danego elementu w tablicy

static size_t right(size_t i) { return (2 * i + 2); }///<Pozycja prawego dziecka danego elementu w tablicy

/**
 * @brief Zamienia zawartości dwóch zmiennych typu unsigned
 * @param first Wskaźnik na jedną zmienną
 * @param second Wskaźnik na drugą zmienną
 */
static void swap(unsigned *first, unsigned *second) {
    unsigned buff = *first;
    *first = *second;
    *second = buff;
}

/**
 * @brief Zamienia miejscami dwa wskaźniki
 * @param first pierwszy wskaźnik
 * @param second drugi wskaźnik
 */
static void objSwap(void **first, void **second) {
    void *buff = *first;
    *first = *second;
    *second = buff;
}

PriorityQueue newQueue() {
    PriorityQueue result = malloc(sizeof(struct PriorityQueue));
    if (!result) {
        return NULL;
    }
    result->heap = malloc(sizeof(unsigned) * FIRST_ALLOC_SIZE);
    if (!result->heap) {
        free(result);
        return NULL;
    }
    result->objHeap = malloc(sizeof(void *) * FIRST_ALLOC_SIZE);
    if (!result->objHeap) {
        free(result->heap);
        free(result);
    }
    result->allocatedSize = FIRST_ALLOC_SIZE;
    result->size = 0;
    return result;
}

bool isEmpty(PriorityQueue queue) {
    return (queue->size == 0);
}

/**
 * @brief Przywraca własność kopca zaczynając od wskazanej pozycji
 * @param queue Kolejka na której wykonujemy operację
 * @param position Pozycja od której zaczynamy
 */
static void MinHeapify(PriorityQueue queue, size_t position) {
    size_t l = left(position);
    size_t r = right(position);
    size_t min = position;
    if (l < queue->size && queue->heap[l] < queue->heap[min]) {
        min = l;
    }
    if (r < queue->size && queue->heap[r] < queue->heap[min]) {
        min = r;
    }
    if (min != position) {
        swap(&(queue->heap[position]), &(queue->heap[min]));
        objSwap(&(queue->objHeap[position]), &(queue->objHeap[min]));
        MinHeapify(queue, min);
    }
}

void *popMin(PriorityQueue queue) {
    if (isEmpty(queue)) {
        return NULL;
    }
    if (queue->size == 1) {
        void *answer = queue->objHeap[0];
        queue->size = 0;
        return answer;
    }
    void *answer = queue->objHeap[0];
    queue->heap[0] = queue->heap[queue->size - 1];
    queue->objHeap[0] = queue->objHeap[queue->size - 1];
    queue->size--;
    MinHeapify(queue, 0);
    return answer;
}


bool insertPriorityQueue(PriorityQueue queue, void *obj, unsigned key) {
    if (queue->size == queue->allocatedSize) {
        unsigned *newHeap = realloc(queue->heap, sizeof(unsigned) * (queue->allocatedSize) * 2);
        if (!newHeap) {
            return false;
        }
        void **newObjHeap = realloc(queue->objHeap, sizeof(void *) * (queue->allocatedSize) * 2);
        if (!newObjHeap) {
            free(newHeap);
            return false;
        }
        queue->heap = newHeap;
        queue->objHeap = newObjHeap;
        queue->allocatedSize *= 2;
    }
    queue->size++;
    size_t position = queue->size - 1;
    queue->heap[position] = key;
    queue->objHeap[position] = obj;

    while (position != 0 && queue->heap[parent(position)] > queue->heap[position]) {
        swap(&(queue->heap[position]), &(queue->heap[parent(position)]));
        objSwap(&(queue->objHeap[position]), &(queue->objHeap[parent(position)]));
        position = parent(position);
    }
    return true;
}

void removePriorityQueue(PriorityQueue queue) {
    free(queue->heap);
    free(queue->objHeap);
    free(queue);
}


