#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define N 10
#define MAX_NUM 25

int main() {
    omp_set_num_threads(4);     // Set number of threads.
    srand(time(NULL));          // Seed RNG.
    int num[N];  

    // Generate random numbers in parallel.
    #pragma omp parallel for shared(num) \
        schedule(static, 2) // Set the chunk size to 2.
    for (int i = 0; i < N; i++) {
        num[i] = (rand() % MAX_NUM) + 1;
    }    
    
    // Display generated numbers.
    for (int i = 0; i < N; i++) {
        printf("%d ", num[i]);
    }
    printf("\n");

    // Find duplicates and the number of wins.
    // Brute force approach used since the problem size is small.
    int wins = 0;
    bool found;
    for(int i = 1; i <= MAX_NUM; i++) {
        found = false;
        for (int j = 0; j < N; j++) {
            if (num[j] == i) {
                if (found) {
                    wins++;
                    break;
                } else {
                    found = true;
                }
            }
        }
    }
    printf("Wins: %d\n", wins);
}
