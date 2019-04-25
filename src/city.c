#include <stdlib.h>
#include "city.h"
#include "connection_tree.h"

//
// Created by chedat on 25.04.19.
//
void freeCity(City city){
    freeConnectionTree(city->root);
    free(city);
}
City newCity(const char* name){
    City result = malloc(sizeof(struct City));
    if(!result){
        return NULL;
    }
    result->name = name;
    result->root = NULL;
    return result;
}