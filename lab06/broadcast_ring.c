#include <mpi.h>
#include <stdio.h>
int main(int argc, char **argv) {
    int rank, s_value, r_value, size;
    int tag = 0;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    do {
        if (rank == 0) {
            printf("Enter a round number: ");
            fflush(stdout);
            scanf("%d", &s_value);

            // Start the process of sending messages to the next process
            MPI_Send(&s_value, 1, MPI_INT, 1, tag, MPI_COMM_WORLD);
            MPI_Recv(&r_value, 1, MPI_INT, size - 1, tag, MPI_COMM_WORLD,
                     &status);
            printf("Process %d got %d from Process %d\n", rank, r_value,
                   size - 1);
            fflush(stdout);
        } else {

            // Recieve the message from the previous process and pass it on to the next one
            MPI_Recv(&r_value, 1, MPI_INT, rank - 1, tag, MPI_COMM_WORLD,
                     &status);
            MPI_Send(&r_value, 1, MPI_INT, (rank + 1) % size, tag,
                     MPI_COMM_WORLD);
            printf("Process %d got %d from Process %d\n", rank, r_value,
                   rank - 1);
            fflush(stdout);
        }
    } while (r_value >= 0); // Sentinel value is any number below zero
    MPI_Finalize();
    return 0;
}