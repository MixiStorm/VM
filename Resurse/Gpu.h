#pragma once
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

    bool Cpu_triger = false; //Aceasta variabila devine true doar daca apesi tasta D

public:
    GPU(std::vector<uint64_t>* reg , std::vector<uint64_t>* rom , std::vector<uint64_t>* ram , CPU* cpu);

    void Start();

    void PrintRegistri(std::vector<uint64_t> registri);
    
};
