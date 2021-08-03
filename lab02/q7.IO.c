#include <stdio.h>

int main() {
    int chars = 0, i;
    char input[100];
    i = getchar();
    while (chars < 99 && i != EOF) {
        input[chars] = i;
        chars++;
        i = getchar();
    }

    printf("\n%s\n", input);

    return 0;
}