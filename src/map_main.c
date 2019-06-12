#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include "map.h"
#include "input.h"

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

int executeAddRoad(char *line) {
    char *copy = line;
    char *first = tokenize(&copy, ';');
    char *city1 = tokenize(&copy, ';');
    char *city2 = tokenize(&copy, ';');
    char *length = tokenize(&copy, ';');
    char *builtYear = tokenize(&copy, ';');
    if (strcmp(first, "addRoad") != 0 || !city1 || !city2 || !length || !builtYear || tokenize(&copy, ';')) {
        return EXECUTION_FAILED;
    }
    unsigned lengthInt = getUnsignedFromString(length);
    if (errno) {
        return EXECUTION_FAILED;
    }
    int builtYearInt = getIntFromString(builtYear);
    if (errno) {
        return EXECUTION_FAILED;
    }
    bool result = addRoad(map, city1, city2, lengthInt, builtYearInt);
    return result ? EXECUTION_SUCCESSFUL : EXECUTION_FAILED;
}

int executeRepairRoad(char *line) {
    char *copy = line;
    char *first = tokenize(&copy, ';');
    char *city1 = tokenize(&copy, ';');
    char *city2 = tokenize(&copy, ';');
    char *repairYear = tokenize(&copy, ';');
    if (strcmp(first, "repairRoad") != 0 || !city1 || !city2 || !repairYear || tokenize(&copy, ';')) {
        return EXECUTION_FAILED;
    }
    int repairYearInt = getIntFromString(repairYear);
    if (errno) {
        return EXECUTION_FAILED;
    }
    bool result = repairRoad(map, city1, city2, repairYearInt);
    return result ? EXECUTION_SUCCESSFUL : EXECUTION_FAILED;
}

int executeGetRouteDescription(char *line) {
    char *copy = line;
    char *first = tokenize(&copy, ';');
    char *routeNumber = tokenize(&copy, ';');
    if (strcmp(first, "getRouteDescription") != 0 || !routeNumber || tokenize(&copy, ';')) {
        return EXECUTION_FAILED;
    }
    unsigned routeNumberInt = getUnsignedFromString(routeNumber);
    if (errno) {
        return EXECUTION_FAILED;
    }
    char const *result = getRouteDescription(map, routeNumberInt);
    if (result) {
        printf("%s\n", result);
        free((void *) result);//Knowingly discarding the const qualifier.
    }
    return result ? EXECUTION_SUCCESSFUL : EXECUTION_FAILED;
}

/** W przypadku błędów w dalszej części polecenia, wykonuje zmiany na mapie wynikające z wcześniejszej części*/
int executeAddRoute(char *line) {
    bool result = addRoute(map, line);
    return result ? EXECUTION_SUCCESSFUL : EXECUTION_FAILED;
}

int executeNewRoute(char *line) {
    char *copy = line;
    char *command = tokenize(&copy, ';');
    char *routeId = tokenize(&copy, ';');
    char *city1 = tokenize(&copy, ';');
    char *city2 = tokenize(&copy, ';');
    if (strcmp(command, "newRoute") != 0 || !city1 || !city2 || !routeId || tokenize(&copy, ';')) {
        return EXECUTION_FAILED;
    }
    unsigned routeIdInt = getUnsignedFromString(routeId);
    if (errno) {
        return EXECUTION_FAILED;
    }
    bool result = newRoute(map, routeIdInt, city1, city2);
    return result ? EXECUTION_SUCCESSFUL : EXECUTION_FAILED;
}

int executeExtendRoute(char *line) {
    char *copy = line;
    char *command = tokenize(&copy, ';');
    char *routeId = tokenize(&copy, ';');
    char *city1 = tokenize(&copy, ';');
    if (strcmp(command, "extendRoute") != 0 || !city1 || !routeId || tokenize(&copy, ';')) {
        return EXECUTION_FAILED;
    }
    unsigned routeIdInt = getUnsignedFromString(routeId);
    if (errno) {
        return EXECUTION_FAILED;
    }
    bool result = extendRoute(map, routeIdInt, city1);
    return result ? EXECUTION_SUCCESSFUL : EXECUTION_FAILED;
}

int executeRemoveRoad(char *line) {
    char *copy = line;
    char *command = tokenize(&copy, ';');
    char *city1 = tokenize(&copy, ';');
    char *city2 = tokenize(&copy, ';');
    if (strcmp(command, "removeRoad") != 0 || !city1 || !city2 || tokenize(&copy, ';')) {
        return EXECUTION_FAILED;
    }
    bool result = removeRoad(map, city1, city2);
    return result ? EXECUTION_SUCCESSFUL : EXECUTION_FAILED;
}

int executeRemoveRoute(char *line) {
    char *copy = line;
    char *command = tokenize(&copy, ';');
    char *routeId = tokenize(&copy, ';');
    if (strcmp(command, "removeRoute") != 0 || !routeId || tokenize(&copy, ';')) {
        return EXECUTION_FAILED;
    }
    unsigned routeIdInt = getUnsignedFromString(routeId);
    if (errno) {
        return EXECUTION_FAILED;
    }
    bool result = removeRoute(map, routeIdInt);
    return result ? EXECUTION_SUCCESSFUL : EXECUTION_FAILED;
}

/**
 * @brief Rozpoznaje i wykonuje komendę zawartą w linii
 * @param line C-style string zawierający komendę lub NULL
 * @return EXECUTION_FAILED lub EXECUTION_SUCCESSFUL zależnie od wyniku.
 */
int getAndExecuteCommand(char *line) {
    if (!line || strlen(line) == 0 || line[strlen(line) - 1] == ';') {
        //tests for the special case of the command ending with ; which is not taken care of by the tokenizer
        if (line) {
            free(line);
        }
        return EXECUTION_FAILED;
    }
    if (strstr(line, "addRoad") == line) {//line begins with addRoad
        int result = executeAddRoad(line);
        free(line);
        return result;
    }
    if (strstr(line, "repairRoad") == line) {//line begins with repairRoad
        int result = executeRepairRoad(line);
        free(line);
        return result;
    }
    if (strstr(line, "getRouteDescription") == line) {//line begins with getRouteDescription
        int result = executeGetRouteDescription(line);
        free(line);
        return result;
    }
    if (strstr(line, "newRoute") == line) {//line begins with newRoute
        int result = executeNewRoute(line);
        free(line);
        return result;
    }
    if (strstr(line, "extendRoute") == line) {//line begins with extendRoute
        int result = executeExtendRoute(line);
        free(line);
        return result;
    }
    if (strstr(line, "removeRoad") == line) {//line begins with removeRoad
        int result = executeRemoveRoad(line);
        free(line);
        return result;
    }
    if (strstr(line, "removeRoute") == line) {//line begins with removeRoute
        int result = executeRemoveRoute(line);
        free(line);
        return result;
    }
    if (isdigit(line[0])) {
        int result = executeAddRoute(line);
        free(line);
        return result;
    }
    free(line);
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
            if (getAndExecuteCommand(currentLine) == EXECUTION_FAILED) {
                printError(lineNumber);
            }
        }
        lineNumber++;
    }
    return 0;
}