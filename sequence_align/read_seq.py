def read_genome(file1, file2):
    with open(file1,"r") as f:
        strings = f.readlines()
    strings = strings[1:]
    strings = [i.strip() for i in strings]
    result1 = ""
    for i in strings:
        result1 += i
    with open(file2,"r") as f:
        strings = f.readlines()
    strings = strings[1:]
    strings = [i.strip() for i in strings]
    result2 = ""
    for i in strings:
        result2 += i
    return result1, result2
