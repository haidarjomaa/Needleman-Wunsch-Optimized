# Optimizing the Needleman-Wunsch
## Introduction
### Purpose:
This repo was made in the efforts of making an accessible bioinformatics library where you can easily perform pairwise sequence alignment without having to worry about the hardware of your machine, since all of the libraries out there DON'T as a result consuming a lot of RAM and CPU power.
We hope that we will be able to cover all of the necessary functional requirements needed by anyone to perform "Pairwise Sequence Alignment" properly.
This repo is still in it's early days, with correct care and patience we will help it thrive.
### This repo contains:
1. The initial version of the Needleman-Wunsch algorithm that calculates the best score alongside the best possible path written fully in Python.
2. An optimized / parallelized version of the NW algorithm. This technique improves upon the Row Based "RW" method by doing it for both Row and Column simultaneously to allow for parallelism whilst taking care of all dependencies.
3. The "read" file allows us to feed in our sequences for us to send them as inputs to the functions.
4. The best code utilizes the ctype library to run at an extremely high speed with very high efficiency. Since, as we know, C/C++ are much faster than Python especially for heavy computation tasks.
## To be added:
### Parallelized Combination of both the Row Based and Antidiagonal techniques
We're aiming to push an extremely optimized solution that uses a combination of both RW and AD techniques in a parallel format to give a solution at a benchmark speed.
### Returning the best path alongside the score
Ideally we want to also return the best match path we found alongside the score we saw, this makes our solutions formidable.
### Taking into consideration affine gap penalties
In real world practice, opening gaps should cost more than inserting gaps, we want to have this represented EFFICIENTLY.
### Publish as the most accessible and optimized python library
Eventually, this should be available for use in a very accessible manner.
