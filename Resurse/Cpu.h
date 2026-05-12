#pragma once 
#include"VM_Config.h"
#include"Memory.h"
#include"Alu.h"
#include"Gpu.h"
#include"Intreruperi.h"


class CPU{

private:
    //Zona de Managment 
    std::string ROM_FILE_NAME ;

    //RUNING 
    int RUNING = true;
    std::mutex cpu_mutex;

    bool StepIn = true; //Variabila pentru debug

private:
   
    //Vectorul care va reprezenta registri 
    std::vector<uint64_t> Registri;
   
    //Registrul pentru cpu flag 
    int8_t CPU_FLAG = 0;

    //Modulul care se ocupa de memorie 
    Memory* Memorie;

    //Modulele logice : 
    ALU Unitatea_Logica_Aritmetica;

    Intrerupere * SysIntreruperi;

private:
    void DECODER(uint64_t & instructiune);

    void INT_RUTIN();

public: 
    CPU(Memory* mem  , Intrerupere* INT);
    std::vector<uint64_t>* GetReg(){return & Registri;}

    ~CPU();
    void Start();
    std::mutex& GetMutex(){return cpu_mutex;}

    bool* GetStepIn(){return &StepIn;}

};

