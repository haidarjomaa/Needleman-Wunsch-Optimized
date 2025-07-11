# Optimizing the Needleman-Wunsch
The NW algorithm is a global pairwise sequence alignment. The purpose of this repo is a parallel implementation of it that processes rows and columns in parallel instead of working on either separately, which results in cutting the time in half. You can find two implementations, one in Python and another more efficient/powerful implementation in C. Both implementations return the optimal score without the path.<br>
For more information contact me at haj26@mail.aub.edu
## Reading Samples
Using the `read_genome` function available in the "read_seq" file, this function assumes that only the genome strings are included in the files, future implementations will include processing proper FASTA format.<br>
## Python version
To run this version, all you have to do is call the `global_alignment(match_reward, mismatch_penalty, indel_penalty, seq_1, seq_2)` from the "efficient_pairwise_align". This function will return the optimal path score in minimal time without consuming resources.
## Preparing environment
Run the `pip install -r Requirements.txt`, this will install the `ctypes` library which allows us to use C code in Python directly after compiling.
## Python-C version
