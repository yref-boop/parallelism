#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int MPI_BinomialBcast (void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm) {
    int error, k;
    int processes, rank;
    int receiver, sender;

    //MPI variables
    MPI_Status status;
    MPI_Comm_size (MPI_COMM_WORLD, &processes);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);

    for (k = 1; k <= (ceil (log2 (processes))); k++) {

        // rank < 2^{k-1} sends messages
        if (rank < pow (2, k-1)) {
            receiver = rank + pow (2, k-1);
            if (receiver < processes) {
                error = MPI_Send (buffer, count, datatype, receiver, 0, comm);
                if (error != MPI_SUCCESS)
                    return error;
            }
        }
        // rank >= 2^{k-1} receives messages
        else {
            if (rank < pow (2, k)) {
                sender = rank - pow (2, k-1);
                error = MPI_Recv (buffer, count, datatype, sender, 0, comm, &status);
                if (error != MPI_SUCCESS)
                    return error;
            }
        }
    }
    return MPI_SUCCESS;
}

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
        printf ("Incorrect argument number\nsyntaxis should follow: program n L\n  program (program name)\n  n (string size)\n  L (character to look for) (A, C, G o T)\n");
        exit(1);
    }
 
    // initial values
    int n, count = 0;
    char *string;
    char L;

    // auxiliar variables
    int i, j, k;
    int recv_count;
    int processes, rank;

    // needed mpi variables
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    MPI_Comm_size (MPI_COMM_WORLD, &processes);


    if (!rank) {
        // get input values on first thread
        n = atoi (argv[1]);
        L = *argv[2];
    }

    // share input values with other threads
    MPI_BinomialBcast (&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_BinomialBcast (&L, 1, MPI_CHAR, 0, MPI_COMM_WORLD);

    // initialize string
    string = (char *) malloc (n*sizeof (char));
    initialize_string (string, n);

    // count occurences in this thread
    for (k = rank; k < n; k += processes)
        if (string[k] == L)
            count++;

    // free string memory
    free (string);

    // reduce count as its sum with each recv_count
    MPI_Reduce (&count, &recv_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // print the results
    if (!rank)
        printf ("character %c appears %d times\n", L, recv_count);

    // finalize all processes
    MPI_Finalize();
}
