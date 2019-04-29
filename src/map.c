/** @file
 * Implementacja klasy przechowującej mapę dróg krajowych
 *
 * @author Tomasz Cheda <tomasz.cheda@students.mimuw.edu.pl>
 * @date 24.04.2019
 */
#define NUMBER_OF_ROUTES 1000
#define HASHMAP_SIZE_BITS 20
#define MIN_ROUTE_NUM 1
#define MAX_ROUTE_NUM 999
#define EXAMPLE_INVALID_ROUTE -1

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "map.h"
#include "route.h"
#include "hashmap.h"
#include "connection_tree.h"
#include "pathfinder.h"

static bool verifyCityName(const char *city);

struct Map {
    Route routes[NUMBER_OF_ROUTES];
    Hashmap cities;
    CityList allCities;
    int numOfCities;
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

void addCityToList(Map* map, CityList list){
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

bool addRoad(Map *map, const char *city1, const char *city2, unsigned length, int builtYear){
    if(!map || !city1 || !city2){
        return false;
    }
    if(!verifyCityName(city1) || !verifyCityName(city2) || builtYear == 0 || strcmp(city1, city2)==0){
        return false;
    }
    char* ourCity1 = malloc(sizeof(char)*(strlen(city1)+1));
    if(!ourCity1){
        return false;
    }
    char* ourCity2 = malloc(sizeof(char)*(strlen(city2)+1));
    if(!ourCity2){
        free(ourCity1);
        return false;
    }
    strcpy(ourCity1, city1);
    strcpy(ourCity2, city2);
    CityList list1 = NULL;
    CityList list2 = NULL;
    bool firstCityAdded = false;
    bool secondCityAdded = false;
    City city1Pointer = getCity(map->cities, ourCity1);
    if(!city1Pointer){
        list1 = malloc(sizeof(struct CityList));
        if(!list1){
            return false;
        }
        city1Pointer = newCity(ourCity1);
        if(!city1Pointer){
            return false;
        }
        if (!addCity(map->cities, city1Pointer)){
            freeCity(city1Pointer);
            return false;
        }
        firstCityAdded = true;
    }
    City city2Pointer = getCity(map->cities, ourCity2);
    if(!city2Pointer){
        list2 = malloc(sizeof(struct CityList));
        if(!list2){
            return false;
        }
        city2Pointer = newCity(ourCity2);
        if(!city2Pointer){
            if(firstCityAdded){
                removeCity(map->cities, ourCity1);
                free(list1);
            }
            return false;
        }
        if(!addCity(map->cities, city2Pointer)){
            if(firstCityAdded){
                removeCity(map->cities, ourCity1);
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
    map->routes[routeId]->first=findPath(map, city1Pointer, city2Pointer, EXAMPLE_INVALID_ROUTE, NULL);
    if(map->routes[routeId]->first){
        return true;
    }
    else{
        free(map->routes[routeId]);
        map->routes[routeId]=NULL;
    }
    return false;
}

bool extendRoute(Map *map, unsigned routeId, const char *city){

}

bool removeRoad(Map *map, const char *city1, const char *city2){

}

char const* getRouteDescription(Map *map, unsigned routeId){
    if(!map || routeId < MIN_ROUTE_NUM || routeId > MIN_ROUTE_NUM){
        return false;
    }
    Route ourRoute = map->routes[routeId];
    char* result;
    if(!ourRoute){//empty string
        result = malloc(sizeof(char));
        if(!result){
            return NULL;
        }
        *result = '\0';
        return result;
    }
    int length = 0;
}

static bool verifyCityName(const char *city1){
    int length = strlen(city1);
    bool valid = true;
    for(int i = 0; i < length; i++){
        if((city1[i]>=0 && city1[i]<32 )|| city1[i]==59){
            valid = false;
            break;
        }
    }
    return valid&&length>0;

}