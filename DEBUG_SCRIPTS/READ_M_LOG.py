


target_path = r"C:\Users\goguj\Desktop\VM\build\Memory.log"

READ = "|READ"
WRITE = "|WRITE|"

READ_DIC = {}
WRITE_DIC = {}

file = []
with open(target_path , "r" , encoding="utf-8") as f:
    file = f.readlines()

data = []
for f in file:
    data.append(f.split())
    #print(f.split())

for d in data:
    if d[1] == WRITE and d[4] == "0x000000000001CCD4" or d[4] == "0x000000000001CCD5" or d[4] == "0x000000000001CCD6":
        for ceva in d:
            print(ceva , end= " ")
        print()
    if d[1] == READ and d[5] == "0x000000000001CCD4" or d[5] == "0x000000000001CCD5" or d[5] == "0x000000000001CCD6":
        for ceva in d:
            print(ceva , end= " ")
        print()

   