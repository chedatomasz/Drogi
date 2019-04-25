//
// Created by chedat on 25.04.19.
//

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "connection_tree.h"
#include "hashmap.h"

#define KEY_LENGTH 32

struct ConnectionTree {
    Connection content;
    ConnectionTree left;
    ConnectionTree right;
    uint32_t key;
};

static int compareNodes(ConnectionTree t1, ConnectionTree t2);
static int compareNodes2(ConnectionTree t1, City t2, uint32_t key);
static ConnectionTree* findInsertion(ConnectionTree* start, ConnectionTree toInsert);

bool addConnection(City city1, City city2, int length, int builtYear){
    Connection connection1 = malloc(sizeof(struct Connection));
    if(!connection1){
        return false;
    }
    Connection connection2 = malloc(sizeof(struct Connection));
    if(!connection2){
        free(connection1);
        return false;
    }
    ConnectionTree tree1 = malloc(sizeof(struct ConnectionTree));
    if(!tree1){
        free(connection1);
        free(connection2);
        return false;
    }
    ConnectionTree tree2 = malloc(sizeof(struct ConnectionTree));
    if(!tree2){
        free(connection1);
        free(connection2);
        free(tree1);
        return false;
    }
    connection1->city2 = city2;
    connection2->city2 = city1;
    connection1->length = length;
    connection2->length = length;
    connection1->year = builtYear;
    connection2->year = builtYear;
    tree1->content = connection1;
    tree2->content = connection2;
    tree1->left = NULL;
    tree1->right = NULL;
    tree2->left = NULL;
    tree2->right = NULL;
    tree1->key = hash(city2->name, KEY_LENGTH);
    tree2->key = hash(city1->name, KEY_LENGTH);
    ConnectionTree* insertion1 = findInsertion(&(city1->root), tree1);
    ConnectionTree* insertion2 = findInsertion(&(city2->root), tree2);
    *insertion1 = tree1;
    *insertion2 = tree2;
    return true;
}

Connection getConnection(City city1, City city2){
    ConnectionTree start = city1->root;
    while(start!= NULL){
        int comparation = compareNodes2(start, city2, hash(city2->name, KEY_LENGTH));
        if(comparation<0){
            start = start->left;
        }
        else if(comparation > 0){
            start = start->right;
        }
        else{
            return start->content;
        }
    }
    return NULL;
}

void freeConnectionTree(ConnectionTree node){
    if(!node){
        return;
    }
    free(node->content);
    freeConnectionTree(node->left);
    freeConnectionTree(node->right);
    free(node);
}

static ConnectionTree* findInsertion(ConnectionTree* start, ConnectionTree toInsert){
    while(*start != NULL){
        if(compareNodes(*start, toInsert) <0){
            start = &((*start)->left);
        }
        else{
            start = &((*start)->right);
        }
    }
    return start;
}

static int compareNodes(ConnectionTree t1, ConnectionTree t2){
    if(t1->key < t2->key){
        return -1;
    }
    if(t1->key > t2->key){
        return 1;
    }
    return strcmp(t2->content->city2->name, t1->content->city2->name);
}

static int compareNodes2(ConnectionTree t1, City t2, uint32_t key){
    if(t1->key < key){
        return -1;
    }
    if(t1->key > key){
        return 1;
    }
    return strcmp(t2->name, t1->content->city2->name);
}