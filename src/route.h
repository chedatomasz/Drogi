//
// Created by chedat on 25.04.19.
//

#ifndef DROGI_ROUTE_H
#define DROGI_ROUTE_H

/**
 * Struktura przechowująca informacje o pojedynczej drodze krajowej.
 */
typedef struct Route* Route;
typedef struct RouteNumber* RouteNumber;
typedef struct CityList* CityList;
typedef struct City* City;
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

void freeCityList(CityList list);

void freeRoute(Route route);
#endif //DROGI_ROUTE_H
