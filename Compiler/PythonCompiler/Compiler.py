import os , Setings

#Variabile globale 
Source_File_Name = ""
dir_include = [] #lista folosita in Preprocesare pentru a memora fisierele deja incluse 
dir_define = {}
LabelsDict = {}


def ReadFile(File_name: str):
    global Source_File_Name
    #VOm incepe prima data prin a citi fisierul sursa 
    Source_File_Name = File_name

    #Variabila care memoreaza continutul fisierului 
    Content = []
    with open(Source_File_Name, "r" , encoding="utf-8") as f : 
        Content = f.readlines()

    #Inainte sa returnam lisata de linii trebuie sa o curatam prima data de new line
    for i , line in enumerate(Content):
        Content[i] = line.strip() 

    #Acum vom returna continutul 
    return Content

def Preprocesare(Lines : list):
    global Source_File_Name
    global dir_include
    global dir_define
    #Vom face doua treceri , una care detecteaza defineurile si includurile si cealalta trecere pentru inlocuirea definerilor sau integrarea codului nou introdus 
    
    New_Source_Code = []
    Source_code = []

    dir_include.append(Source_File_Name)

    for i , line in enumerate(Lines):
        #eliminam liniile goale din lista 
        if len(line) == 0:
            continue
        if "//" in line: 
            line = line[:line.find("//")]
            Lines[i] = line


        if line.startswith("#define") :
            new_line = line[len("#define"):].split()
        
            nume = new_line[0]
            value = new_line[1]
            dir_define[nume] = value
        
            continue

        #Aici ne ocupam de copierea includerilor 
        if line.startswith("#include"):
            #Daca exista o includere atunci obtinem numele acesteia , citim fisierul sursa si repetam procesul recursiv pana includem toate librariile necesare 
            include_name = line[len("#include"):].replace('"' , "").split()[0]
            if len(dir_include) != 0 :
                if include_name in dir_include:
                    continue
                else:
                    dir_include.append(include_name)

            New_SRC = ReadFile(include_name)
            New_Source_Code += Preprocesare(New_SRC)
            continue

        Source_code.append(Lines[i])

    New_Source_Code += Source_code

    return New_Source_Code 


#FUnctia de tokenizare , aceasta functie va separa fiecare linie in tokenuri diferite 
def Tokenizare(Lines : list ):
    global dir_define
    All_Tokens = []
    delimitatori = "{}[]()+=-*/," #Delimitatori pe care ii vom folosi pentru a separa tokenurile 

    def AddToken(token:str):
        if token in dir_define:
            token = dir_define[token]
        
        if len(token) != 0 : 
            Line_Tokens.append(token)

    for line in Lines:

        Line_Tokens = []
        curent = ""
        #o sa verificam caracter cu careacter pentru a separa tokenurile  
        for chr in line:

            if chr in delimitatori:
                
                if curent != "":
                    AddToken(curent)
                    curent = ""

                Line_Tokens.append(chr)

            elif chr.isspace():
                if curent != "":
                    AddToken(curent)
                    curent = ""
                
            else:
                curent += chr
        AddToken(curent)

        if len(Line_Tokens) != 0:
            All_Tokens.append(Line_Tokens)
    
    return All_Tokens

#detectam toate labelurile 

def Labels(Lines : list):
    global LabelsDict
    New_Tokens = []

    for index , line in enumerate(Lines):
        label_pos = line[0].find(":") 
        if label_pos != -1:
            label_name = line[0][:label_pos]
            LabelsDict[label_name] = index
        else:
            if len(line) != 0 : 
                New_Tokens.append(line)
            

    
    print(f"Labeluri : \n {LabelsDict}")
    return New_Tokens


#Procesarea de expansiuni 
def Procesare_Expansiuni(Lines: list):
    #Mai intai trebuie sa detectam sectiunea de expansiune 
    simbol_expansiune_deschidere = "["
    simbol_expansiune_inchidere = "]"
    
    All_expansion = []
    Tokens_Expansiune = []
    #Luam linie cu linie
    for linie in Lines:
        i = 0
        #luam pe urma token cu token din acea linie 
        while(i < len(linie)):
            token = linie[i] 

            #Verificam daca tokenul curent este o paranteza dreapta deschisa daca , da atunci avem o expansiune 
            if token == simbol_expansiune_deschidere:
                
                i += 1
                token = linie[i]
                #Parcurgem toata linia pana gasim paranteza inchisa sau sfarsitul liei 
                while token != simbol_expansiune_inchidere and i < len(linie):
                    Tokens_Expansiune.append(token)
                    i += 1
                    token = linie[i]

                #In acest punct avem intreaga expansiune salvata in lista Tokens_Expansiune 
                All_expansion.append(Tokens_Expansiune)
                Tokens_Expansiune = []

            i += 1
        
    
    print(All_expansion)




#Citim prima data fisierul 
Source_Code = ReadFile("CODE.src")

#Preprocesam codul sursa 
Preprocesed_Source_Code = Preprocesare(Source_Code)

#Tokwnizam codul preprocesat 
Tokens = Tokenizare(Preprocesed_Source_Code )

#Afisam fiecare stagiu 
print(f"Codul sursa : \n{Source_Code} \n\nCodul Preprocesat: \n {Preprocesed_Source_Code}  \n\nTokens: {Tokens} ")

#Afisam toate labelurile 
Tokens = Labels(Tokens)

#Afisam codul preprocesat asa cum apare in source_code 
print()
for line in Tokens:
    for t in line:
        print(t , end= " ")
    print()

Procesare_Expansiuni(Tokens)