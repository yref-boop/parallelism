#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

void initialize_string (char *string, int n){
    int i;

    for (i=0; i<n/2; i++)
        string[i] = 'A';

    for (i=n/2; i<3*n/4; i++)
        string[i] = 'C';

    for (i=3*n/4; i<9*n/10; i++)
        string[i] = 'G';

    for (i=9*n/10; i<n; i++)
        string[i] = 'T';
}

int main (int argc, char *argv[]) {
    if (argc != 3){
        printf("Incorrect argument number\nsyntaxis should follow: program n L\n  program (program name)\n  n (string size)\n  L (character to look for) (A, C, G o T)\n");
        exit(1);
    }
 
    int i, n, count = 0;
    char *string;
    char L;

    // auxiliar variables
    int rec_n;
    char rec_L;
    int recv_count;
    int numprocs, rank;

    // needed mpi variables
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size (MPI_COMM_WORLD, &numprocs);

    if (rank == 0) {

        // get input values on first thread
        n = atoi (argv[1]);
        L = *argv[2];

        // share input values with other threads
        int i = numprocs;
        while (i-- > 1) {
            MPI_Send (&n, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send (&L, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

    }
    else {
        // get input values from thread #0
        MPI_Recv (&n, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        MPI_Recv (&n, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    }

    for (i=0; i<n; i++){
      if (string[i] == L){
        count++;
      }
    }
 
    if (rank > 0)
        // any process other than 0 sends its number of occurences
        MPI_Send (&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    else {
        // get count from all processes
        int i = numprocs;
        while (i-- > 1) {
            MPI_Recv (&recv_count, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            count += recv_count;
        }
        // print the results
        printf ("character %c appears %d times\n", L, count);
    }
    // finalize all processes
    MPI_Finalize();
}
