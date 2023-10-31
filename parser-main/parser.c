#include "tokenizer.h"
#include "value.h"
#include "linkedlist.h"
#include "talloc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#ifndef _PARSER
#define _PARSER

// peek
// params: 
// returns:
Value *peek(Value *tree) {
    // if the stack is empty, throw error
    if (tree->type == NULL_TYPE) {
        printf("Syntax error: too many close parenthesis\n");
        texit(0);
    }
    Value *topToken = car(tree);
    return topToken;
}

// push
// params: 
// returns:
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
            // car and cdr should already have been allocated in this situation
            newToken->c.car = car(token);
            newToken->c.cdr = cdr(token);
            break;
    }

    tree = cons(newToken, tree);

    return tree;
}

// addToParseTree
// params: 
// returns:
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

// parse
// params: 
// returns:
// Takes a list of tokens from a Scheme program, and returns a pointer to a
// parse tree representing that program.
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
        // syntaxError();
    }

    return tree;
}

// printTree
// params: 
// returns:
// Prints the tree to the screen in a readable fashion. It should look just like
// Scheme code; use parentheses to indicate subtrees.
void printTreeHelper(Value *tree, int count) {
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
            if (car(tree)->type == CONS_TYPE) {
                printf("(");
                count++;
                // printf("[%i]", count);
            }
            printTreeHelper(car(tree), count);
            printTreeHelper(cdr(tree), count);
            break;
        case NULL_TYPE:
            count--;
            if (count != 0) {
                printf(") ");
            }
            break;
    }
}

void printTree(Value *tree) {
    printTreeHelper(tree, 0);
}

// int main() {
    // Value *tokenList = makeNull();
    
    // Value *tempToken1 = talloc(sizeof(Value));
    // tempToken1->type = OPEN_TYPE;
    // tempToken1->s = "(";
    // Value *tempToken2 = talloc(sizeof(Value));
    // tempToken2->type = SYMBOL_TYPE;
    // tempToken2->s = "define";
    // Value *tempToken3 = talloc(sizeof(Value));
    // tempToken3->type = SYMBOL_TYPE;
    // tempToken3->s = "x";
    // Value *tempToken4 = talloc(sizeof(Value));
    // tempToken4->type = OPEN_TYPE;
    // tempToken4->s = "(";
    // Value *tempToken5 = talloc(sizeof(Value));
    // tempToken5->type = SYMBOL_TYPE;
    // tempToken5->s = "quote";
    // Value *tempToken6 = talloc(sizeof(Value));
    // tempToken6->type = SYMBOL_TYPE;
    // tempToken6->s = "a";
    // Value *tempToken7 = talloc(sizeof(Value));
    // tempToken7->type = CLOSE_TYPE;
    // tempToken7->s = ")";
    // Value *tempToken8 = talloc(sizeof(Value));
    // tempToken8->type = CLOSE_TYPE;
    // tempToken8->s = ")";

    // tokenList = push(tokenList, tempToken1);
    // tokenList = push(tokenList, tempToken2);
    // tokenList = push(tokenList, tempToken3);
    // tokenList = push(tokenList, tempToken4);
    // tokenList = push(tokenList, tempToken5);
    // tokenList = push(tokenList, tempToken6);
    // tokenList = push(tokenList, tempToken7);
    // tokenList = push(tokenList, tempToken8);

    // Value *list = tokenize();
    // list = parse(list);
    // printTree(list);

    // printf("popped value = %s\n", peek(tokenList)->s); 
    // printf("popped value = %s\n", peek(tokenList->c.cdr)->s); 
    // printf("popped value = %i\n", peek(tokenList->c.cdr->c.cdr)->i); 
    
//     texit(0);
// }


#endif
