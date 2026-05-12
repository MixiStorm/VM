#pragma once
#include"Memory.h"
#include"VM_Config.h"

class CPU;


class GPU{
private:
    //Pointer catre registri si catre memorie 

    std::vector<uint64_t> * registri ;
    std::vector<uint64_t> * ROM ; 
    std::vector<uint64_t> * RAM ;
    CPU* cpu;

    std::vector<uint64_t> REG;
    Memory* mem;

    bool Cpu_triger = false; //Aceasta variabila devine true doar daca apesi tasta D

public:
    GPU(Memory* mem);

    void Start();

    //void PrintRegistri(std::vector<uint64_t> registri);
    
};
