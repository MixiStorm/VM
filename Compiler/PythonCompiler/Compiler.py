import os , Setings 
import Preprocesare_cod as PreCod
import Optimizari 
import Linker

def PrintTokens(Tokens):
    #Afisam codul preprocesat asa cum apare in source_code 
    print()
    for line in Tokens:
        for t in line:
            print(t , end= " ")
        print()

def Print_Var():
    print("Variabile : ")
    for Var_Name , detali in Setings.Variabile["var"].items():
        print(f"   Name: {Var_Name} \
                \n   Size : {detali["dimension"]}\
                \n   Value: {detali["value"]}\
                \n   Addr: {detali["addr"]}\
                \n")
    
    print("Arrys : ")
    for Var_Name , detali in Setings.Variabile["arr"].items():
        print(f"   Name: {Var_Name} \
                \n   Size : {detali["dimension"]}\
                \n   Value: {detali["value"]}\
                \n   Addr: {detali["addr"]}\
                \n")

"""
        ========================PIPLINE==========================
        1. Citim fisierul sursa , si il stocam intr-o lista linie cu linie 
        2. Preprocesarea 
            -Acest stagiu citeste codul sursa inlocuind toate directivele define 
            si include codul din zona de incluziuni in codul sursa 
        3. Tokensizare :
            Preaia codul preprocesat si il transforma in TOKENS 
        4. Procesarea de Labels si variabile
                -Este cam o minciuna deoarece in acest stagiu 
                identificam toate labeurile si le punem intrun tabel de labels.
                -Aici se identifica variabilele si se inocuieste numele lor 
                cu adresele lor in source code 
        5.Preocesare_Expansiuni 
                -In acest stagiu identificam expresiile din cod ex : load R0 [SP - 1]
                si le procesam , generand unde este necesar instructiuni noi 
                sau inlocuind expresia cu valoarea sa calculata 
        6. Linking 
                -A sosit momentul pentru a calcula toate labeurile si asezarea codului 
                urmand sa fie inlocuite toate labeurile cu adresele lor specifice 
        7. ToBin :
            -Acest stagiu este stagiul care converteste codul sursa in binar executabil de 
            catre VM  
"""


#========FORNT_END=============
#Citim prima data fisierul 
Source_Code = PreCod.ReadFile(Setings.Source_Code_Path)
#Preprocesam codul sursa 
Preprocesed_Source_Code = PreCod.Preprocesare(Source_Code)
#Tokwnizam codul preprocesat 
Tokens = PreCod.Tokenizare(Preprocesed_Source_Code )
#Afisam toate labelurile 
Tokens = PreCod.Procesare_vars(Tokens)

Tokens = Optimizari.Procesare_Expansiuni(Tokens)


LK = Linker.Linker(Tokens)
LK.Start()
#print(f"DEBUG : \n Vars : {Setings.Variabile} \n\n Labels : \n {Setings.LabelsDict}")
#PrintTokens(Tokens)