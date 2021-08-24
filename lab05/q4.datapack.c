#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv) {
    int myrank;
    int a = -1;
    double b = -1;
    int packsize;
    int position;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    int int_size, double_size;
    MPI_Pack_size(1, MPI_INT, MPI_COMM_WORLD, &int_size);
    MPI_Pack_size(1, MPI_DOUBLE, MPI_COMM_WORLD, &double_size);
    char packbuf[int_size + double_size];

    // Create MPI struct
    do {
        if (myrank == 0) {
            printf("Enter an round number (>0) & a real number: ");
            fflush(stdout);
            scanf("%d%lf", &a, &b);

            packsize = 0;
            MPI_Pack(&a, 1, MPI_INT, packbuf, 100, &packsize, MPI_COMM_WORLD);
            MPI_Pack(&b, 1, MPI_DOUBLE, packbuf, 100, &packsize, MPI_COMM_WORLD);
        }
        MPI_Bcast(&packsize, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&packbuf, packsize, MPI_PACKED, 0, MPI_COMM_WORLD);
    
        position = 0;
        MPI_Unpack(&packbuf, packsize, &position, &a, 1, MPI_INT, MPI_COMM_WORLD);
        MPI_Unpack(&packbuf, packsize, &position, &b, 1, MPI_DOUBLE, MPI_COMM_WORLD);
        printf("Rank: %d. a = %d. b = %lf\n", myrank, a, b);
        fflush(stdout);
    } while (a > 0);
    
    MPI_Finalize();
    return 0;
}

