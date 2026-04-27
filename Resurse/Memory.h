#pragma once
#include"VM_Config.h"



class Memory{
private:
    //Definim global RAM-ul si ROM-mul pentru a putea avea acces la ele si din alte clase 
    std::vector<uint64_t> RAM;
    std::vector<uint64_t> ROM;

    //Numele catre fisierul ROM
    std::string ROM_FILE_NAME = "";


    struct MemLock{
        size_t start = 0; 
        size_t end   = 0;
    };

    std::vector<MemLock> LockedMem;

public:
//Declararea de constructor si deconstructor 
    Memory(std::string FileName);

    ~Memory();

    //Functii pentru a returna pointer la memorie 
    std::vector<uint64_t> * GetRom(){return &ROM;}
    std::vector<uint64_t> * GetRam(){return &RAM;}

    //Functie pentru a aaccesa memoria dupa adresa 
    uint64_t Read_Memory(uint64_t addres);

     //Functie pentru a scrie in memoria RAM
    void Write_Memory(uint64_t addres , uint64_t data);

    //Functie care seteaza un range de adrese de memorie invalide pentru scriere 
    void SetLockedMemory(uint64_t start , uint64_t end);

     //===================Functi-DEBUG==============================
    void PrintMemory();
private:
    //Functie pentru a incarca ROM-ul din fisier 
    void Load_Rom();

    //Functia de salvare in ROM
    void Save_Rom();

    //Functia care incarca din ROm in ram primele 512 biti / 8 poziti
	void Load_From_Rom_In_Ram_Start_UP();

     //Functie care scrie sau returneaza datele din memorie de la o adresa relativa 
    uint64_t Relativ_Adres(uint64_t adres , uint64_t data = 0 , bool ret = false );


};