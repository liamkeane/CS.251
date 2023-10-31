/*  Display the product of two complex numbers. */

#include <stdio.h>

/* . */
struct Complex {
    double real;
    double imaginary;
};

struct Complex multiplyComplex (struct Complex c1, struct Complex c2) {
    struct Complex tempc;
    tempc.real = ((c1.real * c2.real) - (c1.imaginary * c2.imaginary));
    tempc.imaginary = ((c1.real * c2.imaginary) + (c2.real * c1.imaginary));
    return tempc;
}

int main () {
    struct Complex cnum1;
    struct Complex cnum2;

    printf("Enter real part of c1: ");
    scanf("%lf", &cnum1.real);
    printf("Enter imaginary part of c1: ");
    scanf("%lf", &cnum1.imaginary);

    printf("Enter real part of c2: ");
    scanf("%lf", &cnum2.real);
    printf("Enter imaginary part of c2: ");
    scanf("%lf", &cnum2.imaginary);

    struct Complex c3 = multiplyComplex(cnum1,cnum2);
    printf("Answer = %10.2f + %10.2f i\n", c3.real, c3.imaginary);

    return 0;
}