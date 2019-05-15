/** @file
 * Interfejs klasy szukającej ścieżek w mapie
 *
 * @author Tomasz Cheda <tomasz.cheda@students.mimuw.edu.pl>
 * @date 29.04.2019
 */
#ifndef DROGI_PATHFINDER_H
#define DROGI_PATHFINDER_H

#include "route.h"
#include "connection_tree.h"
#include "map.h"

/** @brief Znajduje ścieżkę między start a end w podanej mapie.
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
CityList findPath(Map* map, City start, City end, unsigned routeToExclude, Connection connectionToExclude, Connection connectionToExclude2);

#endif //DROGI_PATHFINDER_H
