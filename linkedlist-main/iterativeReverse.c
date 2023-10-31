if (list -> type == CONS_TYPE) {
        // allocate space for Value struct, set prev to a NULL_TYPE
        Value *previous = makeNull();

        // allocate space for Value struct, set current equal to the value of the first cons cell (list)
        Value *currentCar = malloc(sizeof(Value));
        currentCar -> type = car(list) -> type;
        switch (currentCar -> type) {
            case INT_TYPE:
                currentCar -> i = car(list) -> i;
                break;
            case DOUBLE_TYPE:
                currentCar -> d = car(list) -> d;
                break;
            case STR_TYPE:
                strcpy(currentCar -> s, car(list) -> s);
                break;
            case CONS_TYPE:
                break;
            case NULL_TYPE:
                break;
        }

        Value *current = cons(currentCar, previous);

        // allocate space for Value struct, set next equal to the location of the second cons cell
        Value *next = cdr(list);
        
        while (next -> type != NULL_TYPE) {
            previous = current;
            currentCar = malloc(sizeof(Value));
            currentCar -> type = car(next) -> type;
            switch (currentCar -> type) {
                case INT_TYPE:
                    currentCar -> i = car(next) -> i;
                    break;
                case DOUBLE_TYPE:
                    currentCar -> d = car(next) -> d;
                    break;
                case STR_TYPE:
                    strcpy(currentCar -> s, car(next) -> s);
                    break;
                case CONS_TYPE:
                    break;
                case NULL_TYPE:
                    break;
            }

            current = cons(currentCar, previous);
            next = cdr(next);
        }
        cleanup(list);
        return current;

    } else {
        return list;
    }

}