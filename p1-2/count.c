#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int MPI_BinomialBcast (void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm) {
    int error, i;
    int processes, rank;
    int receiver, sender;

    //MPI variables
    MPI_Status status;
    MPI_Comm_size (MPI_COMM_WORLD, &processes);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);

    for (i = 1; i <= (ceil (log2 (processes))); i++) {

        // rank < 2^{k-1} sends messages
        if (rank < pow (2, i-1)) {
            receiver = rank + pow (2, i-1);
            if (receiver < processes) {
                printf("im process %d, sending to %d\n", rank, receiver);
                error = MPI_Send (buffer, count, datatype, receiver, 0, comm);
                if (error != MPI_SUCCESS)
                    return error;
            }
        }
        // rank >= 2^{k-1} receives messages
        else {
            if (rank < pow (2, i)) {
                sender = rank - pow (2, i-1);
                error = MPI_Recv (buffer, count, datatype, sender, 0, comm, &status);
                if (error != MPI_SUCCESS)
                    return error;
            }
        }
    }
    return MPI_SUCCESS;
}


int MPI_FlattreeCollective (int *sendbuf, int *recvbuf, int count, MPI_Datatype datatype, int root, MPI_Comm comm) {
    int error, i;
    int processes, rank;

    //MPI variables
    MPI_Status status;
    MPI_Comm_size (MPI_COMM_WORLD, &processes);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);

    // all processes send their data to #0
    if (rank) {
        error = MPI_Send (sendbuf, count, datatype, 0, 0, comm);
        if (error != MPI_SUCCESS)
            return error;
    }

    // process #0 receives & sums all data from others
    if (!rank) {
        i = processes;
        while (i-- > 1) {
            error = MPI_Recv (recvbuf, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if (error != MPI_SUCCESS)
                return error;
            *sendbuf = *sendbuf + *recvbuf;
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
    MPI_FlattreeCollective (&count, &recv_count, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // print the results
    if (!rank)
        printf ("character %c appears %d times\n", L, count);

    // finalize all processes
    MPI_Finalize();
}
