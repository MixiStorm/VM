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

def Parse(expr , poz = 0) ->BinOP:

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

"""
Functia Proces_Expresion , proceseaza o expresie , daca este o expresie formata doar din numere atunci returneaza rezultatul 
    expresiei , altfel retruneaza un nou AST procesat , in care se afla epresi inre registri si numere sau registri si registri 
""" 
def Proces_Expresion(NOD : BinOP):
        
        #Functia va merge recursiv prin Tree 
        #Caz de baza 
        if not isinstance(NOD , BinOP):
            #Atunci poate fii atat numar sau string
            #print(f"Functie [EXTEND] : NOD(' {NOD} ') , IS_DIGIT : {NOD.isdigit()}") 
            if NOD.isdigit():
                return int(NOD)
            else :
                return NOD
        
        #Coboram pe ramura stanga 
        stanga = Proces_Expresion(NOD.stanga)

        #Coboram pe ramura dreapta 
        dreapta = Proces_Expresion(NOD.dreapta)

        #Daca ambele frunze sunt numere atunci performam calcule pe ele  
        if isinstance(stanga , int) and isinstance(dreapta , int):
            if NOD.op == "+": return stanga + dreapta
            if NOD.op == "-": return stanga - dreapta 
            if NOD.op == "*": return stanga * dreapta
            if NOD.op == "/": return stanga / dreapta
            
        #Daca o frunza se prezinta ca fiind un registru atunci returnam nodul reconstruit 
        if isinstance(stanga , str) and stanga not in Setings.Registri:
            raise ValueError(f"[ERROR AST] Simbol invalid : '{stanga}'")
        
        if isinstance(dreapta , str) and dreapta not in Setings.Registri:
            raise ValueError(f"[ERROR AST] Simbol invalid : '{dreapta}'")

        
        #Daca nu am putut prelucra datele returnam nodul curent 
        return BinOP(stanga , NOD.op , dreapta) 

"""
Functia Expand are scopul de a expanda o instructiune in mai multe instructiuni fiind necesare in cazul unor operati intre regostri sau 
    operati intre registri si numere 
"""
def Expand(NOD : BinOP , ORG : list)->list:
    Stiva = []
    Lista_Instructiuni = []
    if len(ORG) == 0:
        raise ValueError(f"Trebuie introdusa si instructiunea originala inainte de a creea expansiunea")

    def PUSH(reg) -> list:
        if not reg in Stiva:
            Stiva.append(reg)
            return ["push" , reg]
        return None
    
    def POP():
        reg = Stiva.pop()
        return ["pop" , reg]

    #functi matematice ce genereaza instructiunile portrivite operatiei in sine 
    def ADD(a , b)-> list:
        return ["add" , a  , b]

    def SUB(a , b) ->list:
        return ["sub" , a  , b]
    
    def MUL(a , b) ->list:
        return ["mul" , a , b]

    def DEV(a , b) ->list:
        return ["dev" , a  , b]
    
    def GenCod(curent_nod : BinOP):
        #Cazul de baza 
        if not isinstance(curent_nod , BinOP):
            return curent_nod

        stanga = GenCod(curent_nod.stanga)
        dreapa = GenCod(curent_nod.dreapta)

        #performam operatiile acum 
        if isinstance(stanga , str):
            #Daca este string stim sigur ca este un registru 
            temp_list = PUSH(stanga)
            if temp_list != None:
                Lista_Instructiuni.append(temp_list)
        if isinstance(dreapa , str):
            #Daca este string stim sigur ca este un registru 
            temp_list = PUSH(dreapa)
            if temp_list != None:
                Lista_Instructiuni.append(temp_list)

        if NOD.op == "+":
            Lista_Instructiuni.append(ADD(str(stanga) ,str(dreapa)))
        
        if NOD.op == "-":
            Lista_Instructiuni.append( SUB(str(stanga) ,str(dreapa)))
        
        if NOD.op == "*":
            Lista_Instructiuni.append(MUL(str(stanga) ,str(dreapa)))
        
        if NOD.op == "/": 
            Lista_Instructiuni.append(DEV(str(stanga) ,str(dreapa)))
        
        
        return stanga 

    GenCod(NOD)
    Reg_rez = Lista_Instructiuni[len(Lista_Instructiuni) - 1][1]
    """
    Avem un caz special , in care in urma generari calculele ajung natural in registrul desitnatar , cea ce ne introduce doua instructiuni inutile 
        care mai au ca si consecinta suprascrierea rezultatului , exemplu : 
        Instructiune originala : 
        load R0 [R0 + ( 10 * 3 ) - ( SP + 10 )]

        Iar acesta este setul de instructiuni generate de algoritm : 
        push R0 
        sub R0 30 
        push SP 
        sub SP 10 
        sub R0 SP
        load R0 R0
        pop SP 
        pop R0

        Cea ce rezulta la final instructiunea :
        load R0 R0 
        pop R0 
        Problema este cu pop R0 , deoarece suprascrie ce se afla in R0 ce tocmai am calculat 

        Rezolvarea este simpla , verificam daca avem pe stiva registrul in care se afla rezultatul , daca da atunci nu mai facem pop la acel registru 
        si in tandem trebuie sa renuntam la push R0 in cazul de fata , deoarece creeam spatiu mort in STACK , si executam instrucituni inutile 
    """
    if Reg_rez == ORG[1]:
        #Cautam in stiva 
        if Reg_rez in Stiva:
            Stiva.pop(Stiva.index(Reg_rez))
        #Trebuie sa eliminam din lista de instructiuni push Reg_rez 
        if ["push" , Reg_rez] in Lista_Instructiuni:
            Lista_Instructiuni.pop(Lista_Instructiuni.index(["push" , Reg_rez]))

    ORG.append(Reg_rez)
    Lista_Instructiuni.append(ORG)

    while Stiva:
        Lista_Instructiuni.append( POP())

    return Lista_Instructiuni
    

#Procesarea de expansiuni 
def Procesare_Expansiuni(Lines: list):
        Extended = {
            "Extensie" : False,
            "Rezult": 0,
            "Extend": []
        }
        
        #Obtinem expansiunile 
        for line in Lines:
            expresie = []
            expr_start = 0 
            expr_end = 0
            instructiune = ""

            for i , Tok in enumerate(line):
                    if Tok == "[":
                        expr_start = i
                    elif Tok == "]":
                        expr_end = i
            
            expresie = line[expr_start : expr_end]
            instructiune = line[: expr_start] #Trebuie sa salvam instructiunea originala 
            AST = Parse(expresie)
            new_Ast = Proces_Expresion(AST)

            #Acum verificam daca ni sa intors un AST sau un rezultat 
            if isinstance(new_Ast , BinOP):
                Extended["Extensie"] = True
                Extended["Extend"] = Expand(new_Ast , instructiune)
            else:
                Extended["Extensie"] = False
                Extended["Rezult"] = new_Ast

        return Extended

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

#Testam Sistemul de expansiuni '
expresie = ["R0" , "+" ,"(" ,"10" , "*" , "3",")" , "-" , "(" ,"SP" , "+" , "10" , ")" ]
AST = Parse(expresie)
New =  Proces_Expresion(AST)
if isinstance(New , BinOP):
    afiseaza_ast(New)
    print(f"Expansiunea generata de algoritm : {Expand(New , ["load" , "R0"])}")
else:
    print(f"Expresiea evaluata : {New}")