# Optimizing the Needleman-Wunsch
The NW algorithm is a global pairwise sequence alignment. The purpose of this repo is a parallel implementation of it that processes rows and columns in parallel instead of working on either separately, which results in cutting the time in half. You can find two implementations, one in Python and another more efficient/powerful implementation in C. Both implementations return the optimal score without the path.<br>
For more information contact me at haj26@mail.aub.edu
## Reading Samples
Using the `read_genome` function available in the "read_seq" file, this function assumes that only the genome strings are included in the files, future implementations will include processing proper FASTA format.<br>
## Python version
To run this version, all you have to do is call 
```
global_alignment(match_reward, mismatch_penalty, indel_penalty, seq_1, seq_2)
```
From the "python_align". This function will return the optimal path score in minimal time without consuming resources.
## Preparing environment
Run
```
pip install -r Requirements.txt`
```
This will install the `ctypes` library which allows us to use C code in Python directly after compiling.
## Python-C version
This version will call a function written in C, a much faster implementation using less resources while being 60x faster (average C code O.o)
### Compiling the C function
To compile the code into a usable library from Python, run the below commands in CMD
```
gcc -shared -o c_code.dll alignment_c.c
```
### Preparing the function to be called
```
dll_path = "c_code.dll"
my_lib = ctypes.CDLL(dll_path, winmode=0)
global_alignment = my_lib.global_alignment
global_alignment.argtypes = [ctypes.c_float, ctypes.c_float, ctypes.c_float, ctypes.c_char_p, ctypes.c_char_p]
global_alignment.restype = ctypes.c_float
```
Let me explain what's going on:
1. Using the ctypes library to read and fetch the function.
2. Getting the exact function `global_alignment` from within this file.
3. Define the input arguments and data types expected for C.
4. Define the returned output and data type for C as well.
We're now all set ! All we have to do is call the function.
### Running the aligner
```
result = global_alignment(match_reward, mismatch_penalty, indel_penalty, seq_1, seq_2)
```
## Next Steps
1. Refine the code.
2. Implement an idea that further improves parallelism.
3. Implement a way to save the best path.
