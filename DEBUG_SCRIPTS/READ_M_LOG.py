#Fisierul cu logul pe care vrem sa il citim 
target_path = r"C:\Users\goguj\Desktop\VM\build\Memory.log"


DATA_DIC_LIST = []

file = []
with open(target_path , "r" , encoding="utf-8") as f:
    file = f.readlines()

#Dam split la datele originale 
data = []
for f in file:
    if len(f) == 0:
        continue

    data.append(f.split())
    #print(f.split())

new_data = []
to_erase = "|:"
for line in data:
    new_line = []
    for token in line:
        if token == "Addr" or token == "VALUE":
            continue
        tok = ""
        for char in token:
            if char in to_erase:
                continue
            tok += char 
        if len(tok) > 0:
            new_line.append(tok)
    new_data.append(new_line)


for line in new_data:
    DATA_DIC = {'type':"",
                'addr': 0x02,
                 'value': 0x02,
                 'component':""}
    if len(line) >= 4:
        DATA_DIC["component"] = line[0]
        DATA_DIC["type"] = line[1]
        DATA_DIC["addr"] = line[2]
        DATA_DIC["value"] = line[3]
        DATA_DIC_LIST.append(DATA_DIC)
   

#print(DATA_DIC_LIST)

# last_addr = 0 
# last_value = 0 
# last_component = "" 
# index  = 1

# for i , data in enumerate(DATA_DIC_LIST):
#     if int(data["addr"] , 16) >= 0x000000000001CCD4:
#         if int(DATA_DIC_LIST[i - 1]['addr'] , 16) == int(DATA_DIC_LIST[i]['addr'] , 16) and int(DATA_DIC_LIST[i - 1]['value']  , 16)== int(DATA_DIC_LIST[i]['value'] , 16) and DATA_DIC_LIST[i - 1]["component"] == DATA_DIC_LIST[i]["component"] and DATA_DIC_LIST[i - 1]["type"] == DATA_DIC_LIST[i ]["type"] :
#             index += 1
#         else:
#             print(
#                str(index) +"x ",
#                data["component"] + ' |',
#                 data['type'] + '| ',
#                 "ADDR  : "+data['addr'] + " | ",
#                 "VALUE: " + data["value"] + ' '
#             )
#             index = 1

last_addr = 0 
last_value = 0 
last_component = "" 
last_type = ""
index = 0

for i, data in enumerate(DATA_DIC_LIST):
    if int(data["addr"], 16) >= 0x000000000001CCD4 or int(data["addr"] , 16) == 0x00000000000003EB :
    #if data["component"] == "CPU" and data["type"] == "WRITE":
        # Prima intrare sau diferită de precedenta
        if i == 0 or (data["addr"] != last_addr or 
                      data["value"] != last_value or 
                      data["component"] != last_component or 
                      data["type"] != last_type): 
            
            # Afișăm intrarea anterioară dacă există (cu multiplicator)
            if i > 0:
                print(f"{index}x {last_component} | {last_type} | ADDR: {last_addr} | VALUE: {last_value}")
            
            # Resetăm indexul pentru noua secvență
            index = 1
            # Salvăm noile valori
            last_addr = data["addr"]
            last_value = data["value"]
            last_component = data["component"]
            last_type = data["type"]
        else:
            # Incrementăm contorul pentru duplicate
            index += 1

# Afișăm ultima intrare
if index > 0:
    print(f"{index}x {last_component} | {last_type} | ADDR: {last_addr} | VALUE: {last_value}")