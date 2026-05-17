import os , Setings 
import Preprocesare_cod as PreCod



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


#========FORNT_END=============
#Citim prima data fisierul 
Source_Code = PreCod.ReadFile(r"C:\Users\goguj\Desktop\VM\Compiler\PythonCompiler\CODE.src")
#Preprocesam codul sursa 
Preprocesed_Source_Code = PreCod.Preprocesare(Source_Code)
#Tokwnizam codul preprocesat 
Tokens = PreCod.Tokenizare(Preprocesed_Source_Code )
#Afisam fiecare stagiu 
print(f"Codul sursa : \n{Source_Code} \n\nCodul Preprocesat: \n {Preprocesed_Source_Code}  \n\nTokens: {Tokens} ")


#======BACK_END==============

#Afisam toate labelurile 
Tokens = PreCod.Procesare_labels_and_vars(Tokens)

#Afisam codul preprocesat asa cum apare in source_code 
print()
for line in Tokens:
    for t in line:
        print(t , end= " ")
    print()

print(f"Variabile : \n{Setings.Variabile}")
#Procesare_Expansiuni(Tokens)