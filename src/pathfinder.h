//
// Created by chedat on 29.04.19.
//
#include "route.h"
#include "connection_tree.h"

#ifndef DROGI_PATHFINDER_H
#define DROGI_PATHFINDER_H

Route findPath(City start, City end, int routeToExclude, Connection connectionToExclude);

#endif //DROGI_PATHFINDER_H
