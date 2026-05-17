import os , Setings

#Variabile globale 
Source_File_Name = ""
dir_include = [] #lista folosita in Preprocesare pentru a memora fisierele deja incluse 
dir_define = {}
LabelsDict = {}

Variabile = {
    "var":{},
    "arr":{}
}


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

def Procesare_labels_and_vars(Lines: list):
    global LabelsDict 
    global Variabile
    
    New_Tokens = []
    in_section = False
    section_type = ""
    
    # Folosim un iterator/enumerator pentru a parcurge liniile curat, linie cu linie
    for current_index, line in enumerate(Lines):
        # Sărim peste liniile complet goale pentru siguranță
        if not line:
            continue
            
        if not in_section:
            # 1. Procesarea de label-uri (ex: "main:")
            # Verificăm dacă primul token conține caracterul ":" la sfârșit
            if line[0].endswith(":"):
                label_name = line[0][:-1]
                LabelsDict[label_name] = len(New_Tokens) # Folosește lungimea curentă din New_Tokens, nu indexul brut!
                continue
            
            # 2. Detectarea intrării într-o secțiune (ex: "section data. {")
            if line[0] == "section":
                in_section = True
                
                # Validăm tipul de secțiune (trebuie să fie al doilea token)
                if len(line) < 2 or line[1] not in ("data.", "text."):
                    invalid_type = line[1] if len(line) > 1 else "Lipsă"
                    raise ValueError(f"Tipul de secțiune de memorie invalid, introdus: {invalid_type}")
                
                section_type = line[1]
                
                # Validăm deschiderea acoladei (poate fi pe aceeași linie: section data. {)
                if "{" not in line:
                    raise ValueError("Se așteaptă introducerea simbolului -> {")
                continue
                
            # Dacă nu este nici label, nici secțiune, adăugăm linia de cod mașină pur
            New_Tokens.append(line)
            
        else:
            # 3. Suntem în interiorul unei secțiuni (procesare variabile/array-uri)
            
            # Dacă linia conține închiderea secțiunii, ieșim din modul secțiune
            if "}" in line:
                in_section = False
                continue
                
            # Validăm că avem o structură completă de linie (tip nume valoare/dimensiune)
            if len(line) < 3:
                raise ValueError(f"Linie de definiție variabilă invalidă sau incompletă la linia {current_index}: {line}")
                
            var_type = line[0]         # "var" sau "arr"
            var_name = line[1]         # Numele variabilei
            var_size_value = line[2]   # Valoarea sau dimensiunea ei
            
            if var_type == "var":
                Variabile["var"][var_name] = {
                    "dimension": 1,
                    "value": var_size_value
                }
            elif var_type == "arr":
                Variabile["arr"][var_name] = {
                    "dimension": var_size_value,
                    "value": 0
                }
            else:
                raise ValueError(f"Tipul de variabilă invalid la linia {current_index} -> {var_type}")

    return New_Tokens

    



class BinOP:
    def __init__(self , stanga , op , dreapta):
        self.stanga = stanga
        self.op = op 
        self.dreapta = dreapta

# --- FUNCTIA DE AFISARE RECURSIVA ---
def afiseaza_ast(nod, prefix="", este_ultimul=True):
    """
    Afișează un AST în consolă sub formă de arbore ierarhic (text-graphics).
    """
    # Dacă nodul curent este o instanță de BinOP (adică este un operator)
    if isinstance(nod, BinOP):
        # Afișăm operatorul curent cu prefixul potrivit
        print(prefix + ("└── " if este_ultimul else "├── ") + nod.op)
        
        # Calculăm prefixul pentru copiii acestui nod
        nou_prefix = prefix + ("    " if este_ultimul else "│   ")
        
        # Apelăm recursiv pentru copilul stâng și cel drept
        # Copilul stâng NU este ultimul (urmează cel drept), deci este_ultimul=False
        afiseaza_ast(nod.stanga, nou_prefix, este_ultimul=False)
        # Copilul drept este ultimul din acest sub-arbore
        afiseaza_ast(nod.dreapta, nou_prefix, este_ultimul=True)
        
    else:
        # Dacă este doar un număr (valoare text/int), îl afișăm ca pe o frunză
        print(prefix + ("└── " if este_ultimul else "├── ") + str(nod))

def Parse(expr , poz = 0):

    #Puterea fiecarui poerator 
    operator_power = {
        "*": 2.0,
        "/": 2.0,
        "+": 1.0,
        "-": 1.0
    }
    

    #Daca este doar un numar il retrunam si gata , presupunand ca toate expresiile sunt scrise corect 
    if len(expr) == 1 :
        return expr[0]

    if expr[0] == '(' and expr[-1] == ')':
        # Verificăm totuși dacă paranteza de la început se închide chiar la sfârșit, nu pe parcurs
        contor = 0
        inchisa_la_final = True
        
        for i in range(len(expr) - 1):
            if expr[i] == '(': contor += 1
            if expr[i] == ')': contor -= 1
            if contor == 0 and i > 0:
                inchisa_la_final = False
                break
        if inchisa_la_final:
            return Parse(expr[1:-1])


    #Cautam cel mai important operator de pana acum 
    operatori = "+-*/"
    paranteze = "()"
    last_operator_power = float('inf')  #Am pus un numar random pentru ca trebuie sa fie mai mari ca ori ce putere de operatori predefinita pana acum 
    poz = -1
    depth = 0


    for i in range(len(expr) - 1 , -1 , -1):
        if expr[i] == "(":
            depth -= 1

        elif expr[i] == ")":
            depth += 1


        if expr[i] in operatori :
            """
                Logica este in felul urmator 
                    Daca puterea operatorului curent este mai mica ca ultimul il atribuim 
                    Pe urma o sa verificam daca am ajuns la sfarsitul listei , daca da ne oprim si returnam 
            """
            if operator_power[expr[i]] + (depth * 10 ) < last_operator_power:
                last_operator_power = operator_power[expr[i]] + (depth * 10 )
                poz = i

    if poz == -1:
        return expr[0]

    stanga = expr[:poz]
    dreapta = expr[poz + 1 :]
    op = expr[poz]
    
    return BinOP(
        Parse(stanga),
        op ,
        Parse(dreapta)
        )

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
Tokens = Procesare_labels_and_vars(Tokens)

#Afisam codul preprocesat asa cum apare in source_code 
print()
for line in Tokens:
    for t in line:
        print(t , end= " ")
    print()

print(f"Variabile : \n{Variabile}")
#Procesare_Expansiuni(Tokens)