#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ROWS_A 3
#define COLS_A 3
#define ROWS_B 3
#define COLS_B 3

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (size != 2) {
        printf("Error: This program should be run with 2 processes\n");
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {
        // Initialize matrices A and B
        int A[ROWS_A][COLS_A] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
        int B[ROWS_B][COLS_B] = {{9, 8, 7}, {6, 5, 4}, {3, 2, 1}};

        // Send matrix A to process 1
        MPI_Send(A, ROWS_A * COLS_A, MPI_INT, 1, 0, MPI_COMM_WORLD);

        // Send matrix B to process 1
        MPI_Send(B, ROWS_B * COLS_B, MPI_INT, 1, 0, MPI_COMM_WORLD);

        // Receive the product from process 1
        int C[ROWS_A][COLS_B];
        MPI_Recv(C, ROWS_A * COLS_B, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Print the product matrix
        printf("Product:\n");
        for (int i = 0; i < ROWS_A; i++) {
            for (int j = 0; j < COLS_B; j++) {
                printf("%d ", C[i][j]);
            }
            printf("\n");
        }
    }
    else if (rank == 1) {
        // Receive matrix A from process 0
        int A[ROWS_A][COLS_A];
        MPI_Recv(A, ROWS_A * COLS_A, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Receive matrix B from process 0
        int B[ROWS_B][COLS_B];
        MPI_Recv(B, ROWS_B * COLS_B, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Compute the product matrix
        int C[ROWS_A][COLS_B];
        for (int i = 0; i < ROWS_A; i++) {
            for (int j = 0; j < COLS_B; j++) {
                C[i][j] = 0;
                for (int k = 0; k < COLS_A; k++) {
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        }

        // Send the product matrix to process 0
        MPI_Send(C, ROWS_A * COLS_B, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
