#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include "collectives.c"

int main(int argc, char *argv[]){
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

    while (!done) {
		// given the first process, make the print and scan
		if (rank == 0) {
        	printf("Enter the number of points: (0 quits) \n");
        	scanf("%d",&n);
		}
		
		// change individual sends to a broadcast
		MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

		// alternatively use:
		// MPI_FlattreeBcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
		// MPI_BinomialBcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

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

		// reduce pi and pirec as their sum (store at pirec) 
		MPI_Reduce(&pi, &pirec, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
		
		if (rank == 0)
			printf("pi is approx. %.16f, Error is %.16f\n", pirec, fabs(pirec - PI25DT));
	}
	// finalize all proceses
	MPI_Finalize();
}
