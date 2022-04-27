#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

#define DEBUG 1

#define N 1024

int main(int argc, char *argv[] ) {

	int i, j, rows;
	float* m,* mlocal,* xlocal;
	float matrix[N][N];
	float vector[N];
	float result[N];
	struct timeval  tv1, tv2;

	// mpi variables
	int numprocs, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);


  	// initialize matrix and vector 
	for(i=0;i<N;i++) {
		vector[i] = i;
    	for(j=0;j<N;j++) 
			matrix[i][j] = i+j;
  	}

	// broadcast matrix and vector values to all processes
	// MPI_Bcast (&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast (vector, N, MPI_FLOAT, 0, MPI_COMM_WORLD);

	// calculate the number of rows / process
	rows = (N + numprocs - 1) / numprocs;

	// calculate size of matrix data 
	if ((rank == 0) && (N % numprocs))
		m = (float *) realloc(matrix, sizeof(float)*rows*N*numprocs);

	// allocate local submatrices
	mlocal = (float*)malloc(sizeof(float)*rows*N);
	xlocal = (float*)malloc(sizeof(float)*rows*N);

	// scatter matrix data
	MPI_Scatter (m, rows*N, MPI_FLOAT, mlocal, rows*N, MPI_FLOAT, 0, MPI_COMM_WORLD);

	// correct number of rows in p-1
	if (rank == numprocs - 1)
		rows = N - rows * (numprocs - 1);

	// get time
	gettimeofday(&tv1, NULL);

	// calculate result 
  	for(i=0;i<N;i++) {
		result[i]=0;
		for(j=0;j<N;j++) 
			result[i] += matrix[i][j]*vector[j];
	}

	// second time measure
	gettimeofday(&tv2, NULL);

	if (rank == 0)
	
	// gather data
	MPI_Gather(xlocal, N*rows, MPI_FLOAT, result, N*rows, MPI_FLOAT, 0, MPI_COMM_WORLD);
    
	int microseconds = (tv2.tv_usec - tv1.tv_usec)+ 1000000 * (tv2.tv_sec - tv1.tv_sec);

  	// display result
	if (rank == 0){
		if (DEBUG)
			for(i=0;i<N;i++) printf(" %f \t ",result[i]);
		else 
			printf ("Time (seconds) = %lf\n", (double) microseconds/1E6);
	}
	// finalize all threads
	MPI_Finalize();
	return 0;
}

