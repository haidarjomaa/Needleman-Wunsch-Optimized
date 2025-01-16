import multiprocessing
import os
import time
from read_seq import read_genome

#for each row we have this function that the process will run
def process_row(row, prev_row, match_reward: int, mismatch_penalty: int,
                indel_penalty: int, s: str, t: str, index):
    #for the first row on the exterior - we only have gaps in it
    if index == 0:
      for i in range(1, len(s)+1):
        row[i] = row[i-1] + indel_penalty
    #for the rest of the rows
    else:
      for i in range(1, len(row)):
        #if we have a match between the 2 strings (while respecting the order of fixed t - moving s)
        if s[index+i-1] == t[index-1]:
          row[i] = max(prev_row[i]+match_reward, prev_row[i+1]+indel_penalty, row[i-1]+indel_penalty)
        #otherwise just use the mismatch penalty
        else:
          row[i] = max(prev_row[i]+mismatch_penalty, prev_row[i+1]+indel_penalty, row[i-1]+indel_penalty)
    return row

#this one has the exact same description to the row function with some name and variable changes
def process_col(col, prev_col, match_reward: int, mismatch_penalty: int,
                indel_penalty: int, s: str, t: str, index):
    if index == 0:
      for i in range(1, len(t)+1):
        col[i] = col[i-1] + indel_penalty
    else:
      for i in range(1, len(col)):
        if s[index-1] == t[index+i-1]:
          col[i] = max(prev_col[i]+match_reward, prev_col[i+1]+indel_penalty, col[i-1]+indel_penalty)
        else:
          col[i] = max(prev_col[i]+mismatch_penalty, prev_col[i+1]+indel_penalty, col[i-1]+indel_penalty)
    return col

def global_alignment(match_reward: int, mismatch_penalty: int, indel_penalty: int,
                     s: str, t: str):
    m = len(s)
    n = len(t)
    prev_row = [0]*(m+1)
    prev_col = [0]*(n+1)
    #create a pool of max number of processes for the multiprocessing
    with multiprocessing.Pool(processes=os.cpu_count()) as pool:
        #loop over the elements based on the the string values
        for i in range(max(1+m, 1+n)):
            #initialize the current rows
            row = [0]*(m+1-i)
            col = [0]*(n+1-i)
            if i != 0:
              if i < m+1 and i < n+1:
                #initializing the common value between row/col
                if s[i-1] == t[i-1]:
                 row[0] = max(prev_row[0]+match_reward, prev_row[1]+indel_penalty, prev_col[1]+indel_penalty)
                 col[0] = max(prev_row[0]+match_reward, prev_row[1]+indel_penalty, prev_col[1]+indel_penalty)
                else:
                 row[0] = max(prev_row[0]+mismatch_penalty, prev_row[1]+indel_penalty, prev_col[1]+indel_penalty)
                 col[0] = max(prev_row[0]+mismatch_penalty, prev_row[1]+indel_penalty, prev_col[1]+indel_penalty)

                prev_row = pool.apply_async(process_row, args=(row, prev_row, match_reward, mismatch_penalty, indel_penalty,
                      s, t, i,))
                prev_col = pool.apply_async(process_col, args=(col, prev_col, match_reward, mismatch_penalty, indel_penalty,
                      s, t, i,))
                #fetch the result and set flag to done for row
                prev_row = prev_row.get()
                prev_col = prev_col.get()

            else:
              prev_row = pool.apply_async(process_row, args=(row, prev_row, match_reward, mismatch_penalty, indel_penalty,
                    s, t, i,))
              prev_col = pool.apply_async(process_col, args=(col, prev_col, match_reward, mismatch_penalty, indel_penalty,
                    s, t, i,))
              prev_row = prev_row.get()
              prev_col = prev_col.get()
              
        #wait until the 2 finish their tasks
        pool.close()
        pool.join()
    #return the final node of the DP graph
    if len(prev_row) > len(prev_col):
      return prev_row[-1]
    else:
      return prev_col[-1]

if __name__ == '__main__':
    start_time = time.time()
    seq1, seq2 = read_genome("C:\Haidar\AUB\Sem 1\CMPS 396AG - BioInf\Project\Code\[NC_004718.3].fasta", "C:\Haidar\AUB\Sem 1\CMPS 396AG - BioInf\Project\Code\[SPAdes_on_data_93_and_data_92__contigs_(fasta)].fasta")
    print(global_alignment(5, -3.6, -7, seq1, seq2))
    end_time = time.time()
    print(f"Execution time: {end_time - start_time} seconds")
