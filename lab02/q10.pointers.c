#include <stdio.h>

int main() {
    int i = 5;
    printf("Value of i: %d\n", i);

    printf("Initialise pointer to i...\n");
    int* p = &i;
    printf("Address of i: %p\n", &i);
    printf("Value of p: %p\n", p);


    printf("Change the value of i using a pointer...\n");
    *p = 7;
    printf("Value of i: %d\n", i);
    printf("Value of *p: %d\n", *p);

    return 0;
}