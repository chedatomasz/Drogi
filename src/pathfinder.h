//
// Created by chedat on 29.04.19.
//
#ifndef DROGI_PATHFINDER_H
#define DROGI_PATHFINDER_H

#include "route.h"
#include "connection_tree.h"
#include "map.h"


CityList findPath(Map* map, City start, City end, int routeToExclude, Connection connectionToExclude, Connection connectionToExclude2);

#endif //DROGI_PATHFINDER_H
