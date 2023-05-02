#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

#define DEBUG 1

/* translation of the DNA bases
   A -> 0
   C -> 1
   G -> 2
   T -> 3
   N -> 4*/

#define M  8    // number of sequences
#define N  4    // number of bases per sequence

unsigned int g_seed = 0;

int fast_rand (void) {
    g_seed = (214013*g_seed+2531011);
    return (g_seed>>16) % 5;
}

// The distance between two bases
int base_distance (int base1, int base2){

    if((base1 == 4) || (base2 == 4))
        return 3;
    if(base1 == base2)
        return 0;
    if((base1 == 0) && (base2 == 3))
        return 1;
    if((base2 == 0) && (base1 == 3))
        return 1;
    if((base1 == 1) && (base2 == 2))
        return 1;
    if((base2 == 2) && (base1 == 1))
        return 1;

    return 2;
}

int main (int argc, char *argv[] ) {

    int i, j, n;
    int *data1, *data2;
    int *result;
    struct timeval tv1, tv2, tv3, tv4;

    int block_size;
    int *partial_data1, *partial_data2;
    int *partial_result;
    int rank, processes;

    int local_block_size;

    // special tags for times
    int communication_tag = 111;
    int processing_tag = 59;

    MPI_Init(&argc, &argv);

    MPI_Status status;
    MPI_Comm_size (MPI_COMM_WORLD, &processes);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);

    // calculate block_size
    block_size = M / processes + (M % processes ? 1 : 0);

    // initialize matrices on first thread
    if (!rank) {

        printf ("total processes: %d\nmatrix size %d x %d\n", processes, M, N);

        data1  = (int *) malloc (M * N * sizeof (int));
        data2  = (int *) malloc (M * N * sizeof (int));
        result = (int *) malloc (M * sizeof (int));

        // initialize matrices randomly with 20% gap proportion
        for (i = 0; i < M; i++)
            for (j = 0; j < N; j++) {
                data1 [i*N+j] = fast_rand();
                data2 [i*N+j] = fast_rand();
            }
    }

    // allocate partial vectors
    partial_data1 = (int *) malloc (block_size * N * sizeof (int));
    partial_data2 = (int *) malloc (block_size * N * sizeof (int));
    partial_result = (int *) malloc (block_size * sizeof (int));

    gettimeofday (&tv1, NULL);

    // scatter data from process #0
    MPI_Scatter (data1, block_size * N, MPI_INT, partial_data1, block_size * N, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter (data2, block_size * N, MPI_INT, partial_data2, block_size * N, MPI_INT, 0, MPI_COMM_WORLD);

    gettimeofday (&tv2, NULL);

    // redefinition of block_size on last process
    if (rank == processes - 1)
        local_block_size = M - block_size * (processes-1);
    else
        local_block_size = block_size;

    // calculate partial result on all processes
    for (i = 0; i < local_block_size; i++) {
        partial_result[i]=0;
        for (j = 0; j < N; j++)
            partial_result[i] += base_distance (partial_data1[i*N+j], partial_data2[i*N+j]);
    }

    gettimeofday (&tv3, NULL);

    // gather results
    MPI_Gather (partial_result, block_size, MPI_INT, result, block_size, MPI_INT, 0, MPI_COMM_WORLD);

    gettimeofday (&tv4, NULL);

    int communication_time = (tv2.tv_usec - tv1.tv_usec) + 1000000 * (tv2.tv_sec - tv1.tv_sec) + ((tv4.tv_usec -tv3.tv_usec) + 1000000 * (tv4.tv_sec - tv3.tv_sec));
    int processing_time = (tv3.tv_usec - tv2.tv_usec) + 1000000 * (tv3.tv_sec - tv2.tv_sec);

    // display results
    if (DEBUG == 1 && (!rank)) {
        int checksum = 0;
        for (i = 0; i < M; i++)
            checksum += result[i];
        printf("Checksum: %d\n ", checksum);
    }
    else if (DEBUG == 2 && (!rank))
        for (i = 0; i < M; i++)
            printf (" %d \t ",result[i]);

    if (!rank) {

        // print communication & processing times of root process
        printf ("root process communication time (seconds) = %lf\n", (double) communication_time/1E6);
        printf ("root process processing time (seconds) = %lf\n", (double) processing_time/1E6);

        // receive & print communication & processing times from all processes
        for (i = 1; i < processes; i++) {
            MPI_Recv (&communication_time, 1, MPI_INT, i, communication_tag, MPI_COMM_WORLD, &status);
            printf ("process #%d communication time (seconds) = %lf\n", i, (double) communication_time/1E6);
            MPI_Recv (&processing_time, 1, MPI_INT, i, processing_tag, MPI_COMM_WORLD, &status);
            printf ("process #%d processing time (seconds) = %lf\n", i, (double) processing_time/1E6);

        }
    } else {
        MPI_Send (&communication_time, 1, MPI_INT, 0, communication_tag, MPI_COMM_WORLD);
        MPI_Send (&processing_time, 1, MPI_INT, 0, processing_tag, MPI_COMM_WORLD);
    }


    if (!rank) {
        free (data1);
        free (data2);
        free (result);
    }

    free (partial_data1);
    free (partial_data2);
    free (partial_result);

    MPI_Finalize();

    return 0;
}
