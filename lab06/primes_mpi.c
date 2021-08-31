#include <math.h>
#include <mpi.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

bool is_prime(int n);

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

int main(int argc, char *argv[]) {
    // Initialise the timer
    struct timespec start, finish;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Setup MPI
    int rank, size, n_limit;
    int tag = 0;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Master process asks for input
    if (rank == 0) {
        printf("Enter the value of n: ");
        fflush(stdout);
        scanf("%d", &n_limit);
    }
    // Broadcast value of N to all processes
    MPI_Bcast(&n_limit, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int segment_size = n_limit / size;
    // int prime_nums[segment_size];
    int* prime_nums = (int*) malloc(segment_size * sizeof(int));

    int num_found = 0;
    // Divide effort among processes and run prime search
    for (int i = 2 + segment_size * rank;
         i <= MIN((rank + 1) * segment_size + 1, n_limit); i++) {
        if (is_prime(i)) {
            // fprintf(primes, "%d\n", i);
            prime_nums[num_found] = i;
            num_found++;
        }
    }

    if (rank == 0) {
        FILE *primes;
        primes = fopen("primes.txt", "w");
        for (int i = 0; i < num_found; i++) {
            fprintf(primes, "%d\n", prime_nums[i]);
        }
        for (int r = 1; r < size; r++) {
            MPI_Recv(&num_found, 1, MPI_INT, r, tag, MPI_COMM_WORLD, &status);
            MPI_Recv(prime_nums, num_found, MPI_INT, r, tag, MPI_COMM_WORLD,
                     &status);
            for (int i = 0; i < num_found; i++) {
                fprintf(primes, "%d\n", prime_nums[i]);
            }
        }
    } else {
        MPI_Send(&num_found, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
        MPI_Send(prime_nums, num_found, MPI_INT, 0, tag, MPI_COMM_WORLD);
    }

    free(prime_nums);

    // Stop the timer and print the total time taken
    clock_gettime(CLOCK_MONOTONIC, &finish);
    double elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("Total time: %f\n", elapsed);

    MPI_Finalize();
    return 0;
}

// Helper function to determine whether a number is prime or not
bool is_prime(int n) {
    for (int i = 2; i <= sqrt(n); i++) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}
