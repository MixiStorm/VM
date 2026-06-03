#pragma once
#include"Memory.h"
#include"VM_Config.h"

class CPU;


class GPU{
private:
    //Pointer catre registri si catre memorie 
    Memory* mem;

    std::vector<uint64_t> * registri ;
    std::vector<uint64_t> * ROM ; 
    std::vector<uint64_t> * RAM ;
    std::vector<uint64_t> REG;
    CPU* cpu;

    //VRAMUl - gpu-ului
    std::vector<uint64_t> VRAM;

    bool Cpu_triger = false; //Aceasta variabila devine true doar daca apesi tasta D
    uint8_t GPU_MODE = 0 ;

public:
    GPU(Memory* mem);

    void Start();

    //void PrintRegistri(std::vector<uint64_t> registri);
private:
    //FUnctia care se ocupa cu partea de initializare a GPU-ului 
    bool GPU_INIT();    

    //Functia care se ocupa cu scrierea de text pe ecran 
    void TEXT_MODE_PRINT_TEXT();

    //Functia care verifica daca se poate citi din memorie sau nu 
    bool CanRead(){
        uint64_t Magic_NR = mem->Read_Memory(VM::GPU_IO);
        //printf("Magic numbre: 0x%06llx \n" , Magic_NR);
        if(Magic_NR == 0xFFAA) 
            return true;
        else
            return false; 
    }

};
