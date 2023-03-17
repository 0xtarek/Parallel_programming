#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 100 // size of array

int main(int argc, char *argv[]) {
    int rank, size, i, tag = 0;
    int *arr, *subarr;
    int numElements, maxElement = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    numElements = N/size;
    subarr = (int *) malloc(numElements*sizeof(int));

    if(rank == 0) {
        // initialize array
        arr = (int *) malloc(N*sizeof(int));
        for(i = 0; i < N; i++) {
            arr[i] = i;
        }

        // scatter array to all processes
        for(i = 1; i < size; i++) {
            MPI_Send(arr+(i*numElements), numElements, MPI_INT, i, tag, MPI_COMM_WORLD);
        }
        subarr = arr;
    }
    else {
        // receive subarray from process 0
        MPI_Recv(subarr, numElements, MPI_INT, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // find max element in subarray
    for(i = 0; i < numElements; i++) {
        if(subarr[i] > maxElement) {
            maxElement = subarr[i];
        }
    }

    // send max element to process 0
    if(rank != 0) {
        MPI_Send(&maxElement, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
    }
    else {
        // receive max element from all other processes
        int temp;
        for(i = 1; i < size; i++) {
            MPI_Recv(&temp, 1, MPI_INT, i, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if(temp > maxElement) {
                maxElement = temp;
            }
        }

        // print the results
        printf("The maximum element in the array is %d\n", maxElement);
    }

    MPI_Finalize();
    return 0;
}