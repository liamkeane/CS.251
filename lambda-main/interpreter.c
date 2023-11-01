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
evalEach
params: args - a pointer to a Value struct, frame - a pointer to a Frame struct
returns: a Value struct containing the evaluated arguments to be passed into apply()
*/
Value *evalEach(Value *args, Frame *frame) {
    Value *evaledArgs = makeNull();
    Value *arg = args;
    while (arg -> type != NULL_TYPE) {
        evaledArgs = cons(eval(car(arg), frame), evaledArgs);
        arg = cdr(arg);
    }
    return reverse(evaledArgs);
}

/*
makeClosure
params: environment - a pointer to a Frame; parameters - a pointer to a Value representing a linked list of parameters; functionBody - a pointer to a Value representing a function's body as a parse tree
returns: a new Value of type CLOSURE_TYPE containing the information provided in the parameters
*/
Value *makeClosure(Frame *environment, Value *parameters, Value *functionBody) {
    Value *closure = talloc(sizeof(Value));
    closure -> type = CLOSURE_TYPE;
    closure -> cl.paramNames = parameters;
    closure -> cl.functionCode = functionBody;
    closure -> cl.frame = environment;
    return closure;
}

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
apply
params: evaledOperator - a pointer to a Value, that represents a closure corresponding to a function; evaledArgs - a pointer to a value representing a list of previously evaluated function arguments
returns: the result of evaluating the body contained in the given closure, in the context of evaledArgs and the closure's environment
apply() builds a new frame whose parent is the environment specified in the given closure, and adds bindings to it corresponding to each parameter/argument pair.
apply() then evaluates the function body specified in the given closure in the context of the new frame, and returns the result.
*/
Value *apply(Value *evaledOperator, Value *evaledArgs) {
    // if the given operator is not a function, throw an error.
    if (evaledOperator -> type != CLOSURE_TYPE) {
        printf("Evaluation error: non-function being called as function\n");
        texit(0);
    }
    Frame *frame = makeFrame(evaledOperator -> cl.frame);
    Value *param = evaledOperator -> cl.paramNames;
    Value *arg = evaledArgs;
    while (param -> type != NULL_TYPE) {
        // if too few arguments are passed, throw an error.
        if (arg -> type == NULL_TYPE) {
            printf("Evaluation error: too few args passed to function\n");
            texit(0);
        }
        Value *binding = cons(car(param), car(arg));
        addBinding(binding, frame);
        arg = cdr(arg);
        param = cdr(param);
    }

    // if too many arguments are passed, throw an error.
    if (arg -> type != NULL_TYPE) {
        printf("Evaluation error: too many args passed to function\n");
        texit(0);
    }

    Value *result;
    Value *body = evaledOperator -> cl.functionCode;
    while (body -> type != NULL_TYPE) {
        result = eval(car(body), frame);
        body = cdr(body);
    }
    // return the final evaluated expression in body
    return result;
}

/*
evalDefine
params: args - a pointer to a Value struct, frame - a pointer to a Frame struct
returns: a Value struct of type VOID_TYPE
Adds the bindings requested by args to the global frame.
*/
Value *evalDefine(Value *args, Frame *frame) {
    // if no arguments or body are provided for define or too many arguments are provided, throw an error.
    if (args -> type == NULL_TYPE || cdr(args) -> type == NULL_TYPE  || cdr(cdr(args)) -> type != NULL_TYPE) {
        printf("Evaluation error: incorrect number of args for define\n");
        texit(0);
    // if the given variable for definition is not a symbol, throw an error.
    } else if (car(args) -> type != SYMBOL_TYPE) {
        printf("Evaluation error: trying to define non-variable\n");
        texit(0);
    }
    addBinding(cons(car(args), eval(car(cdr(args)), frame)), frame);

    Value *returnValue = talloc(sizeof(Value));
    returnValue -> type = VOID_TYPE;
    return returnValue;
}

/*
evalLambda
params: args - a pointer to a Value representing lambda's args; frame - a pointer to a Frame
returns: a closure corresponding to the lambda expression being evaluated
evalLambda() checks if the lambda expression's arguments are properly formatted, and if so, creates and returns a closure object corresponding to the expression.
evalLambda() also performs general error checking, including the number of args, type of function parameters, and duplicate function parameters.
*/
Value *evalLambda(Value *args, Frame *frame) {
    // if too few arguments are given for lambda, throw an error.
    if (args -> type == NULL_TYPE || cdr(args) -> type == NULL_TYPE) {
        printf("Evaluation error: incorrect number of args for lambda\n");
        texit(0);
    }
    
    Value *params = car(args);
    Value *param = params;
    Value *visited = makeNull();
    while (param -> type != NULL_TYPE) {
        // if lambda's parameters are not formatted correctly, throw an error.
        if (param -> type != CONS_TYPE) {
            printf("Evaluation error: bad param formatting in lambda\n");
            texit(0);
        // if lambda's paramters are not a symbol, throw an error.
        } else if (car(param) -> type != SYMBOL_TYPE) {
            printf("Evaluation error: non-variable param in lambda\n");
            texit(0);
        } else {
            Value *existing = visited;
            while (existing -> type != NULL_TYPE) {
                // if lambda's parameters contain duplicate identifiers, throw an error.
                if (!strcmp(car(existing) -> s, car(param) -> s)) {
                    printf("Evaluation error: duplicate identifier in lambda\n");
                    texit(0);
                }
                existing = cdr(existing);
            }
            visited = cons(car(param), visited);
            param = cdr(param);
        }
    }
    
    return makeClosure(frame, params, cdr(args));
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
    // return the final evaluated expression in body
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
            
            } else if (!strcmp(first->s, "define")) { 
                return evalDefine(args, frame);  

            } else if (!strcmp(first->s, "lambda")) {
                return evalLambda(args, frame);

            } else {
                // if not special form, evaluate first and args, then try to apply the results as a function
                Value *evaledOperator = eval(first, frame);
                Value *evaledArgs = evalEach(args, frame);
                return apply(evaledOperator, evaledArgs);

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
        case CLOSURE_TYPE: {
            printf("#<procedure>");
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
Given the pointer to a Scheme program, iteratively call eval() on each parse tree and display their respective result.
*/
void interpret(Value *tree) {
    Value *current = tree;
    Frame *global = makeFrame(NULL);
    while (current->type != NULL_TYPE) {
        Value *result = eval(car(current), global);
        int needsClose = 0;
        printingHelper(result, &needsClose);
        if (result -> type != VOID_TYPE) {
            printf("\n");
        }
        current = cdr(current);
    }
}

#endif