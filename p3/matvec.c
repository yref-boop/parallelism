#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>

#define DEBUG 0

#define N 7

int main(int argc, char *argv[] ) {

	// needed variables
	int i, j, local_msecs;
	int *count;
	int *displace;
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

	// initialization of displacement and counting values:
	count = malloc(sizeof(int)*numprocs);
	displace = malloc(sizeof(int)*numprocs);

	// calculate conts and displacements
	int remainder = N % numprocs;
	int division = N / numprocs;
	int sum = 0;

	for (int i = 0; i < numprocs; i++){
		count [i] = N * division;
		if (remainder) {
			count[i] += N;
			remainder--;
		}
		displace [i] = sum;
		sum += count[i];
	}
	
	// local submatrices
	local_matrix = (float *)malloc(sizeof(float)*count[rank]*N);
	local_result = (float *)malloc(sizeof(float)*count[rank]*N);

	// scatter matrix data
	MPI_Scatterv (matrix, count, displace, MPI_FLOAT, local_matrix, count[rank], MPI_FLOAT, 0, MPI_COMM_WORLD);

	gettimeofday(&tv1, NULL);
	// calculate result	
  	for(i=0;i<count[rank];i++){
		local_result[i]=0;
		for(j=0;j<N;j++)
			local_result[i] += local_matrix[N*i+j]*vector[j];
	}
	gettimeofday(&tv2, NULL);

	// get value of time
	local_msecs = (tv2.tv_usec - tv1.tv_usec) + 1000000 * (tv2.tv_sec - tv1.tv_sec);

	// prepare use of count and displace for results
	for (int i = 0; i < numprocs; i++){
		count[i] /= N;
		displace[i] /= N;
	}

	// definition of the size of the recieve ends of the gathers
	if (rank == 0){
		result = (float*) malloc(sizeof(float)*numprocs*count[rank]);
		total_msecs = (int *) malloc(sizeof(int)*numprocs*count[rank]);
	}

	// gather the results and the time needed to obtain
	MPI_Gatherv (local_result, count[rank], MPI_FLOAT, result, count, displace, MPI_FLOAT, 0, MPI_COMM_WORLD);
	MPI_Gatherv (&local_msecs, count[rank], MPI_INT, total_msecs, count, displace, MPI_INT, 0, MPI_COMM_WORLD);

	// wait for all processes
	MPI_Barrier(MPI_COMM_WORLD);

  	// display result
	if (rank == 0) {
		if (DEBUG)
			for(i=0;i<N;i++) printf(" %f \t ", result[i]);
		else
			// total_msecs stores orderly time needed by each process
			for (i=0;i<numprocs;i++)
			printf ("time (seconds) of process %d= %lf\n", i, (double) total_msecs[i]/1E6);
	}

	// finalize all threads
	MPI_Finalize();

	// free memory
	free(local_matrix);
	free(local_result);
	free(count);
	free(displace);

	return 0;
}
