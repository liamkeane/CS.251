#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main () {
    char end = fgetc(stdin);
    if (end == EOF) {
        printf("blah\n");
    } else {
        printf("grr\n");
    }
}