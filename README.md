# parallelism
repository for parallelism practicals

#### p1
on the first assignment, we are given a sequential code that implements an approximation of pi by the montecarlo method and are asked to parallelize it

#### p2
on the second assignment, we are asked to improve the code of the first assignment in two different ways:
- change all standard mpi operations (send/recv) to collective operations
- implement a new collective operation, using the same send/recv operations as in the implementation without collectives 
then implement the collective following a binomial tree approach and call it MPI_BinomialCollective
