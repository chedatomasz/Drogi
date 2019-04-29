/** @file
 * Interfejs klasy oferującej hashowane drzewo sąsiedztwa
 *
 * @author Tomasz Cheda <tomasz.cheda@students.mimuw.edu.pl>
 * @date 29.04.2019
 */

#ifndef DROGI_CONNECTION_BST_H
#define DROGI_CONNECTION_BST_H

#include <stdbool.h>
#include "city.h"

/**
 * Struktura przechowująca informacje o pojedynczym drzewie sąsiedztwa.
 */
typedef struct ConnectionTree* ConnectionTree;

/**
 * Struktura przechowująca informacje o pojedynczym połączeniu.
 */
typedef struct Connection* Connection;

/**
 * Definicja struktury przechowująca informacje o pojedynczym połączeniu.
 */
struct Connection {
    int length; ///< Długość drogi między miastami
    int year; ///< Rok budowy lub ostatniego remontu
    City city2; ///< Wskaźnik na miasto docelowe
};

/** @brief Dodaje do drzew sąsiedztwa informacje o połączeniu city1->city2 i city2->city1.
 * @param[in] city1 - pierwsze miasto, które należy połączyć.
 * @param[in] city2 - drugie miasto, które należy połączyć.
 * @param[in] length - długość połączenia między miastami.
 * @param[in] builtYear - rok budowy połączenia między miastami.
 * @return True w przypadku powodzenia, false gdy nie udało się zaalokować pamięci.
 */
bool addConnection(City city1, City city2, int length, int builtYear);

/** @brief Zwraca z drzewa sąsiedztwa informację o połączeniu city1->city2.
 * @param[in] city1 - miasto wyjściowe, którego drzewo sąsiedztwa zostanie przeszukane
 * @param[in] city2 - miasto docelowe. Przy poprawnie zbudowanej strukturze, relacja jest symetrycza.
 * @return Wskaźnik na strukturę w przypadku powodzenia lub NULL, gdy połączenie nie istnieje,
 * lub nie udało się zaalokować pamięci
 */
Connection getConnection(City city1, City city2);

/** @brief Zwraca z drzewa sąsiedztwa informację o wszystkich połączeniac z miasta city1
 * @param[in] city1 - miasto wyjściowe, którego drzewo sąsiedztwa zostanie przeszukane
 * @return Wskaźnik na strukturę w przypadku powodzenia lub NULL, gdy nie udało się zaalokować pamięci.
 * Informacja o rozmiarze tablicy jest zawarta w City->numOfConnections.
 */
Connection* getAllConnections(City city1);

/** @brief Zwalnia drzewo sąsiedztwa wskazywane przez node;
 */
void freeConnectionTree(ConnectionTree node);

/** @brief Usuwa z drzewa sąsiedztwa informację o połączeniu city1->city2.
 * @param[in] city1 - miasto wyjściowe, którego drzewo sąsiedztwa zostanie przeszukane
 * @param[in] city2 - miasto docelowe.
 */
void removeConnection(City city1, City city2);

#endif //DROGI_CONNECTION_BST_H
