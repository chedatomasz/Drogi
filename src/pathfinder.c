/** @file
 * Implementacja klasy szukającej ścieżek w mapie
 *
 * @author Tomasz Cheda <tomasz.cheda@students.mimuw.edu.pl>
 * @date 29.04.2019
 */
#include <stddef.h>
#include <limits.h>
#include <stdlib.h>
#include "pathfinder.h"
#include "priority_queue.h"

/**
 * @brief Struktura reprezentująca stan wierzchołka grafu.
 */
typedef struct State* State;


/**
 * @brief Struktura przechowująca stan wierzchołka
 * grafu, który przeszukujemy
 */
struct State{
    City city; ///< Wskaźnik na miasto, które reprezentuje dany wezeł
    bool reached; ///< Flaga, czy osiągnięto już dany wierzchołek ścieżką ze źródła
    bool visited; ///< Flaga, czy odwiedzono już dany wierzchołek
    unsigned distance; ///< Zminimalizowana odległość od źródła
    int year; ///< Data ostatniego remontu, drugorzędowa cecha porządkująca
    bool sure; ///< Flaga, czy wierzchołek osiągnięto jednoznacznie wyznaczoną trasą
    int previous; ///< Numer porządkowy miasta, z którego osiągnięto węzeł najlepszą trasą.
};

/** @brief Znajduje ścieżkę między start a end w podanej mapie.
 * Używa algorytmu Dijkstry operującego na kopcu binarnym.
 * Elementy kolejki priorytetowej są usuwane leniwie.
* Unika routeToExclude, nie uwzględnia połączeń przez connectionToExclude.
* Zwraca wskaźnik na struct CityList reprezentujący optymalną trasę lub NULL,
* jeśli nie można wybrać ścieżki jednoznacznie lub nie udało się zaalokować pamięci.
* @param[in,out] map    – wskaźnik na przeszukiwana mapę;
* @param[in] start    – wskaźnik na struct City - początek ścieżki.
* @param[in] end    – wskaźnik na struct City - koniec ścieżki.
* @param[in] routeToExclude    – numer drogi krajowej, której elementy należy omijać
 * poza początkiem i końcem ścieżki.
* @param[in] connectionToExclude, connectionToExclude2    – wskaźniki na struct Connection
 * reprezentujący połączenie, którego nie należy używać przy wyszukiwaniu.
* @return Wskaźnik na struct CityList lub NULL, gdy nie znaleziono jednoznacznej ścieżki.
*/
CityList findPath(Map* map, City start, City end, unsigned routeToExclude, Connection connectionToExclude, Connection connectionToExclude2){
    PriorityQueue queue = newQueue();
    if(!queue){
        return NULL;
    }
    CityList first = getCityList(map);
    unsigned dist;
    size_t counter = 0;
    while(first != NULL){
        dist = INT_MAX/2;
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
    for(size_t i = 0; i < counter+1; i++){
        states[i].distance=INT_MAX/2;
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
            unsigned newDistance = states[current->number].distance+neighbors[i]->length;
            int newYear = states[current->number].year;
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