#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>

#define DEBUG 0

#define N 49

// expected result -> 14 20 26 32

int main(int argc, char *argv[] ) {

	int i, j, rows, local_msecs; 
	int *total_msecs;
	float *local_matrix, *local_result;
	float matrix[N][N];
	float vector[N];
	float *result;
	struct timeval  tv1, tv2;

	// mpi variables
	int numprocs, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  	// initialize matrix and vector 
	if (rank == 0){
		for(i=0;i<N;i++) {
			vector[i] = i;
    		for(j=0;j<N;j++) 
				matrix[i][j] = i+j;
		}
	}

	// every process needs all values of the vector
	MPI_Bcast (vector, N, MPI_FLOAT, 0, MPI_COMM_WORLD);

	// number of rows / process
	rows = (N + numprocs - 1) / numprocs;

	// local submatrices
	local_matrix = (float *)malloc(sizeof(float)*rows*N);
	local_result = (float *)malloc(sizeof(float)*rows*N);

	// scatter matrix data
	MPI_Scatter (matrix, rows*N, MPI_FLOAT, local_matrix, rows*N, MPI_FLOAT, 0, MPI_COMM_WORLD);

	gettimeofday(&tv1, NULL);
	// calculate result
  	for(i=0;i<rows;i++) {
		local_result[i]=0;
		for(j=0;j<N;j++)
			local_result[i] += local_matrix[N*i+j]*vector[j];
	}
	gettimeofday(&tv2, NULL);

	// get value of time
	local_msecs = (tv2.tv_usec - tv1.tv_usec) + 1000000 * (tv2.tv_sec - tv1.tv_sec);

	if (rank == 0){
		result = (float*) malloc(sizeof(float)*N*numprocs*rows);
		total_msecs = (int *) malloc(sizeof(int)*numprocs);
	}

	// gather data
	MPI_Gather (local_result, rows, MPI_FLOAT, result, rows, MPI_FLOAT, 0, MPI_COMM_WORLD);
	MPI_Gather (&local_msecs, 1, MPI_INT, total_msecs, 1, MPI_INT, 0, MPI_COMM_WORLD);

  	// display result
	if (rank == 0) {
		if (DEBUG)
			for(i=0;i<N;i++) printf(" %f \t ", result[i]);
		else
			for (i=0;i<numprocs;i++)
			printf ("time (seconds) of process %d= %lf\n", rank, (double) total_msecs[i]/1E6);
	}

	// finalize all threads
	MPI_Finalize();
	return 0;
}
