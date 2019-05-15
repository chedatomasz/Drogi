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
#include "map.h"
#include "route.h"
#include "hashmap.h"
#include "connection_tree.h"
#include "pathfinder.h"


static bool verifyCityName(const char *city);

/**
 * Implementacja struktury przechowująca mapę dróg krajowych.
 */
struct Map {
    Route routes[NUMBER_OF_ROUTES]; ///< Tablica statyczna przechowująca zapisy dróg krajowych
    Hashmap cities; ///< Wskaźnik na hashmapę<string, City>
    CityList allCities; ///< Lista wszystkich miast aby uniknąć przechodzenia hashmapy
    int numOfCities; ///< Liczba miast na mapie, używane do ich indeksowania
};

CityList getCityList(Map* map){
    return map->allCities;
}

Map* newMap(void){
    Map* result = malloc(sizeof(struct Map));
    if(!result){
        return NULL;
    }
    for(int i = 0; i < NUMBER_OF_ROUTES; i++){
        result->routes[i] = NULL;
    }
    result->cities = newHashmap(HASHMAP_SIZE_BITS);
    if(result->cities == NULL){
        free(result);
        return NULL;
    }
    result->allCities = NULL;
    result->numOfCities = 0;
    return result;
}


void deleteMap(Map* map){
    if(!map){
        return;
    }
    deleteCitiesHashmap(map->cities);
    freeCityList(map->allCities);
    for(int i = MIN_ROUTE_NUM; i <= MAX_ROUTE_NUM; i++){
        freeRoute(map->routes[i]);
    }
    free(map);
}
/** @brief Dopisuje CityList do listy wszystkich miast w map*/
static void addCityToList(Map* map, CityList list){
    if(map->allCities == NULL){
        map->allCities = list;
        return;
    }
    CityList insert = map->allCities;
    while(insert->next){
        insert = insert->next;
    }
    insert->next = list;
}

/** @brief Dopisuje w miastach z listy routeId do listy przechodzących przez nie dróg */
static bool addRouteToCities(CityList first, unsigned routeId){
        CityList iterator = first;
        while(iterator != NULL){
            if(!addToRoute(iterator->city, routeId)){
                CityList iterator2 = first;
                while(iterator2!=iterator){
                    removeFromRoute(iterator2->city, routeId);
                    iterator2=iterator2->next;
                }
                return false;
            }
            iterator = iterator->next;
        }
        return true;
}
/** @brief Usuwa w miastach z listy routeId z listy przechodzących przez nie dróg */
static void removeRouteFromCities(CityList first, unsigned routeId){
    CityList iterator = first;
    while(iterator != NULL){
        removeFromRoute(iterator->city, routeId);
        iterator = iterator->next;
    }
}

bool addRoad(Map *map, const char *city1, const char *city2, unsigned length, int builtYear){
    if(!map || !city1 || !city2){
        return false;
    }
    if(!verifyCityName(city1) || !verifyCityName(city2) || builtYear == 0 || length == 0|| strcmp(city1, city2)==0){
        return false;
    }
    CityList list1 = NULL;
    CityList list2 = NULL;
    bool firstCityAdded = false;
    bool secondCityAdded = false;
    City city1Pointer = getCity(map->cities, city1);
    if(!city1Pointer){
        char* ourCity1 = malloc(sizeof(char)*(strlen(city1)+1));
        if(!ourCity1){
            return false;
        }
        strcpy(ourCity1, city1);
        list1 = malloc(sizeof(struct CityList));
        if(!list1){
            free(ourCity1);
            return false;
        }
        city1Pointer = newCity(ourCity1);
        if(!city1Pointer){
            free(list1);
            free(ourCity1);
            return false;
        }
        if (!addCity(map->cities, city1Pointer)){
            freeCity(city1Pointer);
            free(list1);
            return false;
        }
        firstCityAdded = true;
    }
    City city2Pointer = getCity(map->cities, city2);
    if(!city2Pointer){
        char* ourCity2 = malloc(sizeof(char)*(strlen(city2)+1));
        if(!ourCity2){
            if(firstCityAdded){
                removeCity(map->cities, city1);
                free(list1);
            }
            return false;
        }
        strcpy(ourCity2, city2);
        list2 = malloc(sizeof(struct CityList));
        if(!list2){
            free(ourCity2);
            if(firstCityAdded){
                removeCity(map->cities, city1);
                free(list1);
            }
            return false;
        }
        city2Pointer = newCity(ourCity2);
        if(!city2Pointer){
            if(firstCityAdded){
                removeCity(map->cities, city1);
                free(ourCity2);
                free(list2);
                free(list1);
            }
            return false;
        }
        if(!addCity(map->cities, city2Pointer)){
            if(firstCityAdded){
                removeCity(map->cities, city1);
                freeCity(city2Pointer);
                free(list1);
                free(list2);
            }
            return false;
        }
        secondCityAdded = true;
    }
    if(getConnection(city1Pointer, city2Pointer)){//We don't have to free the cities - if there is a connection we didn't create them now
        return false;
    }
    if(!addConnection(city1Pointer, city2Pointer, length, builtYear)){
        if(firstCityAdded){
            freeCity(city1Pointer);
            free(list1);
        }
        if(secondCityAdded){
            freeCity(city2Pointer);
            free(list2);
        }
        return false;
    }
    if(firstCityAdded){
        list1->city = city1Pointer;
        list1->next = NULL;
        addCityToList(map, list1);
        city1Pointer->number=map->numOfCities;
        map->numOfCities++;
    }
    if(secondCityAdded){
        list2->city = city2Pointer;
        list2->next = NULL;
        addCityToList(map, list2);
        city2Pointer->number=map->numOfCities;
        map->numOfCities++;
    }
    return true;
}

bool repairRoad(Map *map, const char *city1, const char *city2, int repairYear){
    if(!map || !city1 || !city2){
        return false;
    }
    if(!verifyCityName(city1) || !verifyCityName(city2) || repairYear == 0 || strcmp(city1, city2)==0){
        return false;
    }
    City city1Pointer = getCity(map->cities, city1);
    City city2Pointer = getCity(map->cities, city2);
    if(!city1Pointer || !city2Pointer){
        return false;
    }
    Connection connection1 = getConnection(city1Pointer, city2Pointer);
    Connection connection2 = getConnection(city2Pointer, city1Pointer);
    if(!connection1 || !connection2){
        return false;
    }
    if(connection1->year > repairYear || connection2->year >repairYear){//Double check is redundant if structure is built properly.
        return false;
    }
    connection1->year = repairYear;
    connection2->year = repairYear;
    return true;
}

bool newRoute(Map *map, unsigned routeId, const char *city1, const char *city2){
    if(!map || !city1 || !city2){
        return false;
    }
    if(!verifyCityName(city1) || !verifyCityName(city2) || strcmp(city1, city2)==0){
        return false;
    }
    if(routeId < MIN_ROUTE_NUM  || routeId > MAX_ROUTE_NUM){
        return false;
    }
    City city1Pointer = getCity(map->cities, city1);
    City city2Pointer = getCity(map->cities, city2);
    if(!city1Pointer || !city2Pointer){
        return false;
    }
    if(map->routes[routeId]){
        return false;
    }
    map->routes[routeId]=malloc(sizeof(struct Route));
    if(!(map->routes[routeId])){
        return false;
    }
    map->routes[routeId]->first=findPath(map, city1Pointer, city2Pointer, EXAMPLE_INVALID_ROUTE, NULL, NULL);
    if(map->routes[routeId]->first){
        if( addRouteToCities(map->routes[routeId]->first, routeId)){
            return true;
        }
        else {
            freeCityList(map->routes[routeId]->first);
        }
    }
    else{
        free(map->routes[routeId]);
        map->routes[routeId]=NULL;
    }
    return false;
}

bool extendRoute(Map *map, unsigned routeId, const char *city){
    if(!map || !city || routeId <MIN_ROUTE_NUM || routeId >MAX_ROUTE_NUM || !verifyCityName(city)){
        return false;
    }
    if(!map->routes[routeId]){
        return false;
    }
    City cityPointer = getCity(map->cities, city);
    if(!cityPointer){
        return false;
    }
    if(belongsToRoute(cityPointer, routeId)){
        return false;
    }
    City firstCity = map->routes[routeId]->first->city;
    CityList lastCityList =  map->routes[routeId]->first;
    while(lastCityList->next){
        lastCityList=lastCityList->next;
    }
    City lastCity = lastCityList->city;
    CityList fromLastCity = findPath(map, lastCity, cityPointer, routeId, NULL, NULL);
    CityList toFirstCity = findPath(map, cityPointer, firstCity, routeId, NULL, NULL);
    if(!fromLastCity && !toFirstCity){
        return false;
    }
    if(!toFirstCity){
        CityList proper = fromLastCity->next;
        free(fromLastCity);
        if(addRouteToCities(proper, routeId)){
            lastCityList->next=proper;
            return true;
        }
        freeCityList(proper);
        return false;
    }
    if(!fromLastCity){
        CityList iterator = toFirstCity;
        while(iterator->next->next){
            iterator = iterator->next;
        }
        free(iterator->next);
        iterator->next=NULL;
        if(addRouteToCities(toFirstCity, routeId)){
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
    while(iterator->next){
        Connection temp = getConnection(iterator->city, iterator->next->city);
        dist1+=temp->length;
        if(temp->year < oldest1){
            oldest1=temp->year;
        }
        iterator=iterator->next;
    }
    long dist2 = 0;
    int oldest2 = INT_MAX;
    iterator = toFirstCity;
    while(iterator->next){
        Connection temp = getConnection(iterator->city, iterator->next->city);
        dist2+=temp->length;
        if(temp->year < oldest2){
            oldest2=temp->year;
        }
        iterator=iterator->next;
    }
    if(dist1 < dist2 || (dist1==dist2 && oldest1 > oldest2)){
        freeCityList(toFirstCity);
        CityList proper = fromLastCity->next;
        free(fromLastCity);
        if(addRouteToCities(proper, routeId)){
            lastCityList->next=proper;
            return true;
        }
        freeCityList(proper);
        return false;
    }
    if(dist1 > dist2 || (dist1==dist2 && oldest2 > oldest1)){
        freeCityList(fromLastCity);
        iterator = toFirstCity;
        while(iterator->next->next){
            iterator = iterator->next;
        }
        free(iterator->next);
        iterator->next=NULL;
        if(addRouteToCities(toFirstCity, routeId)){
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
static bool areInOrder(Map *map, City city1, City city2, unsigned route){
    CityList iterator = map->routes[route]->first;
    while(iterator && (iterator->city != city1)){
        iterator = iterator->next;
    }
    return (iterator!=NULL) && (iterator->next) && iterator->next->city==city2;
}

bool removeRoad(Map *map, const char *city1, const char *city2){
    if(!map || !city1 || !city2){
        return false;
    }
    if(!verifyCityName(city1) || !verifyCityName(city2) || strcmp(city1, city2)==0){
        return false;
    }
    City city1Pointer = getCity(map->cities, city1);
    City city2Pointer = getCity(map->cities, city2);
    if(!city1Pointer || !city2Pointer){
        return false;
    }
    Connection exclude1 = getConnection(city1Pointer, city2Pointer);
    Connection exclude2 = getConnection(city2Pointer, city1Pointer);
    if(!exclude1 || !exclude2){
        return false;
    }
    unsigned toFix[NUMBER_OF_ROUTES];
    int numOfRoutes = 0;
    CityList found[NUMBER_OF_ROUTES];
    int direction[NUMBER_OF_ROUTES];
    for(unsigned i = MIN_ROUTE_NUM; i <= MAX_ROUTE_NUM; i++){
        if(map->routes[i]){
            if(areInOrder(map, city1Pointer, city2Pointer, i)){
                toFix[numOfRoutes]=i;
                direction[numOfRoutes]=1;
                found[numOfRoutes]=findPath(map, city1Pointer, city2Pointer, i, exclude1, exclude2);
                numOfRoutes++;
            }
            else if(areInOrder(map, city2Pointer, city1Pointer, i)){
                toFix[numOfRoutes]=i;
                direction[numOfRoutes]=2;
                found[numOfRoutes]=findPath(map, city2Pointer, city1Pointer, i, exclude1, exclude2);
                numOfRoutes++;
            }
        }
    }
    for(int i = 0; i < numOfRoutes; i++){
        if(!found[i]){
            for(int j = 0; j < numOfRoutes; j++){
                freeCityList(found[j]);
            }
            return false;
        }
        CityList temp = found[i];
        found[i]=found[i]->next;
        free(temp);
        temp = found[i];
        while(temp->next->next){
            temp = temp->next;
        }
        free(temp->next);
        temp->next=NULL;
    }
    for(int i = 0; i < numOfRoutes; i++){
        if(!addRouteToCities(found[i], toFix[i])){
            for(int j = 0; j < i; j++){
                removeRouteFromCities(found[j], toFix[j]);
            }
            return false;
        }
    }
    for(int i = 0; i < numOfRoutes; i++){
        if(direction[i] == 1){
            CityList iterator = map->routes[toFix[i]]->first;
            while(iterator->city!=city1Pointer){
                iterator=iterator->next;
            }
            CityList temp =iterator->next;
            iterator->next = found[i];
            while(iterator->next!=NULL){
                iterator = iterator->next;
            }
            iterator->next = temp;
        }
        if(direction[i] == 2){
            CityList iterator = map->routes[toFix[i]]->first;
            while(iterator->city!=city2Pointer){
                iterator=iterator->next;
            }
            CityList temp = iterator->next;
            iterator->next = found[i];
            while(iterator->next!=NULL){
                iterator = iterator->next;
            }
            iterator->next = temp;
        }
    }
    removeConnection(city1Pointer, city2Pointer);
    removeConnection(city2Pointer, city1Pointer);
    return true;
}

char const* getRouteDescription(Map *map, unsigned routeId){
    if(!map){
        return NULL;
    }
    char* result;
    if(routeId < MIN_ROUTE_NUM || routeId > MAX_ROUTE_NUM || !map->routes[routeId]){
        result = malloc(sizeof(char));
        if(!result){
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
    while(iterator->next){
        length++;// ;
        length+=strlen(iterator->city->name);
        length++;// ;
        Connection link = getConnection(iterator->city, iterator->next->city);
        length += (size_t) floor(log10(link->length));
        length+=2;//last char + ;
        if(link->year < 0){
            length++; //minus sign
        }
        length += (size_t) floor(log10(abs(link->year)));
        length++; //last char
        iterator = iterator->next;
    }
    length++; // ;
    length+=strlen(iterator->city->name);
    result = malloc(sizeof(char)*(length+1));
    if(!result){
        return NULL;
    }
    result[0]='\0';
    iterator = ourRoute->first;
    char intBuffer[14];
    sprintf(intBuffer, "%d;", routeId);
    strcat(result, intBuffer);
    while(iterator->next){
        strcat(result, iterator->city->name);
        Connection link = getConnection(iterator->city, iterator->next->city);
        sprintf(intBuffer, ";%d;", link->length);
        strcat(result, intBuffer);
        sprintf(intBuffer, "%d;", link->year);
        strcat(result, intBuffer);
        iterator=iterator->next;
    }
    strcat(result, iterator->city->name);
    return result;
}

/** @brief Funkcja weryfikująca, czy string spełnia warunki nazwy miasta*/
static bool verifyCityName(const char *city1){
    size_t length = strlen(city1);
    bool valid = true;
    for(size_t i = 0; i < length; i++){
        if((city1[i]>=0 && city1[i]<32 )|| city1[i]==59){
            valid = false;
            break;
        }
    }
    return valid&&length>0;

}