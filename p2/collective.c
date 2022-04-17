#include <stdio.h>
#include <math.h>
#include <mpi.h>

int MPI_FlattreeBcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm) {
	int error, k;
    int numprocs, rank;

	// mpi variables
    MPI_Status status;
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// logic goes here
}

int MPI_BinomialBcast (void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm) {
	int error, k;
	int numprocs, rank;
	int receiver, sender;

	// mpi variables 
	MPI_Status status;
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// logic goes here 
}
