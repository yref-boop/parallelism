#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>

#define DEBUG 1

#define N 10

void populate(int *count, int *displace, int numprocs) {
	int remainder = N % numprocs;
	int division = N / numprocs;
	int sum = 0;

	// we divide the remainder evengly amongst the processes
	for (int i = 0; i < numprocs; i++){
		// initialize common values for all processes 
		count [i] = N * division;
		// divide the remainder 1 to each process
		if (remainder) {
			count[i] += N;
			remainder--;
		}
		displace [i] = sum;
		sum += count[i];
	}
}

int main(int argc, char *argv[] ) {

	// variable definition
	int i, j, local_msecs, local_commtime;
	int *count, *displace, *total_msecs;
	float *local_matrix, *local_result, *result;
	float *matrix, *vector;
	struct timeval tv1, tv2;
	struct timeval ct1, ct2;

	// mpi variables
	int numprocs, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	vector = (float *)malloc(sizeof(float)*N);

  	// initialize matrix and vector 
	if (!rank) {
		matrix = (float *)malloc(sizeof(float)*N*N);
		for(i=0;i<N;i++) {
			vector[i] = i;
    		for(j=0;j<N;j++) 
				matrix[N*i+j] = i+j;
		}
	}

	// every process needs all values of the vector
	MPI_Bcast (vector, N, MPI_FLOAT, 0, MPI_COMM_WORLD);

	count = malloc(sizeof(int)*numprocs);
	displace = malloc(sizeof(int)*numprocs);

	// calculate counts and displacements
	populate(count, displace, numprocs);
	
	local_matrix = (float *)malloc(sizeof(float)*count[rank]*N);
	local_result = (float *)malloc(sizeof(float)*count[rank]);


	gettimeofday(&ct1, NULL);
	// size of each chunk may vary (calculated previously on populate) 
	MPI_Scatterv (matrix, count, displace, MPI_FLOAT, local_matrix, count[rank], MPI_FLOAT, 0, MPI_COMM_WORLD);
	gettimeofday(&ct2,NULL);
	local_commtime = (ct2.tv_usec - ct1.tv_usec) + 1000000 * (ct2.tv_sec - ct1.tv_sec);

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
	if (!rank){
		result = (float*) malloc(sizeof(float)*numprocs*count[rank]);
		total_msecs = (int *) malloc(sizeof(int)*numprocs*count[rank]);
	}

	gettimeofday(&ct1, NULL);
	// gather the results and the time needed to obtain
	MPI_Gatherv (local_result, count[rank], MPI_FLOAT, result, count, displace, MPI_FLOAT, 0, MPI_COMM_WORLD);
	MPI_Gatherv (&local_msecs, count[rank], MPI_INT, total_msecs, count, displace, MPI_INT, 0, MPI_COMM_WORLD);
	gettimeofday(&ct2,NULL);
	local_commtime += (ct2.tv_usec - ct1.tv_usec) + 1000000 * (ct2.tv_sec - ct1.tv_sec);

  	// display result
	if (!rank) {
		if (DEBUG)
			for(i=0;i<N;i++) printf(" %f \t ", result[i]);
		else
			// total_msecs stores orderly time needed by each process
			for (i=0;i<numprocs;i++)
			printf ("computation time (seconds) of process %d= %lf\n", i, (double) total_msecs[i]/1E6);
	}

	// wait for all processes
	MPI_Barrier(MPI_COMM_WORLD);

	if (!DEBUG)
		printf ("communication time (seconds) of process %d = %lf\n", rank, (double) local_commtime/1E6);

	// free from process 0
	if (!rank) {
		free(result);
		free(total_msecs);
	}

	// free from all processes 
	free(local_matrix);
	free(local_result);
	free(count);
	free(displace);

	// finalize all threads
	MPI_Finalize();

	return 0;
}
