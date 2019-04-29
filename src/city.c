#include <stdlib.h>
#include "city.h"
#include "connection_tree.h"

//
// Created by chedat on 25.04.19.
//
void freeCity(City city){
    freeConnectionTree(city->root);
    free(city->name);
    free(city);
    while(city->routes){
        RouteNumber temp = city->routes->next;
        free(city->routes);
        city->routes = temp;
    }
}
City newCity(char* name){
    City result = malloc(sizeof(struct City));
    if(!result){
        return NULL;
    }
    result->name = name;
    result->root = NULL;
    result->numOfConnections = 0;
    result->routes = NULL;
    result->number = 0;
    return result;
}

bool belongsToRoute(City city, int route){
    if(!city){
        return false;
    }
    RouteNumber number= city->routes;
    while(number && number->number!=route){
        number=number->next;
    }
    return (number!=NULL);
}

bool addToRoute(City city, int route){
    if(!city){
        return false;
    }
    if(belongsToRoute(city, route)){
        return false;
    }
    RouteNumber new = malloc(sizeof(struct RouteNumber));
    if(!new){
        return false;
    }
    new->next = NULL;
    new->number = route;
    if(!city->routes){
        city->routes = new;
        return true;
    }
    RouteNumber insertion = city->routes;
    while(insertion->next){
        insertion = insertion->next;
    }
    insertion->next = new;
    return true;
}
