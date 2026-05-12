#pragma once
#include"VM_Config.h"



class Intrerupere{
    
    bool IsIntrerupere = false;
    uint64_t ID_Intrerupere = 0x0;

public:
    Intrerupere();

    //Verifica daca exista o intrerupere     
    bool CheckIntrerupere();
    
    //Seteaza Intreruperea 
    bool SetIntrerupere(uint64_t CodIntrerupere);

    //Obtine codul pentru Intrerupere
    uint64_t GetIntrerupere();

    //Functia care va dezactiva flagul pentru intreruperi 
    void DisableIntreruperi();

};

/*
    Sitemul de intreruperi: 

    Incepand de la adresa : VM::INT_TABEL se afla un tabel unde sa gasesc adresele la care 
    va sari procesorul pentru a gestiona intreruperea curenta . In tabela se va specifica o adresa
    specifica unde este un cod scriis de catre programator ce va gestiona intreruperile sale specifice 
    care pot sa fie atat softwar cat si hardware , dar vom defini din start anumite ID-uri hardware, 
    Aceste ID-uri de intreruperi sunt dedicate pentru dispozitivele hardware , pe care le poti modifica 
    fara probleme din source_codul fiecarei componente imparte , dar dedic urmatoarele coduri : 
        0x1 - GPU
        0x2 - KeyBoard
    
    Pentru inceput am sa incerc sa creez o librarie care sa cotina tot codul necesar pentru gestionarea 
    intreruperilelor hardware.

    Alt lucru important de mentionat : 
        ->Cand procesorul detecteaza o intrerupere va incarca pe stack contextul lui actual , mai exact 
        va incarca pe stack toti registri sai impreuna cu CPU_FLAG. Dupa ce se termina gestionarea unei 
        intreruperi se va utiliza instructiunea IRET (interupt return) , care va folosi EBP pentru a se uita 
        in stack de la ultima pozitie in sens invers pentru a pune inapoi contextul registrilor inapoi in CPU 
        , folosim EBP pentru ca presupunem ca programatorul va utiliza SP intr-un mod sau altul care ar putea duce 
        la erori . Asa ca pentru a putea utiliza IRET va trebui ca la inceputul fiecarei gestionari de intrerupere 
        sa setam EBP = SP , si sa promitem ca nu ne mai atingem de EBP pe urma , pentru ca altfel instructiunea 
        IRET va incarca gunoi inapoi in CPU cea ce va duce la diverse erori nedefinite . Sau poti utiliza EBP dar 
        sa ai grija sa se intoarca mereu la pozitia sa curenta . Evident acestea sunt doar sfaturi pentru ca in loc 
        de IRET se poate flosi la fel de bine si CALL si RET , doar ca trebuie sa fii foarte atent sa nu pierzi adresa de 
        intoarcere , si pe langa asta trebuie sa te ocupi manual de incarcarea tuturor datelor inapoi in registri prin cod 
        LOAD PC SP 
        dec SP 
        LOAD SP SP
        dec SP 
        .......... 

*/
