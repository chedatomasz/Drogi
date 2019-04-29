//
// Created by chedat on 29.04.19.
//
#include <stddef.h>
#include <limits.h>
#include <stdlib.h>
#include "pathfinder.h"
#include "priority_queue.h"

typedef struct State* State;

struct State{
    City city;
    bool reached;
    bool visited;
    long distance;
    int year;
    bool sure;
    int previous;
};

CityList findPath(Map* map, City start, City end, int routeToExclude, Connection connectionToExclude, Connection connectionToExclude2){
    PriorityQueue queue = newQueue();
    if(!queue){
        return NULL;
    }
    CityList first = getCityList(map);
    long dist;
    int counter = 0;
    while(first != NULL){
        dist = LONG_MAX/2;
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
    State states= malloc(sizeof(struct State)*(counter+1));
    if(!states){
        removePriorityQueue(queue);
        return NULL;
    }
    for(int i = 0; i < counter+1; i++){
        states[i].distance=LONG_MAX/2;
        states[i].year=INT_MAX;
        states[i].sure=true;
        states[i].previous=-1;
        states[i].visited = false;
        states[i].reached = false;
    }
    states[start->number].distance=0;
    states[start->number].previous=start->number;
    states[start->number].year=INT_MAX;
    states[start->number].reached=true;
    while(!isEmpty(queue)){
        City current = popMin(queue);
        if(states[current->number].visited || !states[current->number].reached || !states[current->number].sure){
            continue;
        }
        states[current->number].city = current;
        states[current->number].visited = true;
        if(belongsToRoute(current, routeToExclude) && current!=start){
            continue;
        }
        Connection* neighbors = getAllConnections(current);
        for(int i = 0; i < current->numOfConnections; i++){
            if(neighbors[i]==connectionToExclude || neighbors[i]==connectionToExclude2){
                continue;
            }
            int secondCityNum = neighbors[i]->city2->number;
            long newDistance = states[current->number].distance+neighbors[i]->length;
            long newYear = states[current->number].year;
            if(newYear>neighbors[i]->year){
                newYear=neighbors[i]->year;
            }
            if(!states[secondCityNum].reached || newDistance < states[secondCityNum].distance){//If path is shorter:
                states[secondCityNum].distance=newDistance;
                insertPriorityQueue(queue, neighbors[i]->city2, newDistance);
                states[secondCityNum].previous=current->number;
                states[secondCityNum].year=newYear;
                states[secondCityNum].sure=true;
                states[secondCityNum].reached=true;
                continue;
            }
            if(newDistance == states[secondCityNum].distance){//Equal paths;
                if(states[secondCityNum].year<newYear){//If new path is younger
                    states[secondCityNum].year=newYear;
                    states[secondCityNum].previous=current->number;
                    states[secondCityNum].sure=true;
                }
                else if(states[secondCityNum].year==newYear){//path of same age
                    states[secondCityNum].sure=false;
                }
            }
        }
        free(neighbors);
    }
    if(!states[end->number].reached || !states[end->number].sure){
        removePriorityQueue(queue);
        free(states);
        return NULL;
    }
    int position = end->number;
    CityList last = NULL;
    bool ended = false;
    while(!ended){
        if(!states[position].sure){
            freeCityList(last);
            removePriorityQueue(queue);
            free(states);
            return NULL;
        }
        CityList temp = malloc(sizeof(struct CityList));
        if(!temp){
            freeCityList(last);
            removePriorityQueue(queue);
            free(states);
            return NULL;
        }
        temp->city=states[position].city;
        temp->next = last;
        last = temp;
        if(states[position].previous==position){
            ended = true;
        }
        position = states[position].previous;
    }
    free(states);
    removePriorityQueue(queue);
    return last;
}