#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int i, done = 0, n, count;
    double PI25DT = 3.141592653589793238462643;
    double pi, x, y, z;

	// auxiliar variables
	int k;
	double pirec;
	int numprocs, rank;

	// needed mpi variables
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    while (!done)
    {
		// given the first process, make the sends, print & scan
		if (rank == 0){
        	printf("Enter the number of points: (0 quits) \n");
        	scanf("%d",&n);
			// send a message with buffer n (int) for each process (k) using global communicator
			for (k = 1; k < numprocs; k++)
				MPI_Send(&n, 1, MPI_INT, k, 0, MPI_COMM_WORLD);
		} else
			// each process is set to recieve a message sent 
			MPI_Recv(&n, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    
        if (n == 0) break;

        count = 0;  

		// given loop, modified to be relative to numprocs
		for (i = rank + 1; i <= n; i += numprocs) {
            // get the random numbers between 0 and 1
	    	x = ((double) rand()) / ((double) RAND_MAX);
	    	y = ((double) rand()) / ((double) RAND_MAX);

	    	// calculate the square root of the squares
	    	z = sqrt((x*x)+(y*y));

	    	// check whether z is within the circle
	    	if(z <= 1.0)
                count++;
        }
		pi = ((double) count/(double) n)*4.0;

		if (rank > 0)
			// any process with rank != 0 sends its value of pi 
			MPI_Send(&pi, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
		else {
			// process with rank 0 recieves the value of pi from any source
			for (k = 1; k < numprocs; k++){
				MPI_Recv(&pirec, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				pi += pirec;
			}
			// and it prints the current value and error
			printf("pi is approx. %.16f, Error is %.16f\n", pi, fabs(pi - PI25DT));
		}
	}
	// finalize all proceses
	MPI_Finalize();
}

