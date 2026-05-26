Registri = {
"R0" : 0,
"R1" : 1,
"R2" : 2,
"R3" : 3,
"R4" : 4,
"R5" : 5,
"R6" : 6,
"R7" : 7,
"R8" : 8,
"R9" : 9,
"R10" : 10,
"R11" : 11,
"R12" : 12,
"ESP" : 13,
"SP" : 14,
"PC" : 15,
}
Opcodes = {
"Add" : 0x01,
"SUB" : 0x02,
"CMP" : 0x03,
"SET" : 0x04,
"LOAD" : 0x05,
"STORE" : 0x06,
"HALT" : 0x07,
"ROM_Read" : 0x08,
"ROM_Write" : 0x09,
"MOV" : 0x0A,
"PUSH" : 0x0B,
"POP" : 0x0C,
"NOP" : 0x0D,
"JMP" : 0x0f,
"JMQ" : 0x10,
"JML" : 0x11,
"JMM" : 0x12,
"MUL" : 0x13,
"DIV" : 0x14,
"INC" : 0x15,
"DEC" : 0x16,
"XOR" : 0x17,
"AND" : 0x18,
"OR" : 0x19,
"NOT" : 0x1a,
"SHL" : 0x1b,
"SHR" : 0x1c,
"CALL" : 0x1d,
"RET" : 0x1e,
"MEM_LOCK" : 0x1f
}

RAM_SIZE = 131072; # 1 MB 131.072
ROM_SIZE = 1310720; # 10 MB 1.310.720


GPU_IO = RAM_SIZE - (RAM_SIZE / 100 ) * 10; #Adresa de start din memorie dedicata comunicari cu GPU 
INT_TABEL = GPU_IO -  (RAM_SIZE / 100 ) * 5; #Adresa de start a codului pentru a menegerea intreruperile
INT_DATA = INT_TABEL - (RAM_SIZE / 100 ) * 5; #Adresa de start unde se vor pune datele de catre dispozitivele externe 

#Compiler Vars
BOOT_LOADER_SIZE = 9  
CURENT_ADDR = BOOT_LOADER_SIZE - 1 # Memoreaza pentru moment adresa curenta la care am ajuns cu generarea binarului  

Source_File_Name = ""
dir_include = [] #lista folosita in Preprocesare pentru a memora fisierele deja incluse 
dir_define = {}
LabelsDict = {} #Dictionarul pentru labeuri 

Variabile = {
    "var":{},
    "arr":{}
}


if __name__ == "__main__":
    pass