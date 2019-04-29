//
// Created by chedat on 25.04.19.
//

#ifndef DROGI_ROUTE_H
#define DROGI_ROUTE_H

#include "city.h"

/**
 * Struktura przechowująca informacje o pojedynczej drodze krajowej.
 */
typedef struct Route* Route;
typedef struct RouteNumber* RouteNumber;
typedef struct CityList* CityList;
struct RouteNumber{
    int number;
    RouteNumber next;
};
struct CityList{
    City city;
    CityList  next;
};
struct Route{
    CityList first;
};

void freeCityList(CityList list) {
    while(list){
        CityList temp = list->next;
        free(list);
        list = temp;
    }
}

void freeRoute(Route route) {
    if(route){
        freeCityList(route->first);
    }
}
#endif //DROGI_ROUTE_H
