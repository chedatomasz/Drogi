//
// Created by chedat on 16.05.19.
//

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include "input.h"
#define MAIN_FIRST_ALLOC_SIZE 4

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

char* tokenize(char** line, char delimiter){
    if(!line || !(*line) || *(*line)=='\0'){
        return NULL;
    }
    char* result = *line;
    while(*(*line) != '\0' && *(*line) != delimiter){
        (*line)++;
    }
    if(*(*line) != '\0'){
        *(*line)='\0';
        (*line) ++;
    }
    return result;
}

unsigned getUnsignedFromString(char *token){
    char* end = NULL;
    errno = 0;
    unsigned long lengthLong = strtoul(token, &end, 10);
    if(errno || *end != '\0' || lengthLong > UINT_MAX){
        errno=1;
        return 0;
    }
    return (unsigned) lengthLong;
}

int getIntFromString(char *token){
    char* end = NULL;
    errno = 0;
    long YearLong = strtol (token, &end, 10);
    if(errno || *end != '\0' || YearLong > INT_MAX || YearLong < INT_MIN){
        errno=1;
        return 0;
    }
    return (int) YearLong;
}