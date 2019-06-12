//
// Created by chedat on 29.04.19.
//
#include "route.h"
#include <stdlib.h>

void freeCityList(CityList list) {
    while (list) {
        CityList temp = list->next;
        free(list);
        list = temp;
    }
}

void freeRoute(Route route) {
    if (route) {
        freeCityList(route->first);
        free(route);
    }
}
