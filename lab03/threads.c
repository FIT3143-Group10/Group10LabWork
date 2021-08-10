#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

bool is_prime(int n);

int main(int argc, char *argv[]) {
    struct timespec start, finish;
    clock_gettime(CLOCK_MONOTONIC, &start);
    // clock_t ticks;
    // ticks = clock();
    
    // start serial portion
    int cap = atoi(argv[1]);
    FILE* primes;
    primes = fopen("primes.txt", "w");
    // End serial portion.
    // ticks = clock() - ticks;
    // printf("Serial ticks: %ld\n", ticks);

    // Start parallel portion.
    for (int i = 2; i < cap; i++) {
        if (is_prime(i)) {
            fprintf(primes, "%d\n", i);
        }
    }
    // End parallel portion.
    // ticks = clock() - ticks;
    // printf("Parallel ticks: %ld\n", ticks);

    clock_gettime(CLOCK_MONOTONIC, &finish);
    double elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("Total time: %f\n", elapsed);

    fclose(primes);
    return 0;
}

bool is_prime(int n) {
    for (int i = 2; i <= sqrt(n); i++) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}
