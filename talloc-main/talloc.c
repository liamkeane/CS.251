#include <stdlib.h>
#include <stdio.h>
#include "value.h"
#include <assert.h>

#ifndef _TALLOC
#define _TALLOC

// define global list to hold pointers to allocated memory
Value *memoryAddresses = NULL;

int tallocMemoryCount() {
    // loop through the global linked list adding the cost of the node to the cost of the malloc'd item
    int total = 0;
    current = memoryAddresses;

    while (current->type != NULL_TYPE) {
        // 
    }
    // add final NULL allocation
    total += sizeof(Value);

    return total;
}

// talloc
// params: size - the number of bytes requested to allocate
// returns: a pointer to the allocated block
// talloc operates similary to malloc, but adds a pointer to the newly allocated memory to a global linked list
// if the global linked list is NULL, it is initialized as a NULL_TYPE
void *talloc(size_t size) {
    // if our list has not yet been initialized
    if (memoryAddresses == NULL) {
        memoryAddresses = malloc(sizeof(Value));
        memoryAddresses -> type = NULL_TYPE;
    }

    // allocate space for pointer struct
    Value *newItem = malloc(sizeof(Value));
    newItem -> type = PTR_TYPE;
    newItem -> p = malloc(size);

    // allocate space for cons cell containing pointer struct
    Value *consCell = malloc(sizeof(Value));
    consCell -> type = CONS_TYPE;
    consCell -> c.car = newItem;
    consCell -> c.cdr = memoryAddresses;
    memoryAddresses = consCell;

    return newItem -> p;
}

// tfreeHelper
// params: memoryAddress - a pointer to a Value
// returns: Nothing
// a helper function to tfree()
void tfreeHelper(Value *memoryAddress) {
    switch (memoryAddress -> type) {
            case INT_TYPE:
                break;
            case DOUBLE_TYPE:
                break;
            case STR_TYPE:
                break;
            case PTR_TYPE:
                free(memoryAddress -> p);
                free(memoryAddress);
                break;
            case CONS_TYPE:
                tfreeHelper(memoryAddress -> c.car);
                tfreeHelper(memoryAddress -> c.cdr);
                free(memoryAddress);
                break;
            case NULL_TYPE:
                free(memoryAddress);
                break;
        }
}

// tfree
// params: None
// returns: Nothing
// calls tfreeHelper() to recursively free all pointers to blocks of memory allocated by talloc
// resets the global linked list to NULL
void tfree() {
    tfreeHelper(memoryAddresses);
    memoryAddresses = NULL;
}

// texit
// params: status
// returns: Nothing
// calls tfree before calling exit
void texit(int status) {
    tfree();
    exit(status);
}

int main() {
    Value *list = talloc(sizeof(Value));
    printf("Size of list: %lu\n", sizeof(list));
}

#endif

