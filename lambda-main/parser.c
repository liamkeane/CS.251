#include "tokenizer.h"
#include "value.h"
#include "linkedlist.h"
#include "talloc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#ifndef _PARSER
#define _PARSER

/*
syntaxError
params: depth - an integer
returns: nothing
If depth is below or above zero, print the respective error and exit the program.
*/
void syntaxError(int depth) {
    if (depth < 0) {
        printf("Syntax Error: too many close parenthesis\n");
        texit(0);
    } else {
        printf("Syntax Error: too few close parenthesis\n");
        texit(0);
    }
}

/*
push
params: tree - a Value struct, token - a Value struct
returns: tree - a Value struct
Returns a list with the given token pushed onto the top of the given tree.
*/
Value *push(Value *tree, Value *token) {

    Value *newToken = talloc(sizeof(Value));

    switch (token->type) {
        case NULL_TYPE:
            newToken->type = NULL_TYPE;
            break;
        case INT_TYPE:
            newToken->type = INT_TYPE;
            newToken->i = token->i;
            break;
        case DOUBLE_TYPE:
            newToken->type = DOUBLE_TYPE;
            newToken->d = token->d;
            break;
        case STR_TYPE:
            newToken->type = STR_TYPE;
            char *newString = talloc(sizeof(char)*strlen(token->s)+1);
            strcpy(newString, token->s);
            newToken->s = newString;
            break;
        case PTR_TYPE:
            break;
        case OPEN_TYPE:
            newToken->type = OPEN_TYPE;
            char *newOpen = talloc(2*sizeof(char));
            strcpy(newOpen, "(");
            newToken->s = newOpen;
            break;
        case CLOSE_TYPE:
            newToken->type = CLOSE_TYPE;
            char *newClose = talloc(2*sizeof(char));
            strcpy(newClose, ")");
            newToken -> s = newClose;
            break;
        case BOOL_TYPE:
            newToken->type = BOOL_TYPE;
            newToken->i = token->i;
            break;
        case SYMBOL_TYPE:
            newToken->type = SYMBOL_TYPE;
            char *newSymbol = talloc(sizeof(char)*strlen(token->s)+1);
            strcpy(newSymbol, token->s);
            newToken->s = newSymbol;
            break;
        case CONS_TYPE:
            newToken->type = CONS_TYPE;
            return cons(token, tree);
        default:
            break;
    }

    tree = cons(newToken, tree);

    return tree;
}

/* 
addToParseTree
params: tree - a Value struct, depth - a pointer to an integer, token - a Value struct
returns: tree - a Value struct
Pushes the given token onto the tree, if the token is a close parenthesis pop tokens off 
the tree into a new list until it reaches an open parenthesis and push that new list back onto the tree.
*/
Value *addToParseTree(Value *tree, int *depth, Value *token) {
    // is the token not a close paren?
        // push on the stack
    if (token->type != CLOSE_TYPE) {

        if (token->type == OPEN_TYPE) {
            *depth += 1;
        }
        tree = push(tree, token);

    // else,
        // pop items from the stack into a list until an open paren is popped
        // push that list onto the stack
    } else {
        *depth -= 1;
        if (*depth < 0) {
            syntaxError(*depth);
        }
        Value *subTree = makeNull();
        Value *current = tree;

        while (car(current)->type != OPEN_TYPE) {
            subTree = cons(car(current), subTree);
            current = current->c.cdr;
            tree = tree->c.cdr;
        }
        
        tree = tree->c.cdr;
        tree = push(tree, subTree);

    }

    return tree;    
}

/*
parse
params: tokens - a Value struct
returns: tree - a Value struct
Takes a list of tokens from a Scheme program, and returns a pointer to a parse tree representing that program.
*/
Value *parse(Value *tokens) {

    Value *tree = makeNull();
    int depth = 0;

    Value *current = tokens;
    assert(current != NULL && "Error (parse): null pointer");

    while (current->type != NULL_TYPE) {
        Value *token = car(current);
        tree = addToParseTree(tree, &depth, token);
        current = cdr(current);
    }

    if (depth != 0) {
        syntaxError(depth);
    }

    //reverse the list of parse trees
    Value *prev = makeNull();
    current = tree;
    Value *next = cdr(tree);
    while (next->type != NULL_TYPE) {
        current->c.cdr = prev;
        prev = current;
        current = next;
        next = cdr(next);
    }
    current->c.cdr = prev;
    tree = current;

    return tree;
}

/*
printTree
params: tree - a Value struct, needsClose - a pointer to an integer
returns: nothing
A helper function for printTree() meant to recursively print the given tree.
*/
void printTreeHelper(Value *tree, int *needsClose) {
    switch (tree -> type) {
        case INT_TYPE:
            printf("%i ", tree->i);
            break;
        case DOUBLE_TYPE:
            printf("%.2lf ", tree->d);
            break;
        case STR_TYPE:
            printf("%s ", tree->s);
            break;
        case PTR_TYPE:
            break;
        case OPEN_TYPE:
            break;
        case CLOSE_TYPE:
            break;
        case BOOL_TYPE:
            if (tree->i == 1) {
                printf("#t ");
            } else {
                printf("#f ");
            }
            break;
        case SYMBOL_TYPE:
            printf("%s ", tree->s);
            break;
        case CONS_TYPE:
            if (car(tree)->type == CONS_TYPE || car(tree)->type == NULL_TYPE) {
                printf("(");
                *needsClose += 1;
            }
            printTreeHelper(car(tree), needsClose);
            printTreeHelper(cdr(tree), needsClose);
            break;
        case NULL_TYPE:
            if (*needsClose > 0) {
                *needsClose -= 1;
                printf(") ");
            }
            break;
        default:
            break;
    }
}

/* 
printTree 
params: tree - a Value struct
returns: nothing
Outputs the given list of parse trees as they would appear in a Scheme program.
*/
void printTree(Value *tree) {
    int needsClose = 0;
    printTreeHelper(tree, &needsClose);
}

#endif
