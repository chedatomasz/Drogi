//
// Created by chedat on 25.04.19.
//

#ifndef DROGI_ROUTE_H
#define DROGI_ROUTE_H

/**
 * Struktura przechowująca informacje o pojedynczej drodze krajowej.
 */
typedef struct Route* Route;
/**
 * @brief Lista dróg krajowych przechodzących przez węzeł.
 * Przeznaczona do przechowywania przez miasto.
 */
typedef struct RouteNumber* RouteNumber;
/**
 * Lista miast - przechowuje ścieżkę.
 */
typedef struct CityList* CityList;
/**
 * Deklaracja aby uniknąć kołowego załączenia nagłówków.
 */
typedef struct City* City;

/**
 * @brief Lista dróg krajowych przechodzących przez węzeł.
 * Przeznaczona do przechowywania przez miasto.
 */
struct RouteNumber{
    int number; ///< Numer drogi krajowej.
    RouteNumber next; ///< Wskaźnik na kolejny element listy.
};
/**
 * @brief Lista miast przechowująca ścieżkę.
 * Można założyć, że elementy znajdujące się kolejno na CityList są sąsiadami
 */
struct CityList{
    City city; ///< Wskaźnik na miasto.
    CityList  next;///< Wskaźnik na kolejny element listy.
};
/**
 * @brief Struktura przechowująca informacje o pojedynczej drodze krajowej.
 * Wrapper na CityList.
 */
struct Route{
    CityList first;///< Pierwszy element ścieżki.
};
/**
 * @brief Zwalnia CityList wskazywane przez list.
 * Nie zwalnia przechowywanych w węzłach miast.
 * @param[in] list - wskaźnik na listę do zwolnienia.
 */
void freeCityList(CityList list);

/**
 * @brief Zwalnia Route wskazywane przez route.
 * Nie zwalnia przechowywanych w wewnętrznych węzłach miast.
 * @param[in] list - wskaźnik na drogę do zwolnienia.
 */
void freeRoute(Route route);
#endif //DROGI_ROUTE_H
