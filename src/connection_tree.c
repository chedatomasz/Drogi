/** @file
 * Implementacja klasy oferującej hashowane drzewo sąsiedztwa
 *
 * @author Tomasz Cheda <tomasz.cheda@students.mimuw.edu.pl>
 * @date 29.04.2019
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "connection_tree.h"
#include "hashmap.h"

#define KEY_LENGTH 32 ///< Długość klucza używanego do hashowania

/**
 * Implementacja struktury przechowującej informacje o pojedynczym drzewie sąsiedztwa.
 */
struct ConnectionTree {
    Connection content; ///< Wskaźnik na strukturę z informacją o połączeniu
    ConnectionTree left; ///< Wskaźnik na lewego syna
    ConnectionTree right; ///<Wskaźnik na prawego syna
    uint32_t key; ///<Klucz węzła.
};

static int compareNodes(ConnectionTree t1, ConnectionTree t2);
static int compareNodes2(ConnectionTree t1, City t2, uint32_t key);
static ConnectionTree* findInsertion(ConnectionTree* start, ConnectionTree toInsert);

bool addConnection(City city1, City city2, unsigned length, int builtYear){
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
    city1->numOfConnections++;
    city2->numOfConnections++;
    return true;
}

Connection getConnection(City city1, City city2){
    ConnectionTree start = city1->root;
    while(start!= NULL){
        int comparison = compareNodes2(start, city2, hash(city2->name, KEY_LENGTH));
        if(comparison<0){
            start = start->left;
        }
        else if(comparison > 0){
            start = start->right;
        }
        else{
            return start->content;
        }
    }
    return NULL;
}

/**
 * Dodaje rekurencyjnie do tablicy kolejne elementy danego drzewa połączeń
 * @param city1 Węzeł, od którego rozpoczynami dodawanie, może być NULL
 * @param allConnections Tablica do któregj wpisujemy połączenia
 * @param counter licznik wpisanych połączeń i pozycja kolejnych wpisań
 */
static void recursiveGetConnections(ConnectionTree city1, Connection* allConnections, int* counter){
    if(!city1){
        return;
    }
    allConnections[*counter]=city1->content;
    (*counter)++;
    recursiveGetConnections(city1->left, allConnections, counter);
    recursiveGetConnections(city1->right, allConnections, counter);
}

Connection* getAllConnections(City city1){
    Connection* allConnections = malloc(sizeof(Connection)*(size_t)(city1->numOfConnections));
    if(!allConnections){
        return NULL;
    }
    int counter = 0;
    recursiveGetConnections(city1->root, allConnections, &counter);
    return allConnections;
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

/**
 * @brief Znajduje miejsce w drzewie, gdzie należy wstawić dany węzeł. Zakłada brak powtórzeń.
 * @param start adres węzła początkowego
 * @param toInsert węzeł do wstawienia
 * @return adres miejsca, gdzie należy wstawić węzeł
 */
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

/**
 * @brief Porównuje dwa węzły ConnectionTree początowo po kluczu, a w przypadku kolizji po nazwie miasta docelowego
 * @param t1 pierwszy węzeł
 * @param t2 drugi węzeł
 * @return -1 jeśli t1<t2, 1 jeśli t1>t2, 0 jeśli są równe
 */
static int compareNodes(ConnectionTree t1, ConnectionTree t2){
    if(t1->key < t2->key){
        return -1;
    }
    if(t1->key > t2->key){
        return 1;
    }
    return strcmp(t2->content->city2->name, t1->content->city2->name);
}

/**
 * @brief Porównuje węzły jak compareNodes, jednak przyjmuje zamiast drugiego węzła jego składniki
 * @param t1 pierwszy węzeł
 * @param t2 docelowe miasto "drugiego węzła"
 * @param key klucz "drugiego węzła"
 * @return -1 jeśli t1<t2, 1 jeśli t1>t2, 0 jeśli są równe
 */
static int compareNodes2(ConnectionTree t1, City t2, uint32_t key){
    if(t1->key < key){
        return -1;
    }
    if(t1->key > key){
        return 1;
    }
    return strcmp(t2->name, t1->content->city2->name);
}

/**
 * @brief Usuwa węzeł wskazywany przez target z drzewa zachowujac własności BST
 * @param target Węzeł do usunięcia
 */
void removeNode(ConnectionTree* target){
    if(!((*target)->left) && !((*target)->right)){
        free((*target)->content);
        free(*target);
        *target=NULL;
        return;
    }
    if(!((*target)->left)){
        ConnectionTree temp = (*target)->right;
        free((*target)->content);
        free(*target);
        *target=temp;
        return;
    }
    if(!(*target)->right){
        ConnectionTree temp = (*target)->left;
        free((*target)->content);
        free(*target);
        *target=temp;
        return;
    }
    ConnectionTree* minRightSubTree = &((*target)->right);
    while((*minRightSubTree)->left){
        minRightSubTree = &((*minRightSubTree) ->left);
    }
    free((*target)->content);
    (*target)->content=(*minRightSubTree)->content;
    (*target)->key=(*minRightSubTree)->key;
    (*minRightSubTree)->content = NULL;
    removeNode(minRightSubTree);
}
void removeConnection(City city1, City city2){
    ConnectionTree *start = &(city1->root);
    city1->numOfConnections--;
    while(*start!= NULL){
        int comparison = compareNodes2(*start, city2, hash(city2->name, KEY_LENGTH));
        if(comparison<0){
           start = &((*start)->left);
        }
        else if(comparison > 0){
            start = &((*start)->right);
        }
        else{
            removeNode(start);
            return;
        }
    }
}