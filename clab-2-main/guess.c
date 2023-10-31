#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main () {
    printf("Enter a random seed: ");
    unsigned int seed;
    scanf("%u", &seed);
    srandom(seed);

    //srandom((unsigned int)time(NULL));
    int randNum = random()%100 + 1;

    // initialize count, guess variables
    int count = 1;
    int guess = 0;
    while (randNum != guess) {
        printf("Guess a number: ");
        scanf("%i", &guess);
        if (guess < randNum) {
            printf("Too low!\n");
        } else if (guess > randNum) {
            printf("Too high!\n");
        } else {
            printf("Correct! Total guesses = %i\n", count);
        }
        count++;
    }

    return 0;
}
