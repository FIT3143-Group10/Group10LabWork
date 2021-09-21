/* Gets the neighbors in a cartesian communicator
 * Orginally written by Mary Thomas
 * - Updated Mar, 2015
 * Link:
 * https://edoras.sdsu.edu/~mthomas/sp17.605/lectures/MPICart-Comms-and-Topos.pdf
 * Minor modifications to fix bugs and to revise print output
 */
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SHIFT_ROW 0
#define SHIFT_COL 1
#define DISP 1
int main(int argc, char *argv[]) {
    int primes[] = { 2, 3, 5, 7, 9 };
    const int NUM_PRIMES = 5;
    
    int ndims = 2, size, my_rank, reorder, my_cart_rank, ierr;
    int nrows, ncols;
    int nbr_i_lo, nbr_i_hi;
    int nbr_j_lo, nbr_j_hi;

    MPI_Comm comm2D;

    int dims[ndims], coord[ndims];
    int wrap_around[ndims];

    /* start up initial MPI environment */
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    srand(my_rank);

    /* process command line arguments*/
    if (argc == 3) {
        nrows = atoi(argv[1]);
        ncols = atoi(argv[2]);
        dims[0] = nrows; /* number of rows */
        dims[1] = ncols; /* number of columns */
        if ((nrows * ncols) != size) {
            if (my_rank == 0)
                printf("ERROR: nrows*ncols)=%d * %d = % d != % d\n", nrows, ncols, nrows * ncols, size);
            MPI_Finalize();
            return 0;
        }
    } else {
        nrows = ncols = (int)sqrt(size);
        dims[0] = dims[1] = 0;
    }
    /************************************************************
       */
    /* create cartesian topology for processes */
    /************************************************************
       */
    MPI_Dims_create(size, ndims, dims);
    if (my_rank == 0) {
        printf("Root Rank: %d. Comm Size: %d: Grid Dimension = [ % d x % d]\n", my_rank, size, dims[0], dims[1]);
    }
    /* create cartesian mapping */
    wrap_around[0] = wrap_around[1] = 0;
    /* periodic shift is .false. */
    reorder = 1;
    ierr = 0;
    ierr = MPI_Cart_create(MPI_COMM_WORLD, ndims, dims,
                           wrap_around, reorder, &comm2D);
    if (ierr != 0) {
        printf("ERROR[%d] creating CART\n", ierr);
    }
    /* find my coordinates in the cartesian communicator group */
    MPI_Cart_coords(comm2D, my_rank, ndims, coord);
    /* use my cartesian coordinates to find my rank in cartesian
          group*/
    MPI_Cart_rank(comm2D, coord, &my_cart_rank);
    /* get my neighbors; axis is coordinate dimension of shift */
    /* axis=0 ==> shift along the rows: P[my_row-1]: P[me] :
          P[my_row+1] */
    /* axis=1 ==> shift along the columns P[my_col-1]: P[me] :
          P[my_col+1] */
    MPI_Cart_shift(comm2D, SHIFT_ROW, DISP, &nbr_i_lo, &nbr_i_hi);
    MPI_Cart_shift(comm2D, SHIFT_COL, DISP, &nbr_j_lo, &nbr_j_hi);

    // Open output file.
    char pOutputFileName[20];
    snprintf(pOutputFileName, 20, "process_%d.txt", my_rank);
    FILE* output;
    output = fopen(pOutputFileName, "w");
    
    // Generate prime number.
    for (int _ = 0; _ < 10; _++) {
        int p = primes[rand() % NUM_PRIMES];
        
        // Send and receive from neightbours.
        MPI_Request send_req[4];
        MPI_Isend(&p, 1, MPI_INT, nbr_i_lo, 0, comm2D, &send_req[0]);
        MPI_Isend(&p, 1, MPI_INT, nbr_i_hi, 0, comm2D, &send_req[1]);
        MPI_Isend(&p, 1, MPI_INT, nbr_j_lo, 0, comm2D, &send_req[2]);
        MPI_Isend(&p, 1, MPI_INT, nbr_j_hi, 0, comm2D, &send_req[3]);
        
        int up = 0, down = 0, left = 0, right = 0;
        MPI_Request recv_req[4];
        MPI_Irecv(&up, 1, MPI_INT, nbr_i_lo, 0, comm2D, &recv_req[0]);
        MPI_Irecv(&down, 1, MPI_INT, nbr_i_hi, 0, comm2D, &recv_req[1]);
        MPI_Irecv(&left, 1, MPI_INT, nbr_j_lo, 0, comm2D, &recv_req[2]);
        MPI_Irecv(&right, 1, MPI_INT, nbr_j_hi, 0, comm2D, &recv_req[3]);
        MPI_Waitall(4, recv_req, MPI_STATUSES_IGNORE);

        if (p == up) {
            fprintf(output, "Received matching %d from process %d\n", p, nbr_i_lo);
        }
        if (p == down) {
            fprintf(output, "Received matching %d from process %d\n", p, nbr_i_hi);
        }
        if (p == left) {
            fprintf(output, "Received matching %d from process %d\n", p, nbr_j_lo);
        }
        if (p == right) {
            fprintf(output, "Received matching %d from process %d\n", p, nbr_j_hi);
        }
    }
    
    fclose(output);
    fflush(stdout);
    MPI_Comm_free(&comm2D);
    MPI_Finalize();
    return 0;
}
