#pragma once 
#include"VM_Config.h"
#include"Memory.h"
#include"Alu.h"
#include"Gpu.h"


class CPU{

private:
    //Zona de Managment 
    std::string ROM_FILE_NAME ;

    //RUNING 
    int RUNING = true;
    std::mutex cpu_mutex;

    bool StepIn = false; //Variabila pentru debug


private:
   
    //Vectorul care va reprezenta registri 
    std::vector<uint64_t> Registri;
   
    //Registrul pentru cpu flag 
    int8_t CPU_FLAG = 0;

    //Modulul care se ocupa de memorie 
    Memory Memorie;

    //Modulele logice : 
    ALU Unitatea_Logica_Aritmetica;

    //Modulul grafic 
    //GPU Unitatea_DE_Procesare_Grafica;

private:
    void DECODER(uint64_t & instructiune);

public: 
    CPU(std::string ROM_NAME);
    std::vector<uint64_t>* GetReg(){return & Registri;}
    Memory * GetMem(){return & Memorie;}

    ~CPU();
    void Start();
    std::mutex& GetMutex(){return cpu_mutex;}

    bool* GetStepIn(){return &StepIn;}

};

