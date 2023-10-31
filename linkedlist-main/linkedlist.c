#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "value.h"
#include <string.h>
#include <assert.h>

#ifndef _LINKEDLIST
#define _LINKEDLIST

// Looked up documentation for assert() at the following: 
// Functions appear in the order before they are called to prevent implicit definitions(?) from being created.

// makeNull
// params: None
// returns: A new Value with type NULL_TYPE
Value *makeNull() {
    Value *nullValue = malloc(sizeof(Value));
    nullValue -> type = NULL_TYPE;
    return nullValue;
}

// cons
// params: newCar - a pointer to a Value; newCdr - a pointer to a Value
// returns: a new Value with type CONS_TYPE.
// Sets the car and cdr of the value to point to the given parameters.
Value *cons(Value *newCar, Value *newCdr) {
    Value *consCell = malloc(sizeof(Value));
    consCell -> type = CONS_TYPE;
    consCell -> c.car = newCar;
    consCell -> c.cdr = newCdr;
    return consCell;
}

// displayHelper
// params: list - a pointer to a Value; index - an integer
// returns: Nothing
// Helper method called by display() to print the items in list.
void displayHelper(Value *list, int index) {
    Value *currentList = list;
    switch (currentList -> type) {
        case INT_TYPE:
            printf("Item at index %i: %i\n", index, currentList -> i);
            break;
        case DOUBLE_TYPE:
            printf("Item at index %i: %lf\n", index, currentList -> d);
            break;
        case STR_TYPE:
            printf("Item at index %i: %s\n", index, currentList -> s);
            break;
        case CONS_TYPE:
            displayHelper(currentList -> c.car, index);
            displayHelper(currentList -> c.cdr, index + 1);
            break;
        case NULL_TYPE:
            printf("Item at index %i: null\n", index);
            break;
    }
}

// display
// params: list - a pointer to a Value
// returns: Nothing
// Prints out the values contained in list in a readable format
void display(Value *list) {
    displayHelper(list, 0);
}

// car
// params: list - a pointer to a Value
// returns: a pointer to a Value
// car returns the car of the given Value. Throws an error if list is not of type CONS_TYPE.
Value *car(Value *list) {
    assert(list -> type == CONS_TYPE);
    return list -> c.car;
}

// cdr
// params: list - a pointer to a Value
// returns: a pointer to a Value
// cdr returns the cdr of the given Value. Throws an error if list is not of type CONS_TYPE.
Value *cdr(Value *list) {
    assert(list -> type == CONS_TYPE);
    return list -> c.cdr;
}

// cleanup
// params: list - a pointer to a Value
// returns: Nothing
// Recursively frees all memory directly or indirectly referred to by list.
void cleanup(Value *list) {
    switch (list -> type) {
        case INT_TYPE:
            free(list);
            break;
        case DOUBLE_TYPE:
            free(list);
            break;
        case STR_TYPE:
            free(list -> s);
            free (list);
            break;
        case CONS_TYPE:
            cleanup(car(list));
            cleanup(cdr(list));
            free(list);
            break;
        case NULL_TYPE:
            free(list);
            break;
    }
}

// reverseHelper
// params: list - a pointer to a Value; reversed - a pointer to a Value
// returns: a pointer to a Value
// helper method called by reverse() to create a reversed version of list.
Value *reverseHelper(Value *list, Value *reversed) {
    Value *newItem = malloc(sizeof(Value));
    switch (list -> type) {
        case NULL_TYPE:
            free(newItem);
            return reversed;
        case INT_TYPE:
            newItem -> type = INT_TYPE;
            newItem -> i = list -> i;
            return newItem;
        case DOUBLE_TYPE:
            newItem -> type = DOUBLE_TYPE;
            newItem -> d = list -> d;
            return newItem;
        case STR_TYPE:
            newItem -> type = STR_TYPE;
            newItem -> s = malloc(sizeof(char) * strlen(list -> s) + 1);
            strcpy(newItem -> s, list -> s);
            return newItem;
        case CONS_TYPE:
            free(newItem);
            reversed = cons(reverseHelper(list -> c.car, reversed), reversed);          //
            if (cdr(list) -> type == NULL_TYPE || cdr(list) -> type == CONS_TYPE) {     //
                return reverseHelper(list -> c.cdr, reversed);
            } else {
                return cons(reverseHelper(list -> c.cdr, reversed), reversed);
            }
    }
}

// reverse
// params: list - a pointer to a Value
// returns: a pointer to a Value
// reverse uses reverseHelper() to recursively run through the items in the linked list pointed to by list.
// Creates and returns a pointer to a new linked list containing the items in list, in reverse order.2.0s
Value *reverse(Value *list) {
    Value *reversed = makeNull();
    return reverseHelper(list, reversed);
}
    

// isNull
// params: value - a poier to a Value
// returns: true or false
// Returns true if the type of the give Value is of NULL_TYPE, and false otherwise.
bool isNull(Value *value) {
    if (value -> type == NULL_TYPE) {
        return true;
    } else {
        return false;
    }
}

// length
// params: value - a pointer to a value
// returns: an integer
// length iteratively finds the number of items in the linked list pointed to by value.
int length(Value *value) {
    if (value -> type == NULL_TYPE) {
        return 0;
    } else if (value -> type == CONS_TYPE) {
        int count = 1;
        Value *current = value;
        Value *next = cdr(value);

        while (next -> type != NULL_TYPE) {
            current = next;
            next = cdr(next);
            count++;
        }

        return count;
    } else {
        return 1;
    }
}

#endif