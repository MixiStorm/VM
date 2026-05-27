import Setings

#Calculam adresele variabilelor 
def Calculate_Vars_addr():
    adresa_curenta = Setings.BOOT_LOADER_SIZE 

    """
        Scurta prezentare a memoriei: 

            ->BOOT_LOADER = 9 instruction 
            ->SECTION_DATA = ? 
            ->SECTION_TEXT = ? 

            Prima data vom pune vaiabilele si pe urma vom aseza arry-urile in memorie 
    """
    #Prima data parcurgem variabilele 
    for Var_name  , detalii in Setings.Variabile["var"].items():
        detalii['addr'] = adresa_curenta 
        adresa_curenta += 1

    #Acum vom parcurge arry-urile 
    for arr_name , detalii in Setings.Variabile["arr"].items():
        detalii['addr'] = adresa_curenta 
        adresa_curenta += int(detalii['dimension'])
    

    Setings.CURENT_ADDR = adresa_curenta

#Dupa calcularea adreselor de variabile urmatorul pas logic este inlocuirea in program a numelor acelor variabile cu adresele in sine 
def Resolve_Var_Addr(Lines: list) -> list:
    Procesed_code = []
    #mai intai trebuie sa parcurgem lista 
    for line in Lines:
        ln = []
        for token in line:
            #Acum luam token cu token , daca gasim un nume de variabila sau arr , le vom inlocui cu adresele lor corespunzatoare 
            if token in Setings.Variabile['var']: 
                ln.append(str(Setings.Variabile['var'][token]['addr']))

            elif token in Setings.Variabile['arr']:
                ln.append(str(Setings.Variabile['arr'][token]['addr']))
            
            else:
                ln.append(token)

        Procesed_code.append(ln)

    return Procesed_code

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
    #Vom face doua treceri , una care detecteaza defineurile si includurile si cealalta trecere pentru inlocuirea definerilor sau integrarea codului nou introdus s
    New_Source_Code = []
    Source_code = []

    Setings.dir_include.append(Source_File_Name)

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
            Setings.dir_define[nume] = value
        
            continue

        #Aici ne ocupam de copierea includerilor 
        if line.startswith("#include"):
            #Daca exista o includere atunci obtinem numele acesteia , citim fisierul sursa si repetam procesul recursiv pana includem toate librariile necesare 
            include_name = line[len("#include"):].replace('"' , "").split()[0]
            if len(Setings.dir_include) != 0 :
                if include_name in Setings.dir_include:
                    continue
                else:
                    Setings.dir_include.append(include_name)

            New_SRC = ReadFile(include_name)
            New_Source_Code += Preprocesare(New_SRC)
            continue

        Source_code.append(Lines[i])

    New_Source_Code += Source_code

    return New_Source_Code 

#FUnctia de tokenizare , aceasta functie va separa fiecare linie in tokenuri diferite 
def Tokenizare(Lines : list ):

    All_Tokens = []
    delimitatori = "{}[]()+=-*/," #Delimitatori pe care ii vom folosi pentru a separa tokenurile 

    def AddToken(token:str):
        if token in Setings.dir_define:
            token = Setings.dir_define[token]
        
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
def Procesare_vars(Lines: list):
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
            # if line[0].endswith(":"):
            #     label_name = line[0][:-1]
            #     Setings.LabelsDict[label_name] = len(New_Tokens) # Folosește lungimea curentă din New_Tokens, nu indexul brut!
            #     continue
            
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
                Setings.Variabile["var"][var_name] = {
                    "dimension": 1,
                    "value": var_size_value
                }
            elif var_type == "arr":
                Setings.Variabile["arr"][var_name] = {
                    "dimension": var_size_value,
                    "value": 0
                }
            else:
                raise ValueError(f"Tipul de variabilă invalid la linia {current_index} -> {var_type}")
    #La final vom calcula adresele variabilelor 
    Calculate_Vars_addr()
    New_Tokens = Resolve_Var_Addr(New_Tokens)

    

    return New_Tokens

