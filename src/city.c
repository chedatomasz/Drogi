#include <stdlib.h>
#include "city.h"
#include "connection_tree.h"

//
// Created by chedat on 25.04.19.
//
void freeCity(City city) {
    freeConnectionTree(city->root);
    free(city->name);
    while (city->routes) {
        RouteNumber temp = city->routes->next;
        free(city->routes);
        city->routes = temp;
    }
    free(city);
}

City newCity(char *name) {
    City result = malloc(sizeof(struct City));
    if (!result) {
        return NULL;
    }
    result->name = name;
    result->root = NULL;
    result->numOfConnections = 0;
    result->routes = NULL;
    result->number = 0;
    return result;
}

bool belongsToRoute(City city, unsigned route) {
    if (!city) {
        return false;
    }
    RouteNumber number = city->routes;
    while (number && number->number != route) {
        number = number->next;
    }
    return (number != NULL);
}

bool addToRoute(City city, unsigned route) {
    if (!city) {
        return false;
    }
    if (belongsToRoute(city, route)) {
        return false;
    }
    RouteNumber new = malloc(sizeof(struct RouteNumber));
    if (!new) {
        return false;
    }
    new->next = NULL;
    new->number = route;
    if (!city->routes) {
        city->routes = new;
        return true;
    }
    RouteNumber insertion = city->routes;
    while (insertion->next) {
        insertion = insertion->next;
    }
    insertion->next = new;
    return true;
}

void removeFromRoute(City city, unsigned route) {
    if (!city) {
        return;
    }
    if (!belongsToRoute(city, route)) {
        return;
    }
    if (city->routes->number == route) {
        RouteNumber temp = city->routes;
        city->routes = temp->next;
        free(temp);
        return;
    }
    RouteNumber insertion = city->routes;
    while (insertion->next->number != route) {
        insertion = insertion->next;
    }
    RouteNumber temp = insertion->next;
    insertion->next = temp->next;
    free(temp);
}
