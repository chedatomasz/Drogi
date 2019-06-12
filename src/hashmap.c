/** @file
 * Implementacja klasy przechowującej hashmapę City
 *
 * @author Tomasz Cheda <tomasz.cheda@students.mimuw.edu.pl>
 * @date 29.04.2019
 */
#include "hashmap.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * @brief Deklaracja typu przechowującego komórkę hashmapy - listę.
 */
typedef struct Bucket *Bucket;

/**
 * @brief Struktura przechowująca komórkę hashmapy - listę.
 */
struct Bucket {
    void *content;///< Zawartość komórki
    char *key; ///< Klucz komórki kompatybilny z zadeklarowanym rozmiarem klucza
    struct Bucket *next;///< Wskaźnik na następny element listy
};

/**
 * @brief Struktura przechowująca dowolny void* i jego klucz - String.
   Ilość komórek jest potęgą 2 mniejszą od 2^32.
 */
struct Hashmap {
    unsigned int keySizeInBits; ///< Rozmiar klucza w bitach.
    Bucket *buckets; ///< Tablica komórek.
};


Hashmap newHashmap(unsigned int sizeInBits) {
    Hashmap result = malloc(sizeof(struct Hashmap));
    if (!result) {
        return NULL;
    }
    result->keySizeInBits = sizeInBits;
    result->buckets = malloc(sizeof(Bucket) * (1u << sizeInBits));
    if (!result->buckets) {
        free(result);
        return NULL;
    }
    for (unsigned int i = 0; i < (1u << sizeInBits); i++) {
        result->buckets[i] = NULL;
    }
    return result;
}

void deleteCitiesHashmap(Hashmap hashmap) {
    //keys are taken to be static char*
    if (!hashmap) {
        return;
    }
    for (unsigned int i = 0; i < (1u << hashmap->keySizeInBits); i++) {
        while (hashmap->buckets[i] != NULL) {
            Bucket backup = hashmap->buckets[i]->next;
            freeCity(hashmap->buckets[i]->content);
            free(hashmap->buckets[i]);
            hashmap->buckets[i] = backup;
        }
    }
    free(hashmap->buckets);
    free(hashmap);
}

//hashing function concept and magic numbers derived from https://en.wikipedia.org/wiki/Jenkins_hash_function

uint32_t hash(const char *key, unsigned int bits) {
    size_t keyLength = strlen(key);
    uint32_t hash = 0;
    uint32_t mask = 1u << (bits - 1);
    mask--;
    mask += 1u << (bits - 1);

    for (size_t i = 0; i < keyLength; i++) {
        hash += (uint32_t) key[i];
        hash += (hash << 10u);
        hash ^= (hash >> 6u);
    }
    hash += (hash << 3u);
    hash ^= (hash >> 11u);
    hash += (hash << 15u);
    return (hash & mask);
}

City getCity(Hashmap hashmap, const char *city1) {
    if (!hashmap) {
        return NULL;
    }
    Bucket bucket = hashmap->buckets[hash(city1, hashmap->keySizeInBits)];
    while (bucket != NULL) {
        if (strcmp(city1, bucket->key) == 0) {
            return bucket->content;
        }
        bucket = bucket->next;
    }
    return NULL;
}

bool removeCity(Hashmap hashmap, const char *city1) {
    if (!hashmap) {
        return false;
    }
    uint32_t address = hash(city1, hashmap->keySizeInBits);
    //If stored in root:
    if (strcmp(hashmap->buckets[address]->key, city1) == 0) {
        Bucket next = hashmap->buckets[address]->next;
        freeCity(hashmap->buckets[address]->content);
        free(hashmap->buckets[address]);
        hashmap->buckets[address] = next;
        return true;
    }
    //If not stored in root:
    Bucket previous = hashmap->buckets[address];
    Bucket current = previous->next;
    while (current != NULL) {
        if (strcmp(city1, current->key) == 0) {
            previous->next = current->next;
            freeCity(current->content);
            free(current);
            return true;
        }
        previous = current;
        current = current->next;
    }
    return false;
}

bool addCity(Hashmap hashmap, City city1) {
    if (!hashmap || !city1) {
        return NULL;
    }
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