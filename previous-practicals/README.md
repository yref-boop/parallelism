# parallelism
repository for parallelism practicals, it has 3 different folders each implementing some SPMD parallel implementation, using C with MPI

### p1 - montecarlo's method 
on the first assignment, we are given a sequential code that implements an approximation of pi by the montecarlo method and are asked to parallelize it

### p2 - montecarlo's method (collectives)
on the second assignment, we are asked to improve the code of the first assignment in two different ways:
- change all standard mpi operations (send/recv) to collective operations
- implement a new collective operation, using the same send/recv operations as in the implementation without collectives 
then implement the collective following a binomial tree approach and call it MPI_BinomialCollective

### p3 - matrix-vector multiplication
on the third assignment, we are given a sequetial code that implements the multiplication of a matrix by a vector, we are asked to parallelize the code by dividing the matrix between processes, by rows and collecting the results