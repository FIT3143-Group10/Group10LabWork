#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

int master_io(MPI_Comm master_comm, MPI_Comm comm);
int slave_io(MPI_Comm master_comm, MPI_Comm comm);

int main(int argc, char **argv) {
    int rank;
    
    MPI_Comm new_comm;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_split(MPI_COMM_WORLD, rank == 0, 0, &new_comm);
    
    if (rank == 0)
        master_io(MPI_COMM_WORLD, new_comm);
    else
        slave_io(MPI_COMM_WORLD, new_comm);
    
    MPI_Finalize();
    return 0;
}

/* This is the master */
int master_io(MPI_Comm master_comm, MPI_Comm comm) {
    int i, j, size;
    char buf[256];
    MPI_Status status;
    MPI_Comm_size(master_comm, &size);
    for (j = 1; j <= 2; j++) {
        for (i = 1; i < size; i++) {
            MPI_Recv(buf, 256, MPI_CHAR, i, 0, master_comm, &status);
            fputs(buf, stdout);
        }
    }

    char** buffers = malloc((size - 1) * sizeof(char*));
    for (i = 0; i < size - 1; i++) {
        buffers[i] = malloc(256 * sizeof(char));
    }

    MPI_Request unordered_requests[size - 1];
    for (i = 1; i < size; i++) {
        MPI_Irecv(buffers[i - 1], 256, MPI_CHAR, i, 0, master_comm, &unordered_requests[i - 1]);
    }
    int exits = 0;
    int* printed = calloc(size - 1, sizeof(int));
    while (exits < size - 1) {
        for (i = 1; i < size; i++) {
            int flag;
            MPI_Test(&unordered_requests[i - 1], &flag, MPI_STATUS_IGNORE);
            if (flag && !printed[i - 1]) {
                fputs(buffers[i - 1], stdout);
                printed[i - 1] = 1;
                exits++;
            }
        }
    }

    for (i = 0; i < size - 1; i++) {
        free(buffers[i]);
    }
    free(buffers);
    free(printed);
    return 0;
}

/* This is the slave */
int slave_io(MPI_Comm master_comm, MPI_Comm comm) {
    char buf[256];
    int rank;

    MPI_Comm_rank(comm, &rank);
    sprintf(buf, "Hello from slave %d\n", rank);
    MPI_Send(buf, strlen(buf) + 1, MPI_CHAR, 0, 0, master_comm);

    sprintf(buf, "Goodbye from slave %d\n", rank);
    MPI_Send(buf, strlen(buf) + 1, MPI_CHAR, 0, 0, master_comm);

    sprintf(buf, "I'm exiting (%d)\n", rank);
    MPI_Request send_req;
    MPI_Isend(buf, strlen(buf) + 1, MPI_CHAR, 0, 0, master_comm, &send_req);
    return 0;
}
