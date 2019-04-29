/** @file
 * Interfejs klasy oferującej kolejkę priorytetową void*
 *
 * @author Tomasz Cheda <tomasz.cheda@students.mimuw.edu.pl>
 * @date 29.04.2019
 */

#include <stdbool.h>
#include "connection_tree.h"

#ifndef DROGI_PRIORITY_QUEUE_H
#define DROGI_PRIORITY_QUEUE_H

/**
 * Struktura przechowująca kolejkę priorytetową.
 */
typedef struct PriorityQueue* PriorityQueue;

/**
 * @brief Zwraca, czy dana kolejka priorytetowa jest pusta.
 @param[in] queue - Wskaźnik na kolejkę priorytetową.
 @return - true jeśli kolejka jest pusta, false w przeciwnym wypadku.
 */
bool isEmpty(PriorityQueue queue);

/** @brief Tworzy nową strukturę kolejki priorytetowej.
 * Tworzy nową, pustą strukturę niezawierającą żadnych elementów.
 * Kolejka jest typu min.
 * @return Wskaźnik na utworzoną strukturę lub NULL, gdy nie udało się
 * zaalokować pamięci.
 */
PriorityQueue newQueue();

/** @brief Usuwa i zwraca najmniejszy element kolejki.
 * Zwraca element o najmniejszym priorytecie.
 * Utrzymuje własność kopca binarnego typu min.
 * @return Wskaźnik na najmniejszy element lub NULL, gdy kolejka jest pusta.
 */
void* popMin(PriorityQueue queue);

/** @brief Wstawia element do kolejki priorytetowej.
 * Utrzymuje własność kopca binarnego typu min.
 * @param[in,out] queue - wskaźnik na kolejkę, do której wstawiamy.
 * @param[in] obj - wskaźnik na element, który chcemy przechować.
 * @param[in] key - klucz wstawianego elementu.
 * @return True w przypadku powodzenia lub false,
 * gdy nie udało się zaalokować pamięci.
 */
bool insertPriorityQueue(PriorityQueue queue, void* obj, int key);

/** @brief Usuwa kolejkę priotytetową.
 * Wyrzuca pozostałe elementy i zwalnia pamięć.
 * @param[in] queue - wskaźnik na usuwaną kolejkę.
 */
void removePriorityQueue(PriorityQueue queue);
#endif //DROGI_PRIORITY_QUEUE_H
