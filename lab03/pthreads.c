#include <math.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

FILE *primes;
pthread_mutex_t file_mutex;

void *check_primes(void *bnd);
bool is_prime(int n);

struct bounds {
    int lower;
    int upper;
};

int main(int argc, char *argv[]) {

    struct timespec start, finish;
    double elapsed;
    
    clock_gettime(CLOCK_MONOTONIC, &start);

    pthread_mutex_init(&file_mutex, NULL);
    primes = fopen("primes.txt", "w");
    pthread_t t1, t2, t3, t4;

    int cap = atoi(argv[1]);
    int step = cap / 4;
    int t1_cap = step;
    int t2_cap = step * 2;
    int t3_cap = step * 3;

    struct bounds t1_bound, t2_bound, t3_bound, t4_bound;

    t1_bound.lower = 2;
    t1_bound.upper = t1_cap; 
    t2_bound.lower = t1_cap + 1;
    t2_bound.upper = t2_cap;
    t3_bound.lower = t2_cap + 1;
    t3_bound.upper = t3_cap;
    t4_bound.lower = t3_cap + 1;
    t4_bound.upper = cap;

    int iret1 = pthread_create(&t1, NULL, check_primes, &t1_bound);
    int iret2 = pthread_create(&t2, NULL, check_primes, &t2_bound);
    int iret3 = pthread_create(&t3, NULL, check_primes, &t3_bound);
    int iret4 = pthread_create(&t4, NULL, check_primes, &t4_bound);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);

    fclose(primes);
    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("Total time taken (seconds): %f\n", elapsed);
    return 0;
}

void *check_primes(void *bnd) {
    struct bounds *b = (struct bounds *)bnd;
    int lower = b->lower, upper = b->upper;
    for (int n = lower; n <= upper; n++) {
        if (is_prime(n)) {
            pthread_mutex_lock(&file_mutex);
            fprintf(primes, "%d\n", n);
            pthread_mutex_unlock(&file_mutex);
        }
    }
}

bool is_prime(int n) {
    for (int i = 2; i <= sqrt(n); i++) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}
