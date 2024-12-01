import multiprocessing

def process_row(row, prev_row, match_reward: int, mismatch_penalty: int,
                indel_penalty: int, s: str, t: str, index):
    if index == 0:
      for i in range(1, len(s)+1):
        row[i] = row[i-1] + indel_penalty

    else:
      for i in range(1, len(row)):
        if s[index+i-1] == t[index-1]:
          row[i] = max(prev_row[i]+match_reward, prev_row[i+1]+indel_penalty, row[i-1]+indel_penalty)
        else:
          row[i] = max(prev_row[i]+mismatch_penalty, prev_row[i+1]+indel_penalty, row[i-1]+indel_penalty)
    return row

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
    """
    Compute the global alignment of two strings based on given rewards and penalties.

    Args:
    match_reward (int): The reward for a match between two characters.
    mismatch_penalty (int): The penalty for a mismatch between two characters.
    indel_penalty (int): The penalty for an insertion or deletion.
    s (str): The first string.
    t (str): The second string.

    Returns:
    Tuple[int, str, str]: A tuple containing the alignment score and the aligned strings.
    """
    m = len(s)
    n = len(t)
    prev_row = [0]*(m+1)
    prev_col = [0]*(n+1)

    with multiprocessing.Pool(processes=2) as pool:
        for i in range(max(1+m, 1+n)):
            row = [0]*(m+1-i)
            col = [0]*(n+1-i)
            if i != 0:
              if i < m+1 and i < n+1:
                if s[i-1] == t[i-1]:
                 row[0] = max(prev_row[0]+match_reward, prev_row[1]+indel_penalty, prev_col[1]+indel_penalty)
                 col[0] = max(prev_row[0]+match_reward, prev_row[1]+indel_penalty, prev_col[1]+indel_penalty)
                else:
                 row[0] = max(prev_row[0]+mismatch_penalty, prev_row[1]+indel_penalty, prev_col[1]+indel_penalty)
                 col[0] = max(prev_row[0]+mismatch_penalty, prev_row[1]+indel_penalty, prev_col[1]+indel_penalty)

                prev_row = pool.apply_async(process_row, args=(row, prev_row, match_reward, mismatch_penalty, indel_penalty,
                      s, t, i,))
                prev_row = prev_row.get()

                prev_col = pool.apply_async(process_col, args=(col, prev_col, match_reward, mismatch_penalty, indel_penalty,
                      s, t, i,))
                prev_col = prev_col.get()
            else:
              prev_row = pool.apply_async(process_row, args=(row, prev_row, match_reward, mismatch_penalty, indel_penalty,
                    s, t, i,))
              prev_row = prev_row.get()

              prev_col = pool.apply_async(process_col, args=(col, prev_col, match_reward, mismatch_penalty, indel_penalty,
                    s, t, i,))
              prev_col = prev_col.get()

        pool.close()
        pool.join()

    if len(prev_row) > len(prev_col):
      return prev_row[-1]
    else:
      return prev_col[-1]

print(global_alignment(1, -1, -1, seq1, seq2))
