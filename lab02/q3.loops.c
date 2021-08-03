#include <stdio.h>

int main() {
    // For loop.
    printf("For loop:\n");
    for (int i = 0; i <= 9; i++) {
        printf("%d\n", i);
    }

    // While loop.
    printf("While loop:\n");
    int i = 0;
    while (i <= 9) {
        printf("%d\n", i);
        i++;
    }

    // Loop using goto.
    printf("Goto loop:\n");
    i = 0;
start:
    printf("%d\n", i);
    if (i < 9) {
        i++;
        goto start;
    }

    return 0;
}
