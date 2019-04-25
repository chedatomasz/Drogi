/** @file
 * Implementacja klasy przechowującej mapę dróg krajowych
 *
 * @author Tomasz Cheda <tomasz.cheda@students.mimuw.edu.pl>
 * @date 24.04.2019
 */
#define NUMBER_OF_ROUTES 1000

#include <stddef.h>
#include <string.h>
#include "map.h"
#include "route.h"
#include "hashmap.h"


struct Map {
    Route routes[NUMBER_OF_ROUTES];
    Hashmap cities;
};

Map* newMap(void){
    return NULL;
}


void deleteMap(Map* map){

}

static bool verifyCityName(const char *city);

bool addRoad(Map *map, const char *city1, const char *city2, unsigned length, int builtYear){
    if(!map){
        return false;
    }
    if(!verifyCityName(city1) || !verifyCityName(city2)){
        return false;
    }
    if(strcmp(city1, city2)==0){
        return false;
    }
    bool firstCityAdded = false;
    bool secondCityAdded = false;
    City city1Pointer = getCity(map->cities, city1);
    if(!city1Pointer){
        city1Pointer = newCity(city1);
        if(!city1Pointer){
            return false;
        }
        if (!addCity(map->cities, city1Pointer)){
            freeCity(city1Pointer);
            return false;
        }
        firstCityAdded = true;
    }
    City city2Pointer = getCity(map->cities, city2);
    if(!city2Pointer){
        city2Pointer = newCity(city1);
        if(!city2Pointer){
            if(firstCityAdded){
                removeCity(map->cities, city1);
            }
            return false;
        }
        if(!addCity(map->cities, city2Pointer)){
            if(firstCityAdded){
                removeCity(map->cities, city1);
                freeCity(city2Pointer);
            }
            return false;
        }
        secondCityAdded = true;
    }
    //sprawdz czy odcinek juz istnieje
    //jak nie sprobuj dodac
    //jak sie nie uda usun zaalokowane miasta

}

bool repairRoad(Map *map, const char *city1, const char *city2, int repairYear){

}

bool newRoute(Map *map, unsigned routeId, const char *city1, const char *city2){

}

bool extendRoute(Map *map, unsigned routeId, const char *city){

}

bool removeRoad(Map *map, const char *city1, const char *city2){

}

char const* getRouteDescription(Map *map, unsigned routeId){

}

static bool verifyCityName(const char *city1){
    int length = strlen(city1);
    bool valid = true;
    for(int i = 0; i < length; i++){
        if(city1[i]<32 || city1[i]==59){
            valid = false;
            break;
        }
    }
    return valid&&length>0;

}