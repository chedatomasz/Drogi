#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include "map.h"
#include "input.h"

#define COMMAND_ADD_ROAD 1
#define COMMAND_REPAIR_ROAD 2
#define COMMAND_GET_ROUTE_DESCRIPTION 3
#define COMMAND_ADD_ROUTE 4
#define COMMAND_INVALID 5

#define EXECUTION_SUCCESSFUL 0
#define EXECUTION_FAILED 1

Map *map;

/**
 * Zwalnia pamięć używaną przez program.
 */
void cleanUp() {
    deleteMap(map);
}

/**
 * @brief Wypisuje informację o błędzie na standardowe wyjście diagnostyczne
 * @param line numer linii wejścia w której wystąpił błąd, numerowany od 1 i uwzględniający pomijane.
 */
void printError(int line) {
    fprintf(stderr, "ERROR %d\n", line);
}

/**
 * @brief Rozpoznaje typ komendy w line, jednak nie sprawdza dalszej poprawności składniowej.
 * @param line C-style string zawierający komendę lub NULL
 * @return kod rozpoznanej komendy lub COMMAND_INVALID, gdy komenda niepoprawna/line to null.
 */
int getCommand(char *line) {
    if (!line || strlen(line) == 0 || line[strlen(line)-1]==';') {
        //tests for the special case of the command ending with ; which is not taken care of by the tokenizer
        return COMMAND_INVALID;
    }
    if (strstr(line, "addRoad") == line) {//line begins with addRoad
        return COMMAND_ADD_ROAD;
    }
    if (strstr(line, "repairRoad") == line) {//line begins with repairRoad
        return COMMAND_REPAIR_ROAD;
    }
    if (strstr(line, "getRouteDescription") == line) {//line begins with getRouteDescription
        return COMMAND_GET_ROUTE_DESCRIPTION;
    }
    if (isdigit(line[0])){
        return COMMAND_ADD_ROUTE;
    }
    return COMMAND_INVALID;
}

int executeAddRoad(char* line){
    char* copy = line;
    char* first = tokenize(&copy, ';');
    char* city1 = tokenize(&copy, ';');
    char* city2 = tokenize(&copy, ';');
    char* length = tokenize(&copy, ';');
    char* builtYear = tokenize(&copy, ';');
    if(strcmp(first, "addRoad") != 0 || !city1 || !city2 || !length || !builtYear || tokenize(&copy, ';')){
        return EXECUTION_FAILED;
    }
    unsigned lengthInt = getUnsignedFromString(length);
    if(errno){
        return EXECUTION_FAILED;
    }
    int builtYearInt = getIntFromString(builtYear);
    if(errno){
        return EXECUTION_FAILED;
    }
    bool result = addRoad(map, city1, city2, lengthInt, builtYearInt);
    return result? EXECUTION_SUCCESSFUL : EXECUTION_FAILED;
}

int executeRepairRoad(char* line){
    char* copy = line;
    char* first = tokenize(&copy, ';');
    char* city1 = tokenize(&copy, ';');
    char* city2 = tokenize(&copy, ';');
    char* repairYear = tokenize(&copy, ';');
    if(strcmp(first, "repairRoad") != 0 || !city1 || !city2 || !repairYear || tokenize(&copy, ';')){
        return EXECUTION_FAILED;
    }
    int repairYearInt = getIntFromString(repairYear);
    if(errno){
        return EXECUTION_FAILED;
    }
    bool result = repairRoad(map, city1, city2, repairYearInt);
    return result? EXECUTION_SUCCESSFUL : EXECUTION_FAILED;
}

int executeGetRouteDescription(char* line){
    char* copy = line;
    char* first = tokenize(&copy, ';');
    char* routeNumber = tokenize(&copy, ';');
    if(strcmp(first, "getRouteDescription") != 0 || !routeNumber || tokenize(&copy, ';')){
        return EXECUTION_FAILED;
    }
    unsigned routeNumberInt = getUnsignedFromString(routeNumber);
    if(errno){
        return EXECUTION_FAILED;
    }
    char const* result = getRouteDescription(map, routeNumberInt);
    if(result){
        printf("%s\n", result);
        free((void*)result);//Knowingly discarding the const qualifier.
    }
    return result? EXECUTION_SUCCESSFUL : EXECUTION_FAILED;
}
/** W przypadku błędów w dalszej części polecenia, wykonuje zmiany na mapie wynikające z wcześniejszej części*/
int executeAddRoute(char* line){
    char* copy = line;
    char* first = tokenize(&copy, ';');
    char* city1 = tokenize(&copy, ';');
    if(!first || !city1){
        return EXECUTION_FAILED;
    }
    unsigned routeNumber = getUnsignedFromString(first);
    if(errno || !verifyRouteNumber(map, routeNumber) || !verifyCityName(city1)){
        return EXECUTION_FAILED;
    }
    CityList listOfCities = addCityToCityList(map, NULL, city1);
    if(!listOfCities){
        return EXECUTION_FAILED;
    }
    char* length;
    char* builtYear;
    char* city2;
    while(1){
        length = tokenize(&copy, ';');
        builtYear = tokenize(&copy, ';');
        city2 = tokenize(&copy, ';');
        if(!length){//input has ended
            break;
        }
        //input ends mid-sequence
        if(!builtYear || !city2){
            removeCityList(listOfCities);
            return EXECUTION_FAILED;
        }
        unsigned lengthInt = getUnsignedFromString(length);
        if(errno){
            removeCityList(listOfCities);
            return EXECUTION_FAILED;
        }
        int yearInt = getIntFromString(builtYear);
        if(errno){
            removeCityList(listOfCities);
            return  EXECUTION_FAILED;
        }
        if(!verifyCityName(city2)){
            removeCityList(listOfCities);
            return EXECUTION_FAILED;
        }
        int status = roadStatus(map, city1, city2, lengthInt, yearInt);

        if(status == ROAD_CONFLICTING){
            removeCityList(listOfCities);
            return EXECUTION_FAILED;
        }

        if(status == ROAD_NOT_FOUND){
            if(!addRoad(map, city1, city2, lengthInt, yearInt)){
                removeCityList(listOfCities);
                return EXECUTION_FAILED;
            }
        }

        if(status == ROAD_TO_REPAIR){
            if(!repairRoad(map, city1, city2, yearInt)){
                removeCityList(listOfCities);
                return EXECUTION_FAILED;
            }
        }

        if(!addCityToCityList(map, listOfCities, city2)){
            removeCityList(listOfCities);
            return EXECUTION_FAILED;
        }

        city1=city2;
    }
    if(!addRouteExplicit(map, routeNumber, listOfCities)){
        return EXECUTION_FAILED;
    }
    return EXECUTION_SUCCESSFUL;
}

int executeCommand(int commandNumber, char* line){
    if(commandNumber == COMMAND_INVALID){
        if(line){
            free(line);
        }
        return EXECUTION_FAILED;
    }
    if(commandNumber == COMMAND_ADD_ROAD){
        int result = executeAddRoad(line);
        free(line);
        return result;
    }
    if(commandNumber == COMMAND_REPAIR_ROAD){
        int result = executeRepairRoad(line);
        free(line);
        return result;
    }
    if(commandNumber == COMMAND_GET_ROUTE_DESCRIPTION){
        int result = executeGetRouteDescription(line);
        free(line);
        return result;
    }
    if(commandNumber == COMMAND_ADD_ROUTE){
        int result = executeAddRoute(line);
        free(line);
        return result;
    }
    return EXECUTION_FAILED;
}

int main(void) {
    map = newMap();
    if (!map) {
        return 0;
    }
    if (atexit(cleanUp) != 0) {
        return 0;
    }
    int lineNumber = 1;
    char *currentLine = NULL;
    while (peek() != EOF) {
        if (peek() == (int) '\n' || peek() == (int) '#') {
            readToLineEnd();
        } else {
            currentLine = readInNextLine();
            if(executeCommand(getCommand(currentLine), currentLine) == EXECUTION_FAILED){
                printError(lineNumber);
            }
        }
        lineNumber++;
    }
    return 0;
}