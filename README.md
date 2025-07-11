# Optimizing the Needleman-Wunsch
The NW algorithm is a global pairwise sequence alignment. The purpose of this repo is a parallel implementation of it that processes rows and columns in parallel instead of working on either separately, which results in cutting the time in half. You can find two implementations, one in Python and another more efficient/powerful implementation in C.<br>
For more information contact me at haj26@mail.aub.edu
## Reading Samples
Using the `read_genome` function available in the read_seq file, this function assumes that only the genome strings are included in the files, future implementations will include processing proper FASTA format.
