#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char** argv) {
    int rank, size;
    int nrows, ncols;
    int* a = NULL;
    int* b = NULL;
    int* c = NULL;
    int* row = NULL;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (rank == 0) {
        printf("Enter the number of rows and columns for matrix A:\n");
        scanf("%d %d", &nrows, &ncols);
        
        a = (int*) malloc(nrows * ncols * sizeof(int));
        
        printf("Enter the values of matrix A:\n");
        for (int i = 0; i < nrows; i++) {
            for (int j = 0; j < ncols; j++) {
                scanf("%d", &a[i*ncols+j]);
            }
        }
        
        printf("Enter the number of rows and columns for matrix B:\n");
        scanf("%d %d", &nrows, &ncols);
        
        b = (int*) malloc(nrows * ncols * sizeof(int));
        
        printf("Enter the values of matrix B:\n");
        for (int i = 0; i < nrows; i++) {
            for (int j = 0; j < ncols; j++) {
                scanf("%d", &b[i*ncols+j]);
            }
        }
    }
    
    MPI_Bcast(&nrows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&ncols, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    int chunk_size = nrows / size;
    int chunk_start = rank * chunk_size;
    int chunk_end = chunk_start + chunk_size;
    
    if (rank == size - 1) {
        chunk_end = nrows;
    }
    
    row = (int*) malloc(ncols * sizeof(int));
    
    int* recv_buffer = (int*) malloc(ncols * sizeof(int));
    
    c = (int*) malloc(nrows * ncols * sizeof(int));
    
    for (int i = chunk_start; i < chunk_end; i++) {
        for (int j = 0; j < ncols; j++) {
            row[j] = 0;
            for (int k = 0; k < nrows; k++) {
                row[j] += a[i*ncols+k] * b[k*ncols+j];
            }
        }
        MPI_Reduce(row, recv_buffer, ncols, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        if (rank == 0) {
            for (int j = 0; j < ncols; j++) {
                c[i*ncols+j] = recv_buffer[j];
            }
        }
    }
    
    MPI_Gather(c + chunk_start*ncols, chunk_size*ncols, MPI_INT,
               c, chunk_size*ncols, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        printf("The result of matrix multiplication is:\n");
        for (int i = 0; i < nrows; i++) {
            for (int j = 0; j < ncols; j++) {
                printf("%d ", c[i*ncols+j]);
            }
            printf("\n");
        }
    }
    
    free(a);
    free(b);
    free(c);
    free(row);

    MPI_Finalize();
    return 0;

}
