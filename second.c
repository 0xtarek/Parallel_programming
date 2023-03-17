#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ROWS 3
#define COLS 3

int main(int argc, char *argv[]) {
    int rank, size, i, j, k;
    int **matrix_a, **matrix_b, **matrix_c;
    int *send_buffer, *recv_buffer;
    MPI_Status status;

    // Initialize MPI environment
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Allocate memory for matrices
    matrix_a = (int **) malloc(ROWS * sizeof(int *));
    matrix_b = (int **) malloc(COLS * sizeof(int *));
    matrix_c = (int **) malloc(ROWS * sizeof(int *));
    for (i = 0; i < ROWS; i++) {
        matrix_a[i] = (int *) malloc(COLS * sizeof(int));
        matrix_c[i] = (int *) malloc(COLS * sizeof(int));
    }
    for (i = 0; i < COLS; i++) {
        matrix_b[i] = (int *) malloc(ROWS * sizeof(int));
    }

    // Initialize matrices
    if (rank == 0) {
        for (i = 0; i < ROWS; i++) {
            for (j = 0; j < COLS; j++) {
                matrix_a[i][j] = i + j;
            }
        }
        for (i = 0; i < COLS; i++) {
            for (j = 0; j < ROWS; j++) {
                matrix_b[i][j] = i * j;
            }
        }
    }

    // Scatter matrix b to all processes
    send_buffer = (int *) malloc(ROWS * sizeof(int));
    recv_buffer = (int *) malloc(ROWS * sizeof(int));
    MPI_Scatter(matrix_b, ROWS, MPI_INT, send_buffer, ROWS, MPI_INT, 0, MPI_COMM_WORLD);

    // Compute local portion of matrix c
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            matrix_c[i][j] = 0;
            for (k = 0; k < ROWS; k++) {
                matrix_c[i][j] += matrix_a[i][k] * send_buffer[k];
            }
        }
    }

    // Gather local portions of matrix c to process 0
    MPI_Gatherv(matrix_c, ROWS*COLS/size, MPI_INT, matrix_c, ROWS*COLS/size, MPI_INT, 0, MPI_COMM_WORLD);

    // Print result
    if (rank == 0) {
        printf("Matrix A:\n");
        for (i = 0; i < ROWS; i++) {
            for (j = 0; j < COLS; j++) {
                printf("%d ", matrix_a[i][j]);
            }
            printf("\n");
        }
        printf("Matrix B:\n");
        for (i = 0; i < COLS; i++) {
            for (j = 0; j < ROWS; j++) {
                printf("%d ", matrix_b[i][j]);
            }
            printf("\n");
        }
        printf("Matrix C:\n");
        for (i = 0; i < ROWS; i++) {
            for (j = 0; j < COLS; j++){
                printf("%d ", matrix_c[i][j]);
            }
        }
    }
}
