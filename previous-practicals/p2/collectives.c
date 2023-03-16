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

	// process with root rank sends a message for each process (k) using global communicator
    if (rank == root) {
		for (k = 0; k < numprocs; k++)
            if (k != root) {
                error = MPI_Send(buffer, count, datatype, k, 0, comm);
				// check error
                if (error != MPI_SUCCESS) return error;
            }
    } else {
		// other processes recieve each message
        error = MPI_Recv(buffer, count, datatype, root, 0, comm, &status);
		// check error
        if (error != MPI_SUCCESS) return error;
    }
    return MPI_SUCCESS;
}

int MPI_BinomialBcast (void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm) {
	int error, k;
	int numprocs, rank;
	int receiver, sender;

	// mpi variables
	MPI_Status status;
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	    for (k = 1; k <= ceil(log2(numprocs)); k++)
		// if process has rank < 2^{k-1}, sends messages
        if (rank < pow(2, k-1)) {
			// destination's rank is rank + 2^{k-1}
            receiver = rank + pow(2, k-1);
            if (receiver < numprocs) {
				// do / check send, if error, return it
                error = MPI_Send(buffer, count, datatype, receiver, 0, comm);
                if (error != MPI_SUCCESS) return error;
            }
        } else {
			// if process has rank < 2^k is set to recieve messages
            if (rank < pow(2, k)) {
				// source of the message has rank - 2^{k-1}
                sender = rank - pow(2, k-1);
				// do / check recive, if error, return it
                error = MPI_Recv(buffer, count, datatype, sender, 0, comm, &status);
                if (error != MPI_SUCCESS) return error;
            }
        }
    return MPI_SUCCESS;
}
