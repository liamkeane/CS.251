#include "tokenizer.h"
#include "value.h"
#include "linkedlist.h"
#include "talloc.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#ifndef _INTERPRETER
#define _INTERPRETER

// define eval
Value *eval(Value *, Frame *);

/*
makeFrame
params: parent - a pointer to a Frame struct
returns: newFrame - a pointer to a Frame struct
Given a parent frame, allocates a null Frame and sets its parent to point to the parameter.
*/
Frame *makeFrame(Frame *parent) {
   Frame *newFrame = talloc(sizeof(Frame));
   newFrame -> parent = parent;
   newFrame -> bindings = makeNull();
   return newFrame;
}

/*
addBinding
params: binding - a pointer to a Value representing a binding in dotted pair format; frame - a pointer to a Frame
returns: nothing
addBinding() adds the given binding to the given frame's list of bindings.
*/
void addBinding(Value *binding, Frame *frame) {
    Value *current = frame -> bindings;
    // check for multiple bindings for a variable (not allowed)
    while (current -> type != NULL_TYPE) {
        if (!strcmp(car(car(current)) -> s, car(binding) -> s)) {
            printf("Evaluation error: local variable %s already bound\n", car(binding) -> s);
            texit(0);
        }
        current = cdr(current);
    }

    // check to make sure variable to be bound is of symbol type
    if (car(binding) -> type != SYMBOL_TYPE) {
        printf("Evaluation error: variable being bound must be of symbol type\n");
        texit(0);

    } else {
        frame -> bindings = cons(binding, frame -> bindings);
    }
}

/*
evalIf
params: tree - a pointer to a Value struct, frame - a pointer to a Frame struct
returns: a pointer to a Value struct
Function is called in the case that an 'if' symbol is evaluated. Returns the second arg if the first arg evaluates to true and the third arg if false.
*/
Value *evalIf(Value *args, Frame *frame) {
    // if more or less than 3 args provided, throw an error.
    if (cdr(args) -> type == NULL_TYPE || cdr(cdr(args)) -> type == NULL_TYPE || cdr(cdr(cdr(args))) -> type != NULL_TYPE) {
        printf("Evaluation error: incorrect number of args for if statement\n");
        texit(0);
    }

    Value *boolResult = eval(car(args), frame);
    // if the first arg does not evaluate to a boolean, throw an error.
    if (boolResult -> type != BOOL_TYPE) {
        printf("Evaluation error: if statement predicate does not resolve to boolean\n");
        texit(0);

    } else if (boolResult -> i == 1) {
        return eval(car(cdr(args)), frame);

    } else {
        return eval(car(cdr(cdr(args))), frame);
    }

    return makeNull();
}

/*
evalLet
params: args - a pointer to a Value representing the arguments of the let statement; frame - a pointer to a Frame
returns: a pointer to a Value, that is the result of evaluating the body of the let statement within the proper Frame
evalLet() creates a new Frame containing the let statement's bindings, then evaluates the let statement's body in the context of that Frame.
*/
Value *evalLet(Value *args, Frame *frame) {
    // if no arguments or body are provided for let, throw an error.
    if (args -> type == NULL_TYPE || cdr(args) -> type == NULL_TYPE) {
        printf("Evaluation error: incorrect number of args for let\n");
        texit(0);

    }
    // create new frame in which to evaluate let
    Frame *newFrame = makeFrame(frame);
    
    // checks list of bindings to make sure it is a proper list; throws error if not
    if (car(args) -> type != CONS_TYPE && car(args) -> type != NULL_TYPE) {
        printf("Evaluation error: invalid let binding\n");
        texit(0);
    }

    // checks proper nested list formatting for list of bindings; throws error if bindings are incorrectly formatted
    Value *binding = car(args);
    while(binding -> type != NULL_TYPE) {
        // check outer list format
        if (binding -> type != CONS_TYPE) {
            printf("Evaluation error: invalid let binding\n");
            texit(0);

        // check each binding itself
        } else if (car(binding) -> type != CONS_TYPE) {
            printf("Evaluation error: invalid let binding\n");
            texit(0);

        // adds binding to newFrame
        } else {
            addBinding(cons(car(car(binding)), eval(car(cdr(car(binding))), frame)), newFrame);
            binding = cdr(binding);
        }
    }

    // evaluates body of the let statement in the context of newFrame
    Value *result;
    Value *body = cdr(args);
    while (body -> type != NULL_TYPE) {
        result = eval(car(body), newFrame);
        body = cdr(body);
    }
    return result;
}

/*
lookUpSymbol
params: symbol - a pointer to a Value struct, frame - a pointer to a Frame struct
returns: a pointer to a Value struct
Given a frame and a symbol, traverse the frame searching for the symbol's assigned value.
*/
Value *lookUpSymbol(Value *symbol, Frame *frame) {
    Value *currentBinding = frame -> bindings;
    while (currentBinding -> type != NULL_TYPE) {
        if (!strcmp(car(car(currentBinding)) -> s, symbol -> s)) {
            return cdr(car(currentBinding));
        } else {
            currentBinding = cdr(currentBinding);
        }
    }

    // if the symbol has not been defined, throw an error.
    if (frame -> parent == NULL) {
        printf("Evaluation error: binding for symbol '%s' not defined in a frame\n", symbol -> s);
        texit(0);
    }

    return lookUpSymbol(symbol, frame -> parent);
}

/*
eval
params: tree - a pointer to a Value struct, frame - a pointer to a Frame struct
returns: a pointer to a Value struct
Given a pointer to a parse tree and a pointer to a frame, evaluate the parse tree in the context of the current frame.
*/
Value *eval(Value *tree, Frame *frame) {
    switch (tree->type)  {
        case INT_TYPE: {
            return tree;
        }
        case DOUBLE_TYPE: {
            return tree;
        }
        case STR_TYPE: {
            return tree;
        }
        case BOOL_TYPE: {
            return tree;
        }
        case SYMBOL_TYPE: {
            return lookUpSymbol(tree, frame);
        }  
        case CONS_TYPE: {
            Value *first = car(tree);
            Value *args = cdr(tree);

            if (!strcmp(first->s, "if")) {
               return evalIf(args, frame);
               
            } else if (!strcmp(first->s, "let")) {
                return evalLet(args, frame);

            } else if (!strcmp(first->s, "quote")) {
                // if there are none or multiple args given to quote, throw an error.
                if (args -> type != CONS_TYPE || cdr(args) -> type != NULL_TYPE) {
                    printf("Evaluation error: incorrect number of args for quote\n");
                    texit(0);
                } else {
                    return args;
                }

            // if special form is unrecognized, throw an error.
            } else {
               printf("Evaluation error: unrecognized special form\n");
               texit(0);
            }
            break;
        }
        default: {
            break;
        }
    }
    return makeNull();    
}

/*
printingHelper
params: tree - a pointer to a Value struct, needsClose - a pointer to an integer
returns: nothing
Given a tree representing the result of evaluating a parse tree, print the contents of the tree.
*/
void printingHelper(Value *tree, int *needsClose) {
    switch (tree->type) {
        case INT_TYPE: {
            printf("%i ", tree -> i);
            break;
        }
        case DOUBLE_TYPE: {
            printf("%lf ", tree -> d);
            break;
        }
        case STR_TYPE: {
            printf("%s ", tree -> s);
            break;
        }
        case BOOL_TYPE: {
            if (tree -> i == 1) {
                printf("#t");
            } else {
                printf("#f");
            }
            break;
        }
        case SYMBOL_TYPE: {
            printf("%s ", tree -> s);
            break;
        }
        case CONS_TYPE: {
            if (car(tree)->type == CONS_TYPE || car(tree)->type == NULL_TYPE) {
                printf("(");
                *needsClose += 1;
            }
            printingHelper(car(tree), needsClose);
            printingHelper(cdr(tree), needsClose);
            break;
        }
        case NULL_TYPE: {
            if (*needsClose > 0) {
                *needsClose -= 1;
                printf(") ");
            }
            break;
        }
        default:
            break;
    }
}

/*
interpret
params: tree - a pointer to a Value struct
returns: nothing
Given the pointer to a Scheme program, iteratively call eval() on each parse tree and display their respective result(s).
*/
void interpret(Value *tree) {
    Value *current = tree;
    Frame *global = makeFrame(NULL);
    while (current->type != NULL_TYPE) {
        Value *result = eval(car(current), global);
        int needsClose = 0;
        printingHelper(result, &needsClose);
        printf("\n");
        current = cdr(current);
    }
}

#endif