# parallelism

repository for parallel practicals development, it has 3 different folders, each implementing some SPMD parallell implementation, using C & MPI

### p1-2 counting instances of a character in a string
- we are given a sequential code & asked to paralellize it such that:
    - I/O is managed by thread #1
    - n & L is distributed to all threads
    - each thread will scan the string with a step == to the number of processes
    - information between threads must be managed with `Send` / `Recv`
- once that implementation is finished, we are asked to use collective operations to manage data between threads:
    - using standard MPI collectives
    - reimplementing a broadcast function `MPI_BinomialCollective` following a binomial tree approach
    - reimplementing a reduce function `MPI_FlattreeCollective`, assuming that the operation performed is always an addition

### previous practicals
a set of practices from the previous year:

#### p1 - montecarlo's method
on the first assignment, we are given a sequential code that implements an approximation of pi by the montecarlo method and are asked to parallelize it

#### p2 - montecarlo's method (collectives)
on the second assignment, we are asked to improve the code of the first assignment in two different ways:
- change all standard mpi operations (send/recv) to collective operations
- implement a new collective operation, using the same send/recv operations as in the implementation without collectives
then implement the collective following a binomial tree approach and call it MPI_BinomialCollective

#### p3 - matrix-vector multiplication
on the third assignment, we are given a sequetial code that implements the multiplication of a matrix by a vector, we are asked to parallelize the code by dividing the matrix between processes, by rows and collecting the results
