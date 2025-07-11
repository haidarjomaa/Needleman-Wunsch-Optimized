def read_genome(file1, file2):
    with open(file1,"r") as f:
        strings = f.readlines()
    strings = [i.strip() for i in strings]
    result1 = ""
    for i in strings:
        result1 += i
    with open(file2,"r") as f:
        strings = f.readlines()
    strings = [i.strip() for i in strings]
    result2 = ""
    for i in strings:
        result2 += i
    return result1.encode('utf-8'), result2.encode('utf-8')
