//
// Created by chedat on 25.04.19.
//

#ifndef DROGI_HASHMAP_H
#define DROGI_HASHMAP_H

#include "city.h"

typedef struct Hashmap* Hashmap;

City getCity(Hashmap hashmap, const char* city1);

Hashmap newHashmap(unsigned int sizeInBits);

#endif //DROGI_HASHMAP_H
