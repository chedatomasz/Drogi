#include "hashmap.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
//
// Created by chedat on 25.04.19.
//
typedef struct Bucket* Bucket;

struct Bucket{
    void* content;
    void* key;
    struct Bucket* next;
};

struct Hashmap{
    unsigned int keySizeInBits;
    Bucket *buckets;
};


Hashmap newHashmap(unsigned int sizeInBits){
    Hashmap result = malloc(sizeof(struct Hashmap));
    if(!result){
        return NULL;
    }
    result->keySizeInBits=sizeInBits;
    result->buckets = malloc(sizeof(Bucket) * (1u<<sizeInBits));
    if(!result->buckets){
        free(result);
        return NULL;
    }
    for(unsigned int i = 0; i < (1u<<sizeInBits); i++){
        result->buckets[i] = NULL;
    }
    return result;
}

void deleteCitiesHashmap(Hashmap hashmap){
    //keys are taken to be static char*
    for(unsigned int i = 0; i < (1u<<hashmap->keySizeInBits); i++){
        while(hashmap->buckets[i] != NULL){
            Bucket backup = hashmap->buckets[i]->next;
            freeCity(hashmap->buckets[i]->content);
            free(hashmap->buckets[i]);
            hashmap->buckets[i] = backup;
        }
    }
    free(hashmap);
}

//hashing function concept and magic numbers derived from https://en.wikipedia.org/wiki/Jenkins_hash_function

uint32_t hash(const char* key, unsigned int bits){
    int keyLength = strlen(key);
    uint32_t  hash = 0;
    uint32_t mask = 1u << (bits-1);
    mask --;
    mask += 1u << (bits-1);

    for(int i=0; i < keyLength; i++){
        hash += key[i];
        hash += (hash<<10u);
        hash ^= (hash >>6u);
    }
    hash += (hash << 3u);
    hash ^= (hash >> 11u);
    hash += (hash << 15u);
    return (hash & mask);
}

City getCity(Hashmap hashmap, const char* city1){
    Bucket bucket = hashmap->buckets[hash(city1, hashmap->keySizeInBits)];
    while(bucket != NULL){
        if(strcmp(city1, bucket->key) == 0){
            return bucket->content;
        }
        bucket = bucket->next;
    }
    return NULL;
}

_Bool addCity (Hashmap hashmap, City city1) {
    uint32_t address = hash(city1->name, hashmap->keySizeInBits);
    Bucket new = malloc(sizeof(struct Bucket));
    if (!new) {
        return false;
    }
    new->content = city1;
    new->key = city1->name;
    new->next = NULL;
    if (hashmap->buckets[address] == NULL) {
        hashmap->buckets[address] = new;
        return true;
    } else {
        Bucket parent = hashmap->buckets[address];
        while (parent->next) {
            parent = parent->next;
        }
        parent->next = new;
        return true;
    }
}