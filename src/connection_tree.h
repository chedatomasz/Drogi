//
// Created by chedat on 25.04.19.
//

#ifndef DROGI_CONNECTION_BST_H
#define DROGI_CONNECTION_BST_H

#include <stdbool.h>
#include "city.h"

typedef struct ConnectionTree* ConnectionTree;
typedef struct Connection* Connection;

struct Connection {
    int length;
    int year;
    City city2;
};

bool addConnection(City city1, City city2, int length, int builtYear);

Connection getConnection(City city1, City city2);

//returns null if mem cant be allocd for result
Connection* getAllConnections(City city1);

void freeConnectionTree(ConnectionTree node);

#endif //DROGI_CONNECTION_BST_H
