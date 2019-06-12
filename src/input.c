//
// Created by chedat on 16.05.19.
//

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include "input.h"

#define MAIN_FIRST_ALLOC_SIZE 4

#define STATE_ENTRY 0
#define STATE_CITY 1
#define STATE_LENGTH 2
#define STATE_YEAR 3

char *readInNextLine() {
    size_t allocatedSize = MAIN_FIRST_ALLOC_SIZE;
    size_t realSize = 0;
    char *line = malloc(sizeof(char) * MAIN_FIRST_ALLOC_SIZE);
    int buffer;
    if (!line) {
        exit(0);
    }
    bool stop = false;
    while (!stop) {
        buffer = getc(stdin);
        if (buffer == EOF) {
            free(line);
            return NULL;
        }
        if (buffer == 0) {
            free(line);
            readToLineEnd();
            return NULL;
        }
        if (buffer == '\n') {
            stop = true;
            continue;
        }
        line[realSize] = (char) buffer;
        realSize++;
        //If we run out of space, we increase it x2.
        if (realSize == allocatedSize) {
            allocatedSize *= 2;
            char *placeholder = realloc(line, sizeof(char) * allocatedSize);
            if (!placeholder) {
                free(line);
                exit(0);
            }
            line = placeholder;
        }
    }
    line[realSize] = '\0';
    return line;
}

void readToLineEnd() {
    int buffer;
    while ((buffer = getc(stdin)) != EOF) {
        if (buffer == '\n') {
            return;
        }
    }
}

int peek() {
    int c = getc(stdin);
    if (c != EOF) {
        ungetc(c, stdin);
    }
    return c;
}

char *tokenize(char **line, char delimiter) {
    if (!line || !(*line) || *(*line) == '\0') {
        return NULL;
    }
    char *result = *line;
    while (*(*line) != '\0' && *(*line) != delimiter) {
        (*line)++;
    }
    if (*(*line) != '\0') {
        *(*line) = '\0';
        (*line)++;
    }
    return result;
}

unsigned getUnsignedFromString(char *token) {
    char *end = NULL;
    errno = 0;
    unsigned long lengthLong = strtoul(token, &end, 10);
    if (errno || *end != '\0' || lengthLong > UINT_MAX) {
        errno = 1;
        return 0;
    }
    return (unsigned) lengthLong;
}

int getIntFromString(char *token) {
    char *end = NULL;
    errno = 0;
    long YearLong = strtol(token, &end, 10);
    if (errno || *end != '\0' || YearLong > INT_MAX || YearLong < INT_MIN) {
        errno = 1;
        return 0;
    }
    return (int) YearLong;
}

bool checkAddRouteSyntax(const char *line) {
    int state = STATE_ENTRY;
    int position = 0;
    int fresh = 0;
    bool canEndNowIfCity = false;
    while (line[position] != '\0') {
        switch (state) {
            case STATE_ENTRY://routenum
                if (isdigit(line[position])) {
                    position++;
                    fresh = 1;
                } else if (line[position] == ';' && fresh != 0) {
                    position++;
                    state = STATE_CITY;
                    fresh = 0;
                } else {
                    return false;
                }
                break;
            case STATE_CITY:
                if (line[position] >= 0 && line[position] <= 31) {
                    return false;
                } else if (line[position] != ';') {
                    fresh = 1;
                    position++;
                } else if (fresh == 0) {
                    return false;
                } else {
                    state = STATE_LENGTH;
                    fresh = 0;
                    canEndNowIfCity = true;
                    position++;
                }
                break;
            case STATE_LENGTH:
                if (isdigit(line[position])) {
                    position++;
                    fresh = 1;
                } else if (line[position] == ';' && fresh != 0) {
                    state = STATE_YEAR;
                    fresh = 0;
                    position++;
                } else {
                    return false;
                }
                break;
            case STATE_YEAR:
                if (fresh == 0 && line[position] == '-') {
                    position++;
                } else if (isdigit(line[position])) {
                    position++;
                    fresh = 1;
                } else if (line[position] == ';' && fresh != 0) {
                    state = STATE_CITY;
                    fresh = 0;
                    position++;
                } else {
                    return false;
                }
                break;
        }

    }
    return (canEndNowIfCity && fresh != 0 && state == STATE_CITY);
}

size_t getCityNum(const char *line) {
    size_t i = 0;
    size_t num = 0;
    while (line[i] != '\0') {
        if (line[i] == ';') {
            num++;
        }
        i++;
    }
    return num / 3;
}