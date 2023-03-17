#include <stdio.h>
#include <mpi.h>

#define ARRAY_SIZE 100

int main(int argc, char** argv) {
    int rank, size;
    int array[ARRAY_SIZE];
    int max = 0;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (rank == 0) {
        printf("Enter %d numbers:\n", ARRAY_SIZE);
        for (int i = 0; i < ARRAY_SIZE; i++) {
            scanf("%d", &array[i]);
        }
        
        for (int dest = 1; dest < size; dest++) {
            MPI_Send(array, ARRAY_SIZE, MPI_INT, dest, 0, MPI_COMM_WORLD);
        }
    } else {
        MPI_Recv(array, ARRAY_SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    
    int chunk_size = ARRAY_SIZE / size;
    int chunk_start = rank * chunk_size;
    int chunk_end = chunk_start + chunk_size;
    
    if (rank == size - 1) {
        chunk_end = ARRAY_SIZE;
    }
    
    for (int i = chunk_start; i < chunk_end; i++) {
        if (array[i] > max) {
            max = array[i];
        }
    }
    
    if (rank != 0) {
        MPI_Send(&max, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    } else {
        for (int source = 1; source < size; source++) {
            int local_max;
            MPI_Recv(&local_max, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (local_max > max) {
                max = local_max;
            }
        }
        printf("The maximum number is %d\n", max);
    }
    
    MPI_Finalize();
    
    return 0;
}
