//
// Created by chedat on 29.04.19.
//
#include <stddef.h>
#include <limits.h>
#include <stdlib.h>
#include "pathfinder.h"
#include "priority_queue.h"

CityList findPath(Map* map, City start, City end, int routeToExclude, Connection connectionToExclude, Connection connectionToExclude2){
    PriorityQueue queue = newQueue();
    if(!queue){
        return NULL;
    }
    CityList first = getCityList(map);
    int dist = INT_MAX;
    int counter = 0;
    while(first != NULL){
        dist = INT_MAX;
        if(first->city == start){
            dist = 0;
        }
        if(!insertPriorityQueue(queue, first->city, dist)){
            removePriorityQueue(queue);
            return NULL;
        }
        counter++;
        first = first->next;
    }
    City* cities = malloc(sizeof(City)*(counter+1));
    if(!cities){
        removePriorityQueue(queue);
        return NULL;
    }
    long* distance = malloc(sizeof(long)*(counter+1));
    if(!distance){
        removePriorityQueue(queue);
        free(cities);
        return NULL;
    }
    int* year = malloc(sizeof(int)*(counter+1));
    if(!year){
        removePriorityQueue(queue);
        free(cities);
        free(distance);
        return NULL;
    }
    bool* sure = malloc(sizeof(int)*(counter+1));
    if(!sure){
        removePriorityQueue(queue);
        free(cities);
        free(distance);
        free(year);
        return NULL;
    }
    int* previous = malloc(sizeof(int)*(counter+1));
    if(!previous){
        removePriorityQueue(queue);
        free(cities);
        free(distance);
        free(year);
        free(sure);
        return NULL;
    }
    bool* visited = malloc(sizeof(bool)*(counter+1));
    if(!visited){
        removePriorityQueue(queue);
        free(cities);
        free(distance);
        free(year);
        free(sure);
        free(previous);
        return NULL;
    }
    for(int i = 0; i < counter+1; i++){
        distance[i]=LONG_MAX;
        year[i]=INT_MAX;
        sure[i]=true;
        previous[i]=-1;
        visited[i] = false;
    }
    distance[start->number]=0;
    previous[start->number]=start->number;
    year[start->number]=INT_MAX;
    while(!isEmpty(queue)){
        City current = popMin(queue);
        if(visited[current->number]){
            continue;
        }
        cities[current->number] = current;
        visited[current->number] = true;
        if(!sure[current->number]){//Unsure cities are not a valid entry point for other nodes
            continue;
        }
        if(belongsToRoute(current, routeToExclude) && current!=start){
            continue;
        }
        Connection* neighbors = getAllConnections(current);
        for(int i = 0; i < current->numOfConnections; i++){
            if(neighbors[i]==connectionToExclude || neighbors[i]==connectionToExclude2){
                continue;
            }
            int secondCityNum = neighbors[i]->city2->number;
            long newDistance = distance[current->number]+neighbors[i]->length;
            long newYear = year[current->number];
            if(newYear>neighbors[i]->year){
                newYear=neighbors[i]->year;
            }
            if(newDistance < distance[secondCityNum]){//If path is shorter:
                distance[secondCityNum]=newDistance;
                insertPriorityQueue(queue, neighbors[i]->city2, newDistance);
                previous[secondCityNum]=current->number;
                year[secondCityNum]=newYear;
                sure[secondCityNum]=true;
                continue;
            }
            if(newDistance == distance[secondCityNum]){//Equal paths;
                if(year[secondCityNum]<newYear){//If new path is younger
                    year[secondCityNum]=newYear;
                    previous[secondCityNum]=current->number;
                    sure[secondCityNum]=true;
                }
                else if(year[secondCityNum]==newYear){//path of same age
                    sure[secondCityNum]=false;
                }
            }
        }
        free(neighbors);
    }
    if(!sure[end->number]){
        removePriorityQueue(queue);
        free(cities);
        free(distance);
        free(year);
        free(sure);
        free(previous);
        free(visited);
        removePriorityQueue(queue);
        return NULL;
    }
    if(!cities[end->number]){
        removePriorityQueue(queue);
        free(cities);
        free(distance);
        free(year);
        free(sure);
        free(previous);
        free(visited);
        removePriorityQueue(queue);
        return NULL;
    }
    int position = end->number;
    CityList last = NULL;
    bool ended = false;
    while(!ended){
        CityList temp = malloc(sizeof(struct CityList));
        if(!temp){
            freeCityList(last);
            removePriorityQueue(queue);
            free(cities);
            free(distance);
            free(year);
            free(sure);
            free(previous);
            free(visited);
            removePriorityQueue(queue);
            return NULL;
        }
        temp->city=cities[position];
        temp->next = last;
        last = temp;
        if(previous[position]==position){
            ended = true;
        }
        position = previous[position];
    }
    free(cities);
    free(distance);
    free(year);
    free(sure);
    free(previous);
    free(visited);
    removePriorityQueue(queue);
    return last;
}