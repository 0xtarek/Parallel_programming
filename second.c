#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 100 // size of matrix

int main(int argc, char *argv[]) {
    int rank, size, i, j, k, tag = 0;
    double *matrixA, *matrixB, *matrixC;
    double startTime, endTime;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int numElements = N*N/size;
    int sendCount = numElements;
    int recvCount = numElements;

    matrixA = (double *) malloc(numElements*sizeof(double));
    matrixB = (double *) malloc(N*N*sizeof(double));
    matrixC = (double *) calloc(numElements, sizeof(double));

    // initialize matrix B
    if(rank == 0) {
        for(i = 0; i < N; i++) {
            for(j = 0; j < N; j++) {
                matrixB[i*N+j] = i+j;
            }
        }

        // scatter matrix B to all processes
        for(i = 1; i < size; i++) {
            MPI_Send(matrixB, N*N, MPI_DOUBLE, i, tag, MPI_COMM_WORLD);
        }
    }
    else {
        // receive matrix B from process 0
        MPI_Recv(matrixB, N*N, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // initialize matrix A
    for(i = 0; i < numElements; i++) {
        for(j = 0; j < N; j++) {
            matrixA[i*N+j] = (i+(rank*numElements)/N)*j;
        }
    }

    // send matrix A to all other processes
    for(i = 0; i < size; i++) {
        if(i != rank) {
            MPI_Send(matrixA+(i*numElements), sendCount, MPI_DOUBLE, i, tag, MPI_COMM_WORLD);
        }
    }

    // receive matrix A from all other processes
    for(i = 0; i < size; i++) {
        if(i != rank) {
            MPI_Recv(matrixA+(i*numElements), recvCount, MPI_DOUBLE, i, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    // matrix multiplication
    for(i = 0; i < numElements; i++) {
        for(k = 0; k < N; k++) {
            for(j = 0; j < N; j++) {
                matrixC[i] += matrixA[i*N+k] * matrixB[k*N+j];
            }
        }
    }

    // send matrix C to process 0
    if(rank != 0) {
        MPI_Send(matrixC, sendCount, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);
    }
    else {
        // receive matrix C from all other processes
        for(i = 1; i < size; i++) {
            MPI_Recv(matrixC+(i*numElements), recvCount, MPI_DOUBLE, i, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        endTime = MPI_Wtime();

        // print the results
        printf("Matrix C:\n");
        for(i = 0; i < N; i++) {
            for(j = 0; j < N; j++) {
                printf("%f ", matrixC[i*N+j]);
            }
            printf("\n");
        }

    }
}

