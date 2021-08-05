#include <stdio.h>

int main() {
    // Console IO.
    int chars = 0, i;
    char input[100] = "";
    i = getchar();
    while (chars < 99 && i != (int) '\n') {
        input[chars] = i;
        chars++;
        i = getchar();
    }

    printf("Input: %s\n", input);

    // File IO.
    // Write to file.
    FILE* file;
    file = fopen("q7.txt", "w");
    fprintf(file, "%s\n", input);
    fclose(file);
    printf("Wrote input to q7.txt.\n");
    
    // Count words in file.
    file = fopen("q7.txt", "r");
    char str[100];
    char c[1];
    int count = 0;
    while (fscanf(file, "%s%c", str, c) != EOF) {
        count++;
    }
    fclose(file);
    printf("Word count: %d\n", count);

    return 0;
}