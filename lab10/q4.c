#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    float a_coeff, b_coeff, c_coeff, x1, x2, disc;
    float x1r, x1i, x2r, x2i;

    int fileElementCount = 0;
    int my_rank;
    int p;

    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    switch (my_rank) {
    case 0: {
        FILE *pInfile = fopen("quad.txt", "r");
        
        // Read the number of coefficients and broadcast this to the other processes.
        fscanf(pInfile, "%d\n", &fileElementCount);
        MPI_Bcast(&fileElementCount, 1, MPI_INT, 0, MPI_COMM_WORLD);

        fscanf(pInfile, "%*s %*s %*s\n"); // Skip over the second line
        for (int i = 0; i < fileElementCount; i++) {
            fscanf(pInfile, "%f %f %f", &a_coeff, &b_coeff, &c_coeff);
            disc = pow(b_coeff, 2) - 4 * a_coeff * c_coeff;
            float sendbuf[4] = {a_coeff, b_coeff, c_coeff, disc};
            MPI_Send(sendbuf, 4, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);
        }
        fclose(pInfile);
        break;
    }
    case 1: {
        MPI_Bcast(&fileElementCount, 1, MPI_INT, 0, MPI_COMM_WORLD);
        for (int _ = 0; _ < fileElementCount; _++) {
            float recvbuf[4];
            MPI_Recv(recvbuf, 4, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &status);
            a_coeff = recvbuf[0];
            b_coeff = recvbuf[1];
            c_coeff = recvbuf[2];
            disc = recvbuf[3];

            // Pre-emptively send the discriminant to the next process.
            // We do this so that the next process can work out what
            // to expect in subsequent MPI_Recv calls.
            MPI_Send(&disc, 1, MPI_FLOAT, 2, 0, MPI_COMM_WORLD);
            if (disc >= 0) {
                float r = sqrt(disc);
                x1 = (-b_coeff + r) / (2 * a_coeff);
                x2 = (-b_coeff - r) / (2 * a_coeff);

                float sendbuf[2] = {x1, x2};
                MPI_Send(sendbuf, 2, MPI_FLOAT, 2, 0, MPI_COMM_WORLD);
            } else {
                // Calculate real and imag parts (-ve disc)
                x1r = (-b_coeff) / (2 * a_coeff);
                x1i = -(sqrt(abs(disc)) / (2 * a_coeff));
                x2r = (-b_coeff) / (2 * a_coeff);
                x2i = sqrt(abs(disc)) / (2 * a_coeff);
                float sendbuf[4] = {x1r, x1i, x2r, x2i};
                MPI_Send(sendbuf, 4, MPI_FLOAT, 2, 0, MPI_COMM_WORLD);
            }
        }
        break;
    }
    case 2: {
        MPI_Bcast(&fileElementCount, 1, MPI_INT, 0, MPI_COMM_WORLD);
        FILE *outFile = fopen("roots.txt", "w");
        fprintf(outFile, "%d\n", fileElementCount);
        fprintf(
            outFile,
            "x1        x2        x1_real   x1_img    x2_real   x2_img    \n");
        for (int _ = 0; _ < fileElementCount; _++) {
            // Determine whether we have real or complex roots.
            MPI_Recv(&disc, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD, &status);
            if (disc >= 0) {
                float recvbuf[2];
                MPI_Recv(recvbuf, 2, MPI_FLOAT, 1, 0, MPI_COMM_WORLD, &status);
                fprintf(outFile, "%-+9.2f %-+9.2f\n", recvbuf[0], recvbuf[1]);
            } else {
                float recvbuf[4];
                MPI_Recv(recvbuf, 4, MPI_FLOAT, 1, 0, MPI_COMM_WORLD, &status);
                fprintf(outFile,
                        "                    %-+9.2f %-+9.2f %-+9.2f %-+9.2f",
                        recvbuf[0], recvbuf[1], recvbuf[2], recvbuf[3]);
            }
        }
        fclose(outFile);
        break;
    }
    }
    MPI_Finalize();
    return 0;
}