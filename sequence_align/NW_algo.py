m = len(seq1)
n = len(seq2)
init_mat = []

#These we can change based on our test case, for now let's leave them 1 -1 -1
match = 1
mismatch = -1
gap = -1
for i in range(m+1):                            
    t = []
    for j in range(n+1):                        
        t.append(0)
    init_mat.append(t)                       
#initialize the first row of the matrix
#ofcourse we need to follow topological sorting
for j in range(n+1):
    init_mat[0][j] = gap*j                      
#doing the same thing for the first col
for i in range(m+1):
    init_mat[i][0] = gap*i                      
print("PART1")
#this part is the intensive part
#when looking into how much resources the system used
#this mainly pops as the reason behind it
for i in range(1,m+1):
    for j in range(1, n+1):
        if seq1[i-1] == seq2[j-1]:
            init_mat[i][j] = max(init_mat[i][j-1]+gap, init_mat[i-1][j]+gap, init_mat[i-1][j-1]+match)
        else:
            init_mat[i][j] = max(init_mat[i][j-1]+gap, init_mat[i-1][j]+gap, init_mat[i-1][j-1]+mismatch)
#the prints are for testing purposes to catch where the code is being
#exhaustive
print("PART2")

#this is the simplest and fastest section as it just moves through the resuls
#we obtained and puts them into the corresponding variables
seq1_align = ""
seq2_align = ""
score = 0

i = m
j = n
while (i>0 or j>0):
    if seq1[i-1] == seq2[j-1]:
        seq1_align += seq1[i-1]
        seq2_align += seq2[j-1]
        i -= 1
        j -= 1
    #we create this reference list to backtrack from all possible places
    elif seq1[i-1] != seq2[j-1]:
        temp_list = [init_mat[i-1][j-1], init_mat[i-1][j], init_mat[i][j-1]]
        #mismatch
        if max(temp_list) == temp_list[0]:
            seq1_align += seq1[i-1]
            seq2_align += seq2[j-1]
            i -= 1
            j -= 1
        #indels
        elif max(temp_list) == temp_list[1]:
            seq1_align += seq1[i-1]
            seq2_align += "-"
            i -= 1
        elif max(temp_list) == temp_list[-1]:
            seq1_align += "-"
            seq2_align += seq2[j-1]
            j-=1

print("PART3")
seq1_align = seq1_align[::-1]
seq2_align = seq2_align[::-1]                   

#build the string to get the same result as the GALAXY
match_string = ""
for i in range(len(seq1_align)):
    if seq1_align[i] == seq2_align[i]:
        match_string += "|"
    elif seq1_align[i] != seq2_align[i]:
        if (seq1_align[i] == "-" or seq2_align[i] == "-"):
            match_string += " "
        else:
            match_string += "*"
alignment_score = 0
for i in range(len(match_string)):
    if match_string[i] == "|":
        alignment_score += 1
    elif (match_string[i] == "*" or match_string[i] == " "):
        alignment_score += -1
print(seq1_align)
print(match_string)
print(seq2_align)
print("Alignment score:", alignment_score)
