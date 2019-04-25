//
// Created by chedat on 25.04.19.
//

#ifndef DROGI_HASHMAP_H
#define DROGI_HASHMAP_H

#include <stdbool.h>
#include "city.h"

/**
 * @brief Struktura przechowująca dowolny void* i jego klucz - String.
   Ilość komórek jest potęgą 2 mniejszą od 2^32.
 */
typedef struct Hashmap* Hashmap;

/** @brief Tworzy nową strukturę.
 * Tworzy nową, pustą strukturę niezawierającą żadnych void* ani kluczy.
 * @return Wskaźnik na utworzoną strukturę lub NULL, gdy nie udało się
 * zaalokować pamięci.
 */
Hashmap newHashmap(unsigned int sizeInBits);

/** @brief Usuwa strukturę.
 * Usuwa strukturę wskazywaną przez @p hashmap oraz wszystkie zawarte w niej obiekty,
 * włącznie z zapisanymi w niej obiektami typu City.
 * Nic nie robi, jeśli wskaźnik ten ma wartość NULL.
 * @param[in] hashmap        – wskaźnik na usuwaną strukturę przechowującą City.
 */
void deleteCitiesHashmap(Hashmap hashmap);

/** @brief Znajduje City w podanej hashmapie.
* Zwraca wskaźnik na struct City przechowywany w hashmap o podanym kluczu
* lub NULL, jeśli taki obiekt nie znajduje się w hashmapie lub @p hashmap to NULL.
* @param[in] hashmap    – wskaźnik na przeszukiwana ashmapę;
* @param[in] city1    – wskaźnik na string, który jest kluczem szukanego obiektu.
* @return Wskaźnik na struct City lub NULL, gdy nie znaleziono obiektu.
*/
City getCity(Hashmap hashmap, const char* city1);

/** @brief Dodaje obiekt city do hashmapy.
 * Odnajduje odpowiednią komórkę hashmapy i tworzy nowy element listy zawierający City.
 * @param[in,out] hashmap    – wskaźnik na zmienianą hashmapę;
 * @param[in] city1      – wskaźnik na struct City;
 * @return Wartość @p true, jeśli modyfikacja się powiodła.
 * Wartość @p false, jeśli wystąpił błąd: @p hashmap lub @p city1 to NULL,
 * lub nie udało się zaalokować pamięci.
 */
bool addCity (Hashmap hashmap, City city1);

/** @brief Usuwa obiekt city z hashmapy.
 * Odnajduje odpowiednią komórkę hashmapy, zwalnia element listy i sam obiekt City.
 * @param[in,out] hashmap    – wskaźnik na zmienianą hashmapę;
 * @param[in] city1      – klucz usuwanego City;
 * @return Wartość @p true, jeśli modyfikacja się powiodła.
 * Wartość @p false, jeśli wystąpił błąd: @p hashmap to NULL,
 * lub @p hashmap nie zawiera obiektu o podanym kluczu.
 */
bool removeCity (Hashmap hashmap, const char* city1);

#endif //DROGI_HASHMAP_H
