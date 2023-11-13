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
primtiveMinus
params:
returns:
*/
Value *primitiveMinus(Value *args) {
    // Checks if no arguments were provided, and if so throw an error
    if (args -> type == NULL_TYPE) {
        printf("Evaluation error: no argument provided for '-'\n");
        texit(0);
    }

    int differenceAsInt = 0;
    double differenceAsDouble = 0;
    bool allInts = true;

    Value *current = args;

    // case where there is one argument
    if (cdr(args) -> type != NULL_TYPE) {
        if (car(args) -> type == DOUBLE_TYPE) {
            allInts = false;
            differenceAsDouble = car(args) -> d;
            current = cdr(current);
        } else if (car(args) -> type == INT_TYPE) {
            differenceAsInt = car(args) -> i;
            current = cdr(current);
        }
    }

    // case where there are more than one argument
    Value *currentValue;
    while (current -> type != NULL_TYPE) {
        currentValue = car(current);
        if (currentValue -> type != INT_TYPE && currentValue -> type != DOUBLE_TYPE) {
            printf("Evaluation error: non real-number arguments for '-'\n");
            texit(0);
        // If a double type seen in the arguments, switches sum to be stored as a double
        } else if (currentValue -> type == DOUBLE_TYPE && allInts) {
            differenceAsDouble = differenceAsInt - currentValue -> d;
            allInts = false;      
        } else if (allInts) {
            differenceAsInt = differenceAsInt - currentValue -> i;
        } else {
            if (currentValue -> type == INT_TYPE) {
                differenceAsDouble = differenceAsDouble - currentValue -> i;
            } else {
                differenceAsDouble = differenceAsDouble - currentValue -> d;
            }
        }
        current = cdr(current);
    }

    // make sure result is of the proper type
    Value *result = talloc(sizeof(Value));
    if (allInts) {
        result -> type = INT_TYPE;
        result -> i = differenceAsInt;
    } else {
        result -> type = DOUBLE_TYPE;
        result -> d = differenceAsDouble;
    }

    return result;
}

/*
primtiveLessThan
params:
returns:
*/
Value *primitiveLessThan(Value *args) {
    // result is initially set to true
    Value *result = talloc(sizeof(Value));
    result -> type = BOOL_TYPE;
    result -> i = 1;

    // Check if there are no arguments
    if (args -> type == NULL_TYPE) {
        return result;
    }

    Value *current = args;
    // Check if the first argument is a numerial type
    if (car(current) -> type != INT_TYPE && car(current) -> type != DOUBLE_TYPE) {
        printf("Evaluation error: non numerical argument for '<'\n");
        texit(0); 
    }

    while (cdr(current) -> type != NULL_TYPE) {
        // Check if the next argument is a numerical type
        if (car(cdr(current)) -> type != INT_TYPE && car(cdr(current)) -> type != DOUBLE_TYPE) {
            printf("Evaluation error: non numerical argument for '<'\n");
            texit(0); 
        }
        // Is the current argument of double type?
        if (car(current) -> type == DOUBLE_TYPE) {
            // Is the next argument of double type?
            if (car(cdr(current)) -> type == DOUBLE_TYPE) {
                if (car(current) -> d >= car(cdr(current)) -> d) {
                    result -> i = 0;
                    return result;
                }
            // The next argument must be of int type
            } else {
                if (car(current) -> d >= car(cdr(current)) -> i) {
                    result -> i = 0;
                    return result;
                }
            }

        // The current argument must be of int type
        } else {
            // Is the next argument of double type?
            if (car(cdr(current)) -> type == DOUBLE_TYPE) {
                if (car(current) -> i >= car(cdr(current)) -> d) {
                    result -> i = 0;
                    return result;
                }
            // The next argument must be of int type
            } else {
                if (car(current) -> i >= car(cdr(current)) -> i) {
                    result -> i = 0;
                    return result;
                }
            }
        }
        current = cdr(current);
    }

    return result;
}

/*
primtiveGreatorThan
params:
returns:
*/
Value *primitiveGreatorThan(Value *args) {
    // result is initially set to true
    Value *result = talloc(sizeof(Value));
    result -> type = BOOL_TYPE;
    result -> i = 1;

    // Check if there are no arguments
    if (args -> type == NULL_TYPE) {
        return result;
    }

    Value *current = args;
    // Check if the first argument is a numerial type
    if (car(current) -> type != INT_TYPE && car(current) -> type != DOUBLE_TYPE) {
        printf("Evaluation error: non numerical argument for '>'\n");
        texit(0); 
    }

    while (cdr(current) -> type != NULL_TYPE) {
        // Check if the next argument is a numerical type
        if (car(cdr(current)) -> type != INT_TYPE && car(cdr(current)) -> type != DOUBLE_TYPE) {
            printf("Evaluation error: non numerical argument for '>'\n");
            texit(0); 
        }
        // Is the current argument of double type?
        if (car(current) -> type == DOUBLE_TYPE) {
            // Is the next argument of double type?
            if (car(cdr(current)) -> type == DOUBLE_TYPE) {
                if (car(current) -> d <= car(cdr(current)) -> d) {
                    result -> i = 0;
                    return result;
                }
            // The next argument must be of int type
            } else {
                if (car(current) -> d <= car(cdr(current)) -> i) {
                    result -> i = 0;
                    return result;
                }
            }

        // The current argument must be of int type
        } else {
            // Is the next argument of double type?
            if (car(cdr(current)) -> type == DOUBLE_TYPE) {
                if (car(current) -> i <= car(cdr(current)) -> d) {
                    result -> i = 0;
                    return result;
                }
            // The next argument must be of int type
            } else {
                if (car(current) -> i <= car(cdr(current)) -> i) {
                    result -> i = 0;
                    return result;
                }
            }
        }
        current = cdr(current);
    }

    return result;
}

/*
primtiveEqual
params:
returns:
*/
Value *primitiveEqual(Value *args) {
    // result is initially set to true
    Value *result = talloc(sizeof(Value));
    result -> type = BOOL_TYPE;
    result -> i = 1;

    Value *current = args;
    while (current -> type != NULL_TYPE) {
        // Checks if argument is neither a float nor an int
        if (car(current) -> type != INT_TYPE && car(current) -> type != DOUBLE_TYPE) {
            printf("Evaluation error: non numerical argument for '='\n");
            texit(0);
        }

        // is the current arg of type double?
        if (car(current) -> type == DOUBLE_TYPE) {
            // is the first arg of type double?
            if (car(args) -> type == DOUBLE_TYPE) {
                if (car(current) -> d != car(args) -> d) {
                    result -> i = 0;
                    return result;
                }
            } else {
                if (car(current) -> d != car(args) -> i) {
                    result -> i = 0;
                    return result;
                }
            }
        
        // current arg is an int type
        } else {
            // is the first arg of type double?
            if (car(args) -> type == DOUBLE_TYPE) {
                if (car(current) -> i != car(args) -> d) {
                    result -> i = 0;
                    return result;
                }
            } else {
                if (car(current) -> i != car(args) -> i) {
                    result -> i = 0;
                    return result;
                }
            }
        }

        current = cdr(current);
    }
    return result;
}

/*
primitivePlus
params: args - a pointer to a Value representing a linked list of arguments
returns: a pointer to a Value containing an integer or double that equals the sum of the arguments
primitivePlus() throws an error if it encounters a non real-number argument.
If no arguments are provided, primitivePlus returns a pointer to a Value containing 0.
*/
Value *primitivePlus(Value *args) {
    // Checks if no arguments were provided, and if so returns a pointer to an integer-type Value containing 0
    if (args -> type == NULL_TYPE) {
        Value *result = talloc(sizeof(Value));
        result -> type = INT_TYPE;
        result -> i = 0;
    }
   
   Value *current = args;
   Value *currentValue;
   int sumAsInt = 0;
   double sumAsDouble = 0;
   bool allInts = true;
   while (current -> type != NULL_TYPE) {
        currentValue = car(current);
        if (currentValue -> type != INT_TYPE && currentValue -> type != DOUBLE_TYPE) {
            printf("Evaluation error: attempting to sum non real-number arguments\n");
            texit(0);
        // If a double type seen in the arguments, switches sum to be stored as a double
        } else if (currentValue -> type == DOUBLE_TYPE && allInts) {
            sumAsDouble = sumAsInt + currentValue -> d;
            allInts = false;      
        } else if (allInts) {
            sumAsInt = sumAsInt + currentValue -> i;
        } else {
            if (currentValue -> type == INT_TYPE) {
                sumAsDouble = sumAsDouble + currentValue -> i;
            } else {
                sumAsDouble = sumAsDouble + currentValue -> d;
            }
        }
        current = cdr(current);
    }

    // make sure result is of the proper type and has its data stored in the proper locations
    Value *result = talloc(sizeof(Value));
    if (allInts) {
        result -> type = INT_TYPE;
        result -> i = sumAsInt;
    } else {
        result -> type = DOUBLE_TYPE;
        result -> d = sumAsDouble;
    }

    return result;
}

/*
primitiveNull
params: args - a pointer to a Value representing a linked list of arguments
returns: a pointer to a boolean-type Value
primitiveNull() throws an error if greater or fewer than one argument is provided.
The boolean-type Value returned by primitiveNull() will contain true if the argument was an empty list, and false in any other case.
The 
*/
Value *primitiveNull(Value *args) {
    if (args -> type == NULL_TYPE || cdr(args) -> type != NULL_TYPE) {
        printf("Evaluation error: incorrect number of args for 'null?'\n");
        texit(0);
    } else {
        Value *arg = car(args);
        Value *result = talloc(sizeof(Value));
        result -> type = BOOL_TYPE;
        if (args -> type == CONS_TYPE) {
            if (isNull(arg)) {
                result -> i = 1;
            } else {
                result -> i = 0;
            }
        } else {
            result -> i = 0;
        }
        return result;
    }
    return makeNull();
}

/*
primitiveCar
params: args - a pointer to a Value representing a linked list of arguments
returns: a pointer to a Value representing the first item in a given list
primitiveCar() will throw an error if it is given greater or fewer than one argument.
*/
Value *primitiveCar(Value *args) {
    if (args -> type == NULL_TYPE || cdr(args) -> type != NULL_TYPE) {
        printf("Evaluation error: incorrect number of args for 'car'\n");
        texit(0);
    } else {
        Value *arg = car(args);
        if (arg -> type != CONS_TYPE) {
            printf("Evaluation error: argument to car is not a cons cell\n");
            texit(0);
        } else {
            return car(arg);
        }
    }
    return makeNull();
}

/*
primitiveCdr
params: a pointer to a Value representing a linked list of arguments
returns: a pointer to a Value representing everything but the first item in a given list
primitiveCdr() will throw an error if it is given greater or fewer than one argument.
*/
Value *primitiveCdr(Value *args) {
    if (args -> type == NULL_TYPE || cdr(args) -> type != NULL_TYPE) {
        printf("Evaluation error: incorrect number of args for 'cdr'\n");
        texit(0);
    } else {
        Value *arg = car(args);
        if (arg -> type != CONS_TYPE) {
            printf("Evaluation error: argument to cdr is not a cons cell\n");
            texit(0);
        } else {
            return cdr(arg);
        }
    }
    return makeNull();
}

/*
primitiveCons
params: args - a pointer to a Value struct
returns: a pointer to a Value struct
Returns a Cons cell of the two values contained within args. If args does not contain exactly two args, throw an error.
*/
Value *primitiveCons(Value *args) {
    // If args does not contain exactly two args, throw an error.
    if (args -> type == NULL_TYPE || cdr(args) -> type == NULL_TYPE || cdr(cdr(args)) -> type != NULL_TYPE) {
        printf("Evaluation error: incorrect number of args for 'cons'\n");
        texit(0);

    } else {
        return cons(car(args), car(cdr(args)));
    }
    return makeNull();
}

/*
bind
params: name - a pointer to a string, function - a pointer to a function, frame - a pointer to a Frame struct
returns: nothing
bind() adds a definition to the global frame where the given name is the key and the function pointer is its value.
*/
void bind(char *name, Value *(*function)(struct Value *), Frame *frame) {
    Value *functionValue = talloc(sizeof(Value));
    functionValue -> type = PRIMITIVE_TYPE;
    functionValue -> pf = function;
    
    Value *nameValue = talloc(sizeof(Value));
    nameValue -> type = SYMBOL_TYPE;
    nameValue -> s = name;

    Value *binding = cons(nameValue, functionValue);
    
    frame -> bindings = cons(binding, frame -> bindings);
}

/*
evalEach
params: args - a pointer to a Value struct, frame - a pointer to a Frame struct
returns: a Value struct containing the evaluated arguments to be passed into apply()
*/
Value *evalEach(Value *args, Frame *frame, bool needsReversal) {
    Value *evaledArgs = makeNull();
    Value *arg = args;
    while (arg -> type != NULL_TYPE) {
        evaledArgs = cons(eval(car(arg), frame), evaledArgs);
        arg = cdr(arg);
    }
    if (needsReversal && evaledArgs -> type != NULL_TYPE) {

        //reverse the list of parse trees
        Value *prev = makeNull();
        Value *current = evaledArgs;
        Value *next = cdr(evaledArgs);
        while (next->type != NULL_TYPE) {
            current->c.cdr = prev;
            prev = current;
            current = next;
            next = cdr(next);
        }
        current->c.cdr = prev;
        evaledArgs = current;
    }
    return evaledArgs;
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
params: evaledOperator - a pointer to a Value struct that represents a closure corresponding to a function; evaledArgs - a pointer to a value representing a list of previously evaluated function arguments
returns: the result of evaluating the body contained in the given closure, in the context of evaledArgs and the closure's environment
apply() builds a new frame whose parent is the environment specified in the given closure, and adds bindings to it corresponding to each parameter/argument pair.
apply() then evaluates the function body specified in the given closure in the context of the new frame, and returns the result.
*/
Value *apply(Value *evaledOperator, Value *evaledArgs) {
    // if the given operator is not a function, throw an error.
    if (evaledOperator -> type != CLOSURE_TYPE && evaledOperator -> type != PRIMITIVE_TYPE) {
        printf("Evaluation error: non-function being called as function\n");
        texit(0);
    
    //
    } else if (evaledOperator -> type == PRIMITIVE_TYPE) {
        Value *result = (evaledOperator -> pf)(evaledArgs);
        return result;
        
    // 
    } else {
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
    return makeNull();
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
            return car(currentBinding);
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
evalBegin
params:
returns:
*/
Value *evalBegin(Value *args, Frame *frame) {
    // if there are no arguments, return a Value of VOID_TYPE
    if (args -> type == NULL_TYPE) {
        Value *returnValue = talloc(sizeof(Value));
        returnValue -> type = VOID_TYPE;
        return returnValue;
    }
    
    // else, return the evaluated final argument
    Value *result;
    while (args -> type != NULL_TYPE) {
        result = eval(car(args), frame);
        args = cdr(args);
    }
    return result;
}

/*
evalSetbang
params:
returns:
*/
Value *evalSetbang(Value *args, Frame *frame) {
    // if no arguments or body are provided for define or too many arguments are provided, throw an error.
    if (args -> type == NULL_TYPE || cdr(args) -> type == NULL_TYPE  || cdr(cdr(args)) -> type != NULL_TYPE) {
        printf("Evaluation error: incorrect number of args for 'set!'\n");
        texit(0);
    // if the given variable for definition is not a symbol, throw an error.
    } else if (car(args) -> type != SYMBOL_TYPE) {
        printf("Evaluation error: trying to reassign non-variable with 'set!'\n");
        texit(0);
    }
    // find symbol, then reassign its value
    Value *symbol = lookUpSymbol(car(args), frame);
    symbol -> c.cdr = eval(car(cdr(args)), frame);

    // return Value of VOID_TYPE
    Value *returnValue = talloc(sizeof(Value));
    returnValue -> type = VOID_TYPE;
    return returnValue;
}

/*
evalLetrec
params:
returns:
*/
Value *evalLetrec(Value *args, Frame *frame) {
    // if no arguments or body are provided for let, throw an error.
    if (args -> type == NULL_TYPE || cdr(args) -> type == NULL_TYPE) {
        printf("Evaluation error: incorrect number of args for letrec\n");
        texit(0);

    }
    // create new frame in which to evaluate letrec
    Frame *newFrame = makeFrame(frame);
    
    // checks list of bindings to make sure it is a proper list; throws error if not
    if (car(args) -> type != CONS_TYPE && car(args) -> type != NULL_TYPE) {
        printf("Evaluation error: invalid letrec binding\n");
        texit(0);
    }

    Value *unspecValue = talloc(sizeof(Value));
    unspecValue -> type = UNSPECIFIED_TYPE;
    Value *binding = car(args);

    // checks proper nested list formatting for list of bindings; throws error if bindings are incorrectly formatted
    while(binding -> type != NULL_TYPE) {
        // check outer list format
        if (binding -> type != CONS_TYPE) {
            printf("Evaluation error: invalid letrec binding\n");
            texit(0);

        // check each binding itself
        } else if (car(binding) -> type != CONS_TYPE) {
            printf("Evaluation error: invalid letrec binding\n");
            texit(0);

        // create binding and temporarily set its value to UNSPECIFIED_TYPE
        } else {
            addBinding(cons(car(car(binding)), unspecValue), newFrame);
            binding = cdr(binding);
        }
    }

    // reverse list of bindings to ensure they are assigned the correct values
    Value *prev = makeNull();
    Value *current = newFrame -> bindings;
    Value *next = cdr(current);
    while (next->type != NULL_TYPE) {
        current->c.cdr = prev;
        prev = current;
        current = next;
        next = cdr(next);
    }
    current->c.cdr = prev;
    newFrame -> bindings = current;

    // evaluate each expression in the context of newFrame and add the result to bindings
    Value *unspecBindings = newFrame -> bindings;
    binding = car(args);
    while(binding -> type != NULL_TYPE) {
        car(unspecBindings) -> c.cdr = eval(car(cdr(car(binding))), newFrame);
        unspecBindings = cdr(unspecBindings);
        binding = cdr(binding);
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
eval
params: tree - a pointer to a Value struct, frame - a pointer to a Frame struct
returns: a pointer to a Value struct
Given a pointer to a parse tree and a pointer to a frame, evaluate the parse tree in the context of the current frame.
*/
Value *eval(Value *tree, Frame *frame) {
    switch (tree->type)  {
        case UNSPECIFIED_TYPE: {
            printf("Evaluation error: must not evaluate another variable before values are computed\n");
            texit(0);
        }
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
            return cdr(lookUpSymbol(tree, frame));
        }  
        case CONS_TYPE: {
            Value *first = car(tree);
            Value *args = cdr(tree);

            if (first -> type != SYMBOL_TYPE && first -> type != CONS_TYPE) {
                printf("Evaluation error: given type not a function\n");
                texit(0);

            } else if (!strcmp(first -> s, "if")) {
               return evalIf(args, frame);
               
            } else if (!strcmp(first -> s, "let")) {
                return evalLet(args, frame);

            } else if (!strcmp(first -> s, "letrec")) {
                return evalLetrec(args, frame);

            } else if (!strcmp(first -> s, "quote")) {
                // if there are none or multiple args given to quote, throw an error.
                if (args -> type != CONS_TYPE || cdr(args) -> type != NULL_TYPE) {
                    printf("Evaluation error: incorrect number of args for quote\n");
                    texit(0);
                } else {
                    return car(args);
                }
            
            } else if (!strcmp(first->s, "define")) { 
                return evalDefine(args, frame);  

            } else if (!strcmp(first->s, "lambda")) {
                return evalLambda(args, frame);

            } else if (!strcmp(first->s, "set!")) {
                return evalSetbang(args, frame); 

            } else if (!strcmp(first->s, "begin")) {
                return evalBegin(args, frame);

            } else {
                // if not special form, evaluate first and args, then try to apply the results as a function
                Value *evaledOperator = eval(first, frame);
                bool needsReversal = true;
                if (!strcmp(first->s, "car") || !strcmp(first->s, "cdr")) {
                    needsReversal = false;
                }
                Value *evaledArgs = evalEach(args, frame, needsReversal);

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
void printingHelper(Value *tree) {
    Value *current;
    Value *currentCar;
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
            Value *current = tree;
            printf("(");
            while (current -> type != NULL_TYPE) {
                currentCar = car(current);
                if (cdr(current) -> type != CONS_TYPE && cdr(current) -> type != NULL_TYPE) {
                    printingHelper(currentCar);
                    printf(". ");
                    printingHelper(cdr(current));
                    break;
                } else {
                    printingHelper(currentCar);
                }
                current = cdr(current);
            }
            printf(") ");
            break;
        }
        case NULL_TYPE: {
            printf("()");
            break;
        }
        case CLOSURE_TYPE: {
            printf("#<procedure>");
            break;
        }
        case PRIMITIVE_TYPE: {
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
    
    //add primitive functions to the global frame
    bind("+", primitivePlus, global);
    bind("-", primitiveMinus, global);
    bind("=", primitiveEqual, global);
    bind("null?", primitiveNull, global);
    bind("car", primitiveCar, global);
    bind("cdr", primitiveCdr, global);
    bind("cons", primitiveCons, global);
    bind(">", primitiveGreatorThan, global);
    bind("<", primitiveLessThan, global);

    while (current->type != NULL_TYPE) {
        Value *result = eval(car(current), global);
        int needsClose = 0;
        printingHelper(result);
        if (result -> type != VOID_TYPE) {
            printf("\n");
        }
        current = cdr(current);
    }
}

// int main() {
//     Value *tokens = tokenize();
//     Value *parseTree = parse(tokens);
//     interpret(parseTree);
// }

#endif