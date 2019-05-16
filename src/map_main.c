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
    //converting length from string to uint and checking bounds
    char* end = NULL;
    errno = 0;
    unsigned long lengthLong = strtoul(length, &end, 10);
    if(errno || *end != '\0' || lengthLong > UINT_MAX){
        return EXECUTION_FAILED;
    }
    //converting builtYear from string to int and checking bounds
    errno = 0;
    long builtYearLong = strtol (builtYear, &end, 10);
    if(errno || *end != '\0' || builtYearLong > INT_MAX || builtYearLong < INT_MIN){
        return EXECUTION_FAILED;
    }
    bool result = addRoad(map, city1, city2, (unsigned) lengthLong, (int) builtYearLong);
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

    //converting repairYear from string to int and checking bounds
    char* end = NULL;
    errno = 0;
    long repairYearLong = strtol (repairYear, &end, 10);
    if(errno || *end != '\0' || repairYearLong > INT_MAX || repairYearLong < INT_MIN){
        return EXECUTION_FAILED;
    }
    bool result = repairRoad(map, city1, city2, (int) repairYearLong);
    return result? EXECUTION_SUCCESSFUL : EXECUTION_FAILED;
}

int executeGetRouteDescription(char* line){
    char* copy = line;
    char* first = tokenize(&copy, ';');
    char* routeNumber = tokenize(&copy, ';');
    if(strcmp(first, "getRouteDescription") != 0 || !routeNumber || tokenize(&copy, ';')){
        return EXECUTION_FAILED;
    }

    //converting routeNumber from string to uint and checking bounds
    char* end = NULL;
    errno = 0;
    unsigned long routeNumberLong = strtoul (routeNumber, &end, 10);
    if(errno || *end != '\0' || routeNumberLong > UINT_MAX){
        return EXECUTION_FAILED;
    }
    char const* result = getRouteDescription(map, (unsigned) routeNumberLong);
    if(result){
        printf("%s\n", result);
        free((void*)result);//Knowingly discarding the const qualifier.
    }
    return result? EXECUTION_SUCCESSFUL : EXECUTION_FAILED;
}

int executeAddRoute(char* line){

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