/** @file
 * Implementacja klasy przechowującej mapę dróg krajowych
 *
 * @author Tomasz Cheda <tomasz.cheda@students.mimuw.edu.pl>
 * @date 24.04.2019
 */
#define NUMBER_OF_ROUTES 1000 ///< Maksymalna liczba dróg
#define HASHMAP_SIZE_BITS 20 ///< Rozmiar w bitach używanej hashmapy
#define MIN_ROUTE_NUM 1 ///< Najniższy poprawny numer drogi krajowej
#define MAX_ROUTE_NUM 999 ///< Najwyższy poprawny numer drogi krajowej
#define EXAMPLE_INVALID_ROUTE 9999 ///< Niepoprawny numer drogi krajowej - placeholder

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include "map.h"
#include "route.h"
#include "hashmap.h"
#include "connection_tree.h"
#include "pathfinder.h"
#include "input.h"


/**
 * Implementacja struktury przechowująca mapę dróg krajowych.
 */
struct Map {
    Route routes[NUMBER_OF_ROUTES]; ///< Tablica statyczna przechowująca zapisy dróg krajowych
    Hashmap cities; ///< Wskaźnik na hashmapę<string, City>
    CityList allCities; ///< Lista wszystkich miast aby uniknąć przechodzenia hashmapy
    CityList lastCity; ///< Koniec powyższej listy
    int numOfCities; ///< Liczba miast na mapie, używane do ich indeksowania
};

CityList getCityList(Map *map) {
    return map->allCities;
}

Map *newMap(void) {
    Map *result = malloc(sizeof(struct Map));
    if (!result) {
        return NULL;
    }
    for (int i = 0; i < NUMBER_OF_ROUTES; i++) {
        result->routes[i] = NULL;
    }
    result->cities = newHashmap(HASHMAP_SIZE_BITS);
    if (result->cities == NULL) {
        free(result);
        return NULL;
    }
    result->allCities = NULL;
    result->lastCity = NULL;
    result->numOfCities = 0;
    return result;
}


void deleteMap(Map *map) {
    if (!map) {
        return;
    }
    deleteCitiesHashmap(map->cities);
    freeCityList(map->allCities);
    for (int i = MIN_ROUTE_NUM; i <= MAX_ROUTE_NUM; i++) {
        freeRoute(map->routes[i]);
    }
    free(map);
}

/** @brief Dopisuje CityList do listy wszystkich miast w map*/
static void addCityToList(Map *map, CityList list) {
    if (map->allCities == NULL) {
        map->allCities = list;
        map->lastCity = list;
        return;
    }
    while (map->lastCity->next) {
        map->lastCity = map->lastCity->next;
    }
    map->lastCity->next = list;
    map->lastCity = list;
}

/** @brief Dopisuje w miastach z listy routeId do listy przechodzących przez nie dróg */
static bool addRouteToCities(CityList first, unsigned routeId) {
    CityList iterator = first;
    while (iterator != NULL) {
        if (!addToRoute(iterator->city, routeId)) {
            CityList iterator2 = first;
            while (iterator2 != iterator) {
                removeFromRoute(iterator2->city, routeId);
                iterator2 = iterator2->next;
            }
            return false;
        }
        iterator = iterator->next;
    }
    return true;
}

/** @brief Usuwa w miastach z listy routeId z listy przechodzących przez nie dróg */
static void removeRouteFromCities(CityList first, unsigned routeId) {
    CityList iterator = first;
    while (iterator != NULL) {
        removeFromRoute(iterator->city, routeId);
        iterator = iterator->next;
    }
}

bool removeRoute(Map *map, unsigned routeId) {
    if (!map || routeId < MIN_ROUTE_NUM || routeId > MAX_ROUTE_NUM || !(map->routes[routeId])) {
        return false;
    }
    Route subject = map->routes[routeId];
    removeRouteFromCities(subject->first, routeId);
    freeCityList(subject->first);
    free(subject);
    map->routes[routeId] = NULL;
    return true;
}

bool addRoad(Map *map, const char *city1, const char *city2, unsigned length, int builtYear) {
    if (!map || !city1 || !city2) {
        return false;
    }
    if (!verifyCityName(city1) || !verifyCityName(city2) || builtYear == 0 || length == 0 ||
        strcmp(city1, city2) == 0) {
        return false;
    }
    CityList list1 = NULL;
    CityList list2 = NULL;
    bool firstCityAdded = false;
    bool secondCityAdded = false;
    City city1Pointer = getCity(map->cities, city1);
    if (!city1Pointer) {
        char *ourCity1 = malloc(sizeof(char) * (strlen(city1) + 1));
        if (!ourCity1) {
            return false;
        }
        strcpy(ourCity1, city1);
        list1 = malloc(sizeof(struct CityList));
        if (!list1) {
            free(ourCity1);
            return false;
        }
        city1Pointer = newCity(ourCity1);
        if (!city1Pointer) {
            free(list1);
            free(ourCity1);
            return false;
        }
        if (!addCity(map->cities, city1Pointer)) {
            freeCity(city1Pointer);
            free(list1);
            return false;
        }
        firstCityAdded = true;
    }
    City city2Pointer = getCity(map->cities, city2);
    if (!city2Pointer) {
        char *ourCity2 = malloc(sizeof(char) * (strlen(city2) + 1));
        if (!ourCity2) {
            if (firstCityAdded) {
                removeCity(map->cities, city1);
                free(list1);
            }
            return false;
        }
        strcpy(ourCity2, city2);
        list2 = malloc(sizeof(struct CityList));
        if (!list2) {
            free(ourCity2);
            if (firstCityAdded) {
                removeCity(map->cities, city1);
                free(list1);
            }
            return false;
        }
        city2Pointer = newCity(ourCity2);
        if (!city2Pointer) {
            if (firstCityAdded) {
                removeCity(map->cities, city1);
                free(ourCity2);
                free(list2);
                free(list1);
            }
            return false;
        }
        if (!addCity(map->cities, city2Pointer)) {
            if (firstCityAdded) {
                removeCity(map->cities, city1);
                freeCity(city2Pointer);
                free(list1);
                free(list2);
            }
            return false;
        }
        secondCityAdded = true;
    }
    if (getConnection(city1Pointer,
                      city2Pointer)) {//We don't have to free the cities - if there is a connection we didn't create them now
        return false;
    }
    if (!addConnection(city1Pointer, city2Pointer, length, builtYear)) {
        if (firstCityAdded) {
            freeCity(city1Pointer);
            free(list1);
        }
        if (secondCityAdded) {
            freeCity(city2Pointer);
            free(list2);
        }
        return false;
    }
    if (firstCityAdded) {
        list1->city = city1Pointer;
        list1->next = NULL;
        addCityToList(map, list1);
        city1Pointer->number = map->numOfCities;
        map->numOfCities++;
    }
    if (secondCityAdded) {
        list2->city = city2Pointer;
        list2->next = NULL;
        addCityToList(map, list2);
        city2Pointer->number = map->numOfCities;
        map->numOfCities++;
    }
    return true;
}

/**
 * Struktura danych przechowująca liste zmian
 */
struct changeFile {
    CityList *addedCity; ///< Tablica dodanych miast
    City *addedConn1; ///< Tablica początków dodanych dróg
    City *addedConn2; ///< Tablica końców dodanych dróg
    City *moddedConn1; ///< Tablica początków zmodyfikowanych dróg
    City *moddedConn2; ///< Tablica końców zmodyfikowanych dróg
    int *moddedConn3; ///< Tablica pierwotnych danych zmodyfikowanych dróg
    size_t addedCities; ///< Liczba dodanych miast
    size_t addedConns; ///< Liczba dodanych połączeń
    size_t moddedConns; ///< Liczba zmodyfikowanych połączeń
    bool successful; ///< Flaga, czy udało się stworzyć powyższe tablice
};

/**
 * @brief Funkcja tworząca nową listę zmian
 * @param numOfCities rozmiar tablic
 * @return stworzona struktura
 */
struct changeFile newChangeFile(size_t numOfCities) {
    struct changeFile file;
    file.addedCity = malloc(sizeof(struct CityList) * numOfCities);
    file.addedConn1 = malloc(sizeof(City) * numOfCities);
    file.addedConn2 = malloc(sizeof(City) * numOfCities);
    file.moddedConn1 = malloc(sizeof(City) * numOfCities);
    file.moddedConn2 = malloc(sizeof(City) * numOfCities);
    file.moddedConn3 = malloc(sizeof(int) * numOfCities);
    file.addedCities = 0;
    file.addedConns = 0;
    file.moddedConns = 0;
    if (!file.addedCity || !file.addedConn1 || !file.addedConn2 || !file.moddedConn1 || !file.moddedConn2 ||
        !file.moddedConn3) {
        free(file.addedCity);
        free(file.addedConn1);
        free(file.addedConn2);
        free(file.moddedConn1);
        free(file.moddedConn2);
        free(file.moddedConn3);
        file.successful = false;
        return file;
    }
    file.successful = true;
    return file;
}

/**
 * @brief Funkcja wycofująca zmiany z listy
 * @param map mapa, w której należy wycofać zmiany
 * @param log lista zmian do wycofania
 */
void undoChanges(Map *map, struct changeFile log) {
    //Unmodifying repair dates
    for (size_t i = 0; i < log.moddedConns; i++) {
        getConnection(log.moddedConn1[i], log.moddedConn2[i])->year = log.moddedConn3[i];
        getConnection(log.moddedConn2[i], log.moddedConn1[i])->year = log.moddedConn3[i];
    }
    //Removing added connections
    //Order is reverse addition so that tree leaves are removed first, should be faster than opposite
    for (size_t i = 0; i < log.addedConns; i++) {
        removeRoad(map, log.addedConn1[log.addedConns - 1 - i]->name, log.addedConn2[log.addedConns - 1 - i]->name);
    }
    //Removing added cities
    for (size_t i = 0; i < log.addedCities; i++) {
        removeCity(map->cities, log.addedCity[i]->city->name);
        free(log.addedCity[i]);
    }
    //Removing the log
    free(log.addedCity);
    free(log.addedConn1);
    free(log.addedConn2);
    free(log.moddedConn1);
    free(log.moddedConn2);
    free(log.moddedConn3);
}

/**
 * @brief Funkcja zatwierdzająca zmiany z listy
 * @param map mapa, w której zmiany będą zatwierdzone
 * @param log lista zmian
 */
void implementChangeLog(Map *map, struct changeFile log) {
    //Numbering the cities and adding them to the list
    for (size_t i = 0; i < log.addedCities; i++) {
        log.addedCity[i]->next = NULL;
        addCityToList(map, log.addedCity[i]);
        log.addedCity[i]->city->number = map->numOfCities;
        map->numOfCities++;
    }
    //Removing the log
    free(log.addedCity);
    free(log.addedConn1);
    free(log.addedConn2);
    free(log.moddedConn1);
    free(log.moddedConn2);
    free(log.moddedConn3);
}

bool addRoute(Map *map, char *line) {
    if (!checkAddRouteSyntax(line)) {
        return false;
    }
    size_t numOfCities = getCityNum(line) + 1;
    struct changeFile log = newChangeFile(numOfCities);
    if (!log.successful) {
        return false;
    }
    char *copy = (char *) line;
    char *first = tokenize(&copy, ';');
    char *city1 = tokenize(&copy, ';');
    char *length = tokenize(&copy, ';');
    char *builtYear = tokenize(&copy, ';');
    char *city2 = tokenize(&copy, ';');
    if (!first || !city1 || !length || !builtYear || !city2) {
        undoChanges(map, log);
        return false;
    }
    unsigned routeNumber = getUnsignedFromString(first);
    if (errno || !verifyRouteNumber(map, routeNumber) || !verifyCityName(city1) || !verifyCityName(city2)) {
        undoChanges(map, log);
        return false;
    }
    unsigned lengthInt = getUnsignedFromString(length);
    if (errno) {
        undoChanges(map, log);
        return false;
    }
    int yearInt = getIntFromString(builtYear);
    if (errno) {
        undoChanges(map, log);
        return false;
    }
    int status = roadStatus(map, city1, city2, lengthInt, yearInt);
    if (status == ROAD_CONFLICTING) {
        undoChanges(map, log);
        return false;
    }
    if (status == ROAD_NOT_FOUND) {
        if (!getCity(map->cities, city1)) {
            char *ourCity1 = malloc(sizeof(char) * (strlen(city1) + 1));
            if (!ourCity1) {
                undoChanges(map, log);
                return false;
            }
            strcpy(ourCity1, city1);
            log.addedCity[log.addedCities] = malloc(sizeof(struct CityList));
            if (!log.addedCity[log.addedCities]) {
                undoChanges(map, log);
                return false;
            }
            log.addedCity[log.addedCities]->city = newCity(ourCity1);
            if (!log.addedCity[log.addedCities]->city) {
                undoChanges(map, log);
                return false;
            }
            if (!addCity(map->cities, log.addedCity[log.addedCities]->city)) {
                undoChanges(map, log);
                return false;
            }
            log.addedCities++;
        }
        if (!getCity(map->cities, city2)) {
            char *ourCity2 = malloc(sizeof(char) * (strlen(city2) + 1));
            if (!ourCity2) {
                undoChanges(map, log);
                return false;
            }
            strcpy(ourCity2, city2);
            log.addedCity[log.addedCities] = malloc(sizeof(struct CityList));
            if (!log.addedCity[log.addedCities]) {
                undoChanges(map, log);
                return false;
            }
            log.addedCity[log.addedCities]->city = newCity(ourCity2);
            if (!log.addedCity[log.addedCities]->city) {
                undoChanges(map, log);
                return false;
            }
            if (!addCity(map->cities, log.addedCity[log.addedCities]->city)) {
                undoChanges(map, log);
                return false;
            }
            log.addedCities++;
        }
        if (!addRoad(map, city1, city2, lengthInt, yearInt)) {
            undoChanges(map, log);
            return false;
        } else {
            log.addedConn1[log.addedConns] = getCity(map->cities, city1);
            log.addedConn2[log.addedConns] = getCity(map->cities, city2);
            log.addedConns++;
        }
    }
    if (status == ROAD_TO_REPAIR) {
        log.moddedConn1[log.moddedConns] = getCity(map->cities, city1);
        log.moddedConn2[log.moddedConns] = getCity(map->cities, city2);
        log.moddedConn3[log.moddedConns] = getConnection(log.moddedConn1[log.moddedConns],
                                                         log.moddedConn2[log.moddedConns])->year;
        if (!repairRoad(map, city1, city2, yearInt)) {
            undoChanges(map, log);
            return false;
        } else {
            log.moddedConns++;
        }
    }
    CityList listOfCities = newCityList(map, city1);
    if (!listOfCities) {
        undoChanges(map, log);
        return false;
    }
    CityList workingListOfCities = listOfCities;
    workingListOfCities = addCityToCityList(map, workingListOfCities, city2);
    if (!workingListOfCities) {
        removeCityList(listOfCities);
        undoChanges(map, log);
        return false;
    }
    city1 = city2;
    while (1) {
        length = tokenize(&copy, ';');
        builtYear = tokenize(&copy, ';');
        city2 = tokenize(&copy, ';');
        if (!length) {//input has ended
            break;
        }
        //input ends mid-sequence
        if (!builtYear || !city2) {
            removeCityList(listOfCities);
            undoChanges(map, log);
            return false;
        }
        lengthInt = getUnsignedFromString(length);
        if (errno) {
            removeCityList(listOfCities);
            undoChanges(map, log);
            return false;
        }
        yearInt = getIntFromString(builtYear);
        if (errno) {
            removeCityList(listOfCities);
            undoChanges(map, log);
            return false;
        }
        if (!verifyCityName(city2)) {
            removeCityList(listOfCities);
            undoChanges(map, log);
            return false;
        }
        status = roadStatus(map, city1, city2, lengthInt, yearInt);

        if (status == ROAD_CONFLICTING) {
            removeCityList(listOfCities);
            undoChanges(map, log);
            return false;
        }

        if (status == ROAD_NOT_FOUND) {
            if (!getCity(map->cities, city2)) {
                char *ourCity2 = malloc(sizeof(char) * (strlen(city2) + 1));
                if (!ourCity2) {
                    undoChanges(map, log);
                    return false;
                }
                strcpy(ourCity2, city2);
                log.addedCity[log.addedCities] = malloc(sizeof(struct CityList));
                if (!log.addedCity[log.addedCities]) {
                    undoChanges(map, log);
                    return false;
                }
                log.addedCity[log.addedCities]->city = newCity(ourCity2);
                if (!log.addedCity[log.addedCities]->city) {
                    undoChanges(map, log);
                    return false;
                }
                if (!addCity(map->cities, log.addedCity[log.addedCities]->city)) {
                    undoChanges(map, log);
                    return false;
                }
                log.addedCities++;
            }
            if (!addRoad(map, city1, city2, lengthInt, yearInt)) {
                undoChanges(map, log);
                return false;
            } else {
                log.addedConn1[log.addedConns] = getCity(map->cities, city1);
                log.addedConn2[log.addedConns] = getCity(map->cities, city2);
                log.addedConns++;
            }
        }

        if (status == ROAD_TO_REPAIR) {
            log.moddedConn1[log.moddedConns] = getCity(map->cities, city1);
            log.moddedConn2[log.moddedConns] = getCity(map->cities, city2);
            log.moddedConn3[log.moddedConns] = getConnection(log.moddedConn1[log.moddedConns],
                                                             log.moddedConn2[log.moddedConns])->year;
            if (!repairRoad(map, city1, city2, yearInt)) {
                undoChanges(map, log);
                return false;
            } else {
                log.moddedConns++;
            }
        }
        workingListOfCities = addCityToCityList(map, workingListOfCities, city2);
        if (!workingListOfCities) {
            removeCityList(listOfCities);
            undoChanges(map, log);
            return false;
        }

        city1 = city2;
    }
    if (!addRouteExplicit(map, routeNumber, listOfCities)) {
        undoChanges(map, log);
        return false;
    }
    implementChangeLog(map, log);
    return true;
}

bool repairRoad(Map *map, const char *city1, const char *city2, int repairYear) {
    if (!map || !city1 || !city2) {
        return false;
    }
    if (!verifyCityName(city1) || !verifyCityName(city2) || repairYear == 0 || strcmp(city1, city2) == 0) {
        return false;
    }
    City city1Pointer = getCity(map->cities, city1);
    City city2Pointer = getCity(map->cities, city2);
    if (!city1Pointer || !city2Pointer) {
        return false;
    }
    Connection connection1 = getConnection(city1Pointer, city2Pointer);
    Connection connection2 = getConnection(city2Pointer, city1Pointer);
    if (!connection1 || !connection2) {
        return false;
    }
    if (connection1->year > repairYear ||
        connection2->year > repairYear) {//Double check is redundant if structure is built properly.
        return false;
    }
    connection1->year = repairYear;
    connection2->year = repairYear;
    return true;
}

bool newRoute(Map *map, unsigned routeId, const char *city1, const char *city2) {
    if (!map || !city1 || !city2) {
        return false;
    }
    if (!verifyCityName(city1) || !verifyCityName(city2) || strcmp(city1, city2) == 0) {
        return false;
    }
    if (routeId < MIN_ROUTE_NUM || routeId > MAX_ROUTE_NUM) {
        return false;
    }
    City city1Pointer = getCity(map->cities, city1);
    City city2Pointer = getCity(map->cities, city2);
    if (!city1Pointer || !city2Pointer) {
        return false;
    }
    if (map->routes[routeId]) {
        return false;
    }
    map->routes[routeId] = malloc(sizeof(struct Route));
    if (!(map->routes[routeId])) {
        return false;
    }
    map->routes[routeId]->first = findPath(map, city1Pointer, city2Pointer, EXAMPLE_INVALID_ROUTE, NULL, NULL);
    if (map->routes[routeId]->first) {
        if (addRouteToCities(map->routes[routeId]->first, routeId)) {
            return true;
        } else {
            freeCityList(map->routes[routeId]->first);
        }
    } else {
        free(map->routes[routeId]);
        map->routes[routeId] = NULL;
    }
    return false;
}

bool extendRoute(Map *map, unsigned routeId, const char *city) {
    if (!map || !city || routeId < MIN_ROUTE_NUM || routeId > MAX_ROUTE_NUM || !verifyCityName(city)) {
        return false;
    }
    if (!map->routes[routeId]) {
        return false;
    }
    City cityPointer = getCity(map->cities, city);
    if (!cityPointer) {
        return false;
    }
    if (belongsToRoute(cityPointer, routeId)) {
        return false;
    }
    City firstCity = map->routes[routeId]->first->city;
    CityList lastCityList = map->routes[routeId]->first;
    while (lastCityList->next) {
        lastCityList = lastCityList->next;
    }
    City lastCity = lastCityList->city;
    CityList fromLastCity = findPath(map, lastCity, cityPointer, routeId, NULL, NULL);
    CityList toFirstCity = findPath(map, cityPointer, firstCity, routeId, NULL, NULL);
    if (!fromLastCity && !toFirstCity) {
        return false;
    }
    if (!toFirstCity) {
        CityList proper = fromLastCity->next;
        free(fromLastCity);
        if (addRouteToCities(proper, routeId)) {
            lastCityList->next = proper;
            return true;
        }
        freeCityList(proper);
        return false;
    }
    if (!fromLastCity) {
        CityList iterator = toFirstCity;
        while (iterator->next->next) {
            iterator = iterator->next;
        }
        free(iterator->next);
        iterator->next = NULL;
        if (addRouteToCities(toFirstCity, routeId)) {
            iterator->next = map->routes[routeId]->first;
            map->routes[routeId]->first = toFirstCity;
            return true;
        }
        freeCityList(toFirstCity);
        return false;
    }
    long dist1 = 0;
    int oldest1 = INT_MAX;
    CityList iterator = fromLastCity;
    while (iterator->next) {
        Connection temp = getConnection(iterator->city, iterator->next->city);
        dist1 += temp->length;
        if (temp->year < oldest1) {
            oldest1 = temp->year;
        }
        iterator = iterator->next;
    }
    long dist2 = 0;
    int oldest2 = INT_MAX;
    iterator = toFirstCity;
    while (iterator->next) {
        Connection temp = getConnection(iterator->city, iterator->next->city);
        dist2 += temp->length;
        if (temp->year < oldest2) {
            oldest2 = temp->year;
        }
        iterator = iterator->next;
    }
    if (dist1 < dist2 || (dist1 == dist2 && oldest1 > oldest2)) {
        freeCityList(toFirstCity);
        CityList proper = fromLastCity->next;
        free(fromLastCity);
        if (addRouteToCities(proper, routeId)) {
            lastCityList->next = proper;
            return true;
        }
        freeCityList(proper);
        return false;
    }
    if (dist1 > dist2 || (dist1 == dist2 && oldest2 > oldest1)) {
        freeCityList(fromLastCity);
        iterator = toFirstCity;
        while (iterator->next->next) {
            iterator = iterator->next;
        }
        free(iterator->next);
        iterator->next = NULL;
        if (addRouteToCities(toFirstCity, routeId)) {
            iterator->next = map->routes[routeId]->first;
            map->routes[routeId]->first = toFirstCity;
            return true;
        }
        freeCityList(toFirstCity);
        return false;
    }
    freeCityList(fromLastCity);
    freeCityList(toFirstCity);
    return false;

}

/** @brief Stwierdza, czy miasta w route zachowują tutejszą kolejność.
 * @param map - przeszukiwana mapa.
 * @param city1 - pierwsze miasto
 * @param city2 - drugie miasto
 * @param route - analizowane route
 * @return true jeśli kolejność jest zachowana, false jeśli nie
 */
static bool areInOrder(Map *map, City city1, City city2, unsigned route) {
    CityList iterator = map->routes[route]->first;
    while (iterator && (iterator->city != city1)) {
        iterator = iterator->next;
    }
    return (iterator != NULL) && (iterator->next) && iterator->next->city == city2;
}

bool removeRoad(Map *map, const char *city1, const char *city2) {
    if (!map || !city1 || !city2) {
        return false;
    }
    if (!verifyCityName(city1) || !verifyCityName(city2) || strcmp(city1, city2) == 0) {
        return false;
    }
    City city1Pointer = getCity(map->cities, city1);
    City city2Pointer = getCity(map->cities, city2);
    if (!city1Pointer || !city2Pointer) {
        return false;
    }
    Connection exclude1 = getConnection(city1Pointer, city2Pointer);
    Connection exclude2 = getConnection(city2Pointer, city1Pointer);
    if (!exclude1 || !exclude2) {
        return false;
    }
    unsigned toFix[NUMBER_OF_ROUTES];
    int numOfRoutes = 0;
    CityList found[NUMBER_OF_ROUTES];
    int direction[NUMBER_OF_ROUTES];
    for (unsigned i = MIN_ROUTE_NUM; i <= MAX_ROUTE_NUM; i++) {
        if (map->routes[i]) {
            if (areInOrder(map, city1Pointer, city2Pointer, i)) {
                toFix[numOfRoutes] = i;
                direction[numOfRoutes] = 1;
                found[numOfRoutes] = findPath(map, city1Pointer, city2Pointer, i, exclude1, exclude2);
                numOfRoutes++;
            } else if (areInOrder(map, city2Pointer, city1Pointer, i)) {
                toFix[numOfRoutes] = i;
                direction[numOfRoutes] = 2;
                found[numOfRoutes] = findPath(map, city2Pointer, city1Pointer, i, exclude1, exclude2);
                numOfRoutes++;
            }
        }
    }
    for (int i = 0; i < numOfRoutes; i++) {
        if (!found[i]) {
            for (int j = 0; j < numOfRoutes; j++) {
                freeCityList(found[j]);
            }
            return false;
        }
        CityList temp = found[i];
        found[i] = found[i]->next;
        free(temp);
        temp = found[i];
        while (temp->next->next) {
            temp = temp->next;
        }
        free(temp->next);
        temp->next = NULL;
    }
    for (int i = 0; i < numOfRoutes; i++) {
        if (!addRouteToCities(found[i], toFix[i])) {
            for (int j = 0; j < i; j++) {
                removeRouteFromCities(found[j], toFix[j]);
            }
            return false;
        }
    }
    for (int i = 0; i < numOfRoutes; i++) {
        if (direction[i] == 1) {
            CityList iterator = map->routes[toFix[i]]->first;
            while (iterator->city != city1Pointer) {
                iterator = iterator->next;
            }
            CityList temp = iterator->next;
            iterator->next = found[i];
            while (iterator->next != NULL) {
                iterator = iterator->next;
            }
            iterator->next = temp;
        }
        if (direction[i] == 2) {
            CityList iterator = map->routes[toFix[i]]->first;
            while (iterator->city != city2Pointer) {
                iterator = iterator->next;
            }
            CityList temp = iterator->next;
            iterator->next = found[i];
            while (iterator->next != NULL) {
                iterator = iterator->next;
            }
            iterator->next = temp;
        }
    }
    removeConnection(city1Pointer, city2Pointer);
    removeConnection(city2Pointer, city1Pointer);
    return true;
}

char const *getRouteDescription(Map *map, unsigned routeId) {
    if (!map) {
        return NULL;
    }
    char *result;
    if (routeId < MIN_ROUTE_NUM || routeId > MAX_ROUTE_NUM || !map->routes[routeId]) {
        result = malloc(sizeof(char));
        if (!result) {
            return NULL;
        }
        *result = '\0';
        return result;
    }
    Route ourRoute = map->routes[routeId];
    size_t length = 0;
    length += (size_t) floor(log10(routeId));
    length++;
    CityList iterator = ourRoute->first;
    while (iterator->next) {
        length++;// ;
        length += strlen(iterator->city->name);
        length++;// ;
        Connection link = getConnection(iterator->city, iterator->next->city);
        length += (size_t) floor(log10(link->length));
        length += 2;//last char + ;
        if (link->year < 0) {
            length++; //minus sign
        }
        length += (size_t) floor(log10(abs(link->year)));
        length++; //last char
        iterator = iterator->next;
    }
    length++; // ;
    length += strlen(iterator->city->name);
    result = malloc(sizeof(char) * (length + 1));
    if (!result) {
        return NULL;
    }
    result[0] = '\0';
    iterator = ourRoute->first;
    char intBuffer[14];
    sprintf(intBuffer, "%d;", routeId);
    strcat(result, intBuffer);
    while (iterator->next) {
        strcat(result, iterator->city->name);
        Connection link = getConnection(iterator->city, iterator->next->city);
        sprintf(intBuffer, ";%u;", link->length);
        strcat(result, intBuffer);
        sprintf(intBuffer, "%d;", link->year);
        strcat(result, intBuffer);
        iterator = iterator->next;
    }
    strcat(result, iterator->city->name);
    return result;
}

bool verifyCityName(const char *city1) {
    size_t length = strlen(city1);
    bool valid = true;
    for (size_t i = 0; i < length; i++) {
        if ((city1[i] >= 0 && city1[i] < 32) || city1[i] == 59) {
            valid = false;
            break;
        }
    }
    return valid && length > 0;

}

bool verifyRouteNumber(Map *map, unsigned routeNumber) {
    return (map && routeNumber >= MIN_ROUTE_NUM && routeNumber <= MAX_ROUTE_NUM && !(map->routes[routeNumber]));
}

bool addRouteExplicit(Map *map, unsigned routeNumber, CityList list) {
    if (!list) {
        return false;
    }
    if (!map || !verifyRouteNumber(map, routeNumber)) {
        freeCityList(list);
        return false;
    }
    if (!addRouteToCities(list, routeNumber)) {
        freeCityList(list);
        return false;
    }
    map->routes[routeNumber] = malloc(sizeof(struct Route));
    if (!(map->routes[routeNumber])) {
        freeCityList(list);
        return false;
    }
    map->routes[routeNumber]->first = list;
    return true;
}

CityList newCityList(Map *map, char *city) {
    if (!map || !city) {
        return NULL;
    }
    CityList node = malloc(sizeof(struct CityList));
    if (!node) {
        return NULL;
    }
    City cityPtr = getCity(map->cities, city);
    if (!cityPtr) {
        return NULL;
    }
    node->city = cityPtr;
    node->next = NULL;
    return node;
}

CityList addCityToCityList(Map *map, CityList list, char *city) {
    if (!map || !city || !list) {
        return false;
    }
    CityList node = malloc(sizeof(struct CityList));
    if (!node) {
        return false;
    }
    City cityPtr = getCity(map->cities, city);
    if (!cityPtr) {
        return false;
    }
    node->city = cityPtr;
    node->next = NULL;
    while (list->next) {
        list = list->next;
    }
    list->next = node;
    return list->next;
}

void removeCityList(CityList list) {
    freeCityList(list);
}

int roadStatus(Map *map, char *city1, char *city2, unsigned length, int year) {
    if (!map || !city1 || !city2) {
        return ROAD_CONFLICTING;
    }
    City city1Ptr = getCity(map->cities, city1);
    City city2Ptr = getCity(map->cities, city2);
    if (!city1Ptr || !city2Ptr) {
        return ROAD_NOT_FOUND;
    }
    Connection found = getConnection(city1Ptr, city2Ptr);
    if (!found) {
        return ROAD_NOT_FOUND;
    }
    if (found->length != length || found->year > year) {
        return ROAD_CONFLICTING;
    }
    return ROAD_TO_REPAIR;
}
