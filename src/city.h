//
// Created by chedat on 25.04.19.
//

#include "hashmap.h"

#ifndef DROGI_CITY_H
#define DROGI_CITY_H

typedef struct City* City;

struct City {
    char* name;
};

void freeCity(City city);

#endif //DROGI_CITY_H
