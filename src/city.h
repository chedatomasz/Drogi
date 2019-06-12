/** @file
 * Interfejs klasy operującej na strukturach City
 *
 * @author Tomasz Cheda <tomasz.cheda@students.mimuw.edu.pl>
 * @date 29.04.2019
 */
#ifndef DROGI_CITY_H
#define DROGI_CITY_H


#include <stdbool.h>
#include "route.h"

/**
 * Struktura przechowująca miasto i jego połączenia.
 */
typedef struct City *City;

/**
 * @brief Implementacja struktury przechowującej miasto i jego połączenia.
 */
struct City {
    char *name; ///< String przechowujący nazwę miasta.
    struct ConnectionTree *root; ///< Korzeń drzewa ConnectionTree reprezentującego sąsiedztwo.
    int numOfConnections; ///< Liczba sąsiadów wierzchołka.
    int number; ///< Liczba porządkowa, wykorzystywana przy trasowaniu - forma minimal perfect hasha.
    RouteNumber routes; ///< Lista dróg krajowych do których miasto należy
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
City newCity(char *name);

/** @brief Sprawdza, czy city należy do danej route
* Szuka Route wśród numerów zapisanych dla danego miasta.
* @param[in] city     – wskaźnik na  City, którego RouteNumber mają zostać przeszukane
* @param[in] route    – numer szukanej drogi krajowej.
* @return true jeśli znaleziono obiekt, false w przeciwnym przypadku
*/
bool belongsToRoute(City city, unsigned route);

/** @brief Zapisuje w city przynależność do danej route
* Wpisuje Route wśród numerów zapisanych dla danego miasta.
* @param[in] city     – wskaźnik na  City, którego RouteNumber ma zostać dopisane
* @param[in] route    – numer dodawanej drogi krajowej.
* @return true jeśli udało się zaalokować pamięć, false w przeciwnym przypadku
*/
bool addToRoute(City city, unsigned route);

/** @brief Usuwa z  city przynależność do danej route
* Usuwa Route spośród numerów zapisanych dla danego miasta.
* @param[in] city     – wskaźnik na  City, którego RouteNumber ma zostać usunięte
* @param[in] route    – numer usuwanej drogi krajowej.
*/
void removeFromRoute(City city, unsigned route);

#endif //DROGI_CITY_H
