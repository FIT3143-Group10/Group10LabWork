#include <stdio.h>
#include <string.h>

int main() {
    char* hello = "hello world";
    printf("%s\n", hello);

    char hello2[100];
    strcpy(hello2, hello);
    hello = "hello";
    printf("%s\n", hello2);
    printf("%s\n", hello);

    char hello_world[100];
    strcat(hello_world, hello);
    strcat(hello_world, " world");
    printf("%s\n", hello_world);

    return 0;
}
