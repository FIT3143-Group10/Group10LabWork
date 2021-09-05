#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <stdbool.h>
#include <time.h>
#include "IntQueue.h"

#define PRIVILEGE 1

#define ALLOWED 0
#define DENIED -1

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

int critical_section();

int rank, size;
bool has_privilege = false, requesting = false;
int* RN;
int* LN;
IntQueue* Q = NULL;

int main(int argc, char* argv[]) {
    srand(time(NULL));

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /// Initialisation.
    RN = malloc(size * sizeof(int));
    // Initialise elements in RN to -1.
    for (size_t i = 0; i < size; i++) {
        RN[i] = -1;
    }
    LN = malloc(size * sizeof(int));
    queue_init(&Q, size);
    if (rank == 0) {
        has_privilege = true;
        for (size_t i = 0; i < size; i++) {
            LN[i] = -1;
        }
    }

    /// Main Loop.
    int temp[2];
    // Pick a random process to enter the critical section.
    int chosen = (rand() % (size - 1)) + 1;
    if (rank == chosen) {
        printf("Process %d wants to enter critical section.\n", chosen);
        requesting = true;
        // If we don't have the privilege, request it.
        if (!has_privilege) {
            RN[rank] += 1;
            for (int dest = 0; dest < size; dest++) {
                if (dest != rank) {
                    MPI_Request req;
                    // Broadcast (j, RN[j]).
                    temp[0] = rank;
                    temp[1] = RN[rank];
                    MPI_Ibcast(temp, 2, MPI_INT, rank, MPI_COMM_WORLD, &req);
                }
            }
            // Receive Q.
            MPI_Recv(&Q->head, 1, MPI_INT, MPI_ANY_SOURCE, PRIVILEGE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&Q->num_el, 1, MPI_INT, MPI_ANY_SOURCE, PRIVILEGE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(Q->items, size, MPI_INT, MPI_ANY_SOURCE, PRIVILEGE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Status status;
            // Receive LN array.
            MPI_Recv(LN, size, MPI_INT, MPI_ANY_SOURCE, PRIVILEGE, MPI_COMM_WORLD, &status);
            has_privilege = true;
            printf("Privilege sent to process %d from process %d.\n", rank, status.MPI_SOURCE);
        }
        // Can now enter the critical section.
        critical_section();

        // Check if there are other nodes requesting privilege.
        LN[rank] = RN[rank];
        for (int i = 0; i < size; i++) {
            if (i != rank && RN[i] == LN[i] + 1) {
                queue_append(Q, i);
            }
        }
        // If there are, send privilege to them.
        int num;
        queue_count_elements(Q, &num);
        if (num) {
            int dest_node;
            queue_pop(Q, &dest_node);
            // Send Q.
            MPI_Send(&Q->head, 1, MPI_INT, dest_node, PRIVILEGE, MPI_COMM_WORLD);
            MPI_Send(&Q->num_el, 1, MPI_INT, dest_node, PRIVILEGE, MPI_COMM_WORLD);
            MPI_Send(Q->items, size, MPI_INT, dest_node, PRIVILEGE, MPI_COMM_WORLD);
            // Send LN array.
            MPI_Send(LN, size, MPI_INT, dest_node, PRIVILEGE, MPI_COMM_WORLD);
        }
    } else {
        int in_rank, in_num;
        MPI_Request request;
        // Wait for other nodes' requests.
        // Receive (j, RN[j]).
        MPI_Ibcast(temp, 2, MPI_INT, chosen, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, MPI_STATUS_IGNORE);
        in_rank = temp[0];
        in_num = temp[1];
        RN[in_rank] = MAX(RN[in_rank], in_num);
        if (has_privilege && !requesting) {
            has_privilege = false;
            // Send Q.
            MPI_Send(&Q->head, 1, MPI_INT, chosen, PRIVILEGE, MPI_COMM_WORLD);
            MPI_Send(&Q->num_el, 1, MPI_INT, chosen, PRIVILEGE, MPI_COMM_WORLD);
            MPI_Send(Q->items, size, MPI_INT, chosen, PRIVILEGE, MPI_COMM_WORLD);
            // Send LN array.
            MPI_Send(LN, size, MPI_INT, chosen, PRIVILEGE, MPI_COMM_WORLD);
        }
    }

    /// Finish up.
    free(RN);
    free(LN);
    queue_free(Q);
    MPI_Finalize();

    return 0;
}

int critical_section() {
    if (has_privilege) {
        printf("Process %d has entered critical section.\n", rank);
        return ALLOWED;
    } else {
        printf("Process %d is not allowed to enter the critical section.\n", rank);
        return DENIED;
    }
}
