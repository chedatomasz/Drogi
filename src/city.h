//
// Created by chedat on 25.04.19.
//

#ifndef DROGI_CITY_H
#define DROGI_CITY_H


#include <stdbool.h>

/**
 * Struktura przechowująca miasto i jego połączenia.
 */
typedef struct City* City;
typedef struct RouteNumber* RouteNumber; //Forward declaration to avoid circular inclusion with route.h

struct City {
    char* name;
    struct ConnectionTree* root;
    int numOfConnections;
    RouteNumber routes;
};

/** @brief Usuwa strukturę.
 * Usuwa strukturę wskazywaną przez @p city.
 * Nic nie robi, jeśli wskaźnik ten ma wartość NULL.
 * @param[in] city        – wskaźnik na usuwaną strukturę.
 */
void freeCity(City city);

/** @brief Tworzy nową strukturę.
 * Tworzy nową, pustą strukturę niezawierającą żadnych połączeń.
 * @return Wskaźnik na utworzoną strukturę lub NULL, gdy nie udało się
 * zaalokować pamięci.
 */
City newCity(const char* name);

bool belongsToRoute(City city, int route);

bool addToRoute(City city, int route);

#endif //DROGI_CITY_H
