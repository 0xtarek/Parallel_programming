#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define SIZE 4

int main(int argc, char *argv[]) {
    int rank, size, i, j, k;
    int **A, **B, **C;
    int *buf_A, *buf_B, *buf_C;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Allocate memory dynamically for matrices
    A = (int **) malloc(sizeof(int *) * SIZE);
    B = (int **) malloc(sizeof(int *) * SIZE);
    C = (int **) malloc(sizeof(int *) * SIZE);
    for (i = 0; i < SIZE; i++) {
        A[i] = (int *) malloc(sizeof(int) * SIZE);
        B[i] = (int *) malloc(sizeof(int) * SIZE);
        C[i] = (int *) malloc(sizeof(int) * SIZE);
    }

    // Initialize matrices A and B
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            A[i][j] = i + j;
            B[i][j] = i * j;
        }
    }

    // Allocate buffers for sending and receiving matrices
    buf_A = (int *) malloc(sizeof(int) * SIZE * SIZE);
    buf_B = (int *) malloc(sizeof(int) * SIZE * SIZE);
    buf_C = (int *) malloc(sizeof(int) * SIZE * SIZE);

    // Scatter matrix A to all processes
    MPI_Scatter(A[0], SIZE*SIZE/size, MPI_INT, buf_A, SIZE*SIZE/size, MPI_INT, 0, MPI_COMM_WORLD);

    // Broadcast matrix B to all processes
    MPI_Bcast(B[0], SIZE*SIZE, MPI_INT, 0, MPI_COMM_WORLD);

    // Perform matrix multiplication
    for (i = 0; i < SIZE/size; i++) {
        for (j = 0; j < SIZE; j++) {
            C[i][j] = 0;
            for (k = 0; k < SIZE; k++) {
                C[i][j] += buf_A[i*SIZE+k] * B[k][j];
            }
        }
    }

    // Gather all submatrices C back to root process
    MPI_Gather(C[0], SIZE*SIZE/size, MPI_INT, buf_C, SIZE*SIZE/size, MPI_INT, 0, MPI_COMM_WORLD);

    // Print the result
    if (rank == 0) {
        printf("Result:\n");
        for (i = 0; i < SIZE; i++) {
            for (j = 0; j < SIZE; j++) {
                printf("%d ", buf_C[i*SIZE+j]);
            }
            printf("\n");
        }
    }

    // Free allocated memory
    for (i = 0; i < SIZE; i++) {
        free(A[i]);
        free(B[i]);
        free(C[i]);
    }
    free(A);
    free(B);
    free(C);
    free(buf_A);
    free(buf_B);
    free(buf_C);

    MPI_Finalize();
    return 0;
}
