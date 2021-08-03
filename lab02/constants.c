#include <stdio.h>

#define NUM_CHAIRS 23
#define NUM_TABLES 4

int main() {
    printf("%s %d %s", "Number of chairs:", NUM_CHAIRS, "\n");
    printf("%s %d %s", "Number of tables:", NUM_TABLES, "\n");
    printf("%s %d %s",
           "Number of chairs and tables combined:", NUM_CHAIRS + NUM_TABLES,
           "\n");
    return 0;
}
