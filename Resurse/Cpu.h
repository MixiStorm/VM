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
    bool JUMP = false;
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

private:
/*
    Aici se vor afla functiile pentru fiecare instructiune inparte 

    Important !!!!
        Daca imm != 0 atunci vom performa calculele folosind imm , ignorand 
        ori ce alt registru specificat cu exceptia registrului principal , RX1
*/

void OP_ADD(uint64_t rx1 , uint64_t rx2 , uint64_t imm ){
    if (imm)
        Registri[rx1] = Unitatea_Logica_Aritmetica.add(Registri[rx1 ] , imm);
    else
        Registri[rx1] = Unitatea_Logica_Aritmetica.add(Registri[rx1 ] , Registri[rx2]);
}
void OP_SET(uint64_t rx1 , uint64_t imm){
    Registri[rx1] = imm;
}

void OP_SUB(uint64_t rx1 , uint64_t rx2 , uint64_t imm ){
    if(!imm)
        Registri[rx1] = Unitatea_Logica_Aritmetica.sub(Registri[rx1] ,Registri[rx2] );
    else
        Registri[rx1] =  Unitatea_Logica_Aritmetica.sub(Registri[rx1] , imm );
}

void OP_MUL(uint64_t rx1 , uint64_t rx2 , uint64_t imm ){
     if(!imm)
            Registri[rx1] = Unitatea_Logica_Aritmetica.mul(Registri[rx1] ,Registri[rx2] );
        else
            Registri[rx1] =  Unitatea_Logica_Aritmetica.mul(Registri[rx1] , imm );
}
void OP_DIV(uint64_t rx_1 , uint64_t rx_2 , uint64_t imm){
    if(!imm)
        Registri[rx_1] = Unitatea_Logica_Aritmetica.div(Registri[rx_1] ,Registri[rx_2] );
    else
        Registri[rx_1] =  Unitatea_Logica_Aritmetica.div(Registri[rx_1] , imm );
}
void OP_MOV(uint64_t rx_1 , uint64_t rx_2){
    Registri[rx_1] = Registri[rx_2];
}
void OP_CMP(uint64_t rx_1 , uint64_t rx_2 , uint64_t imm){
    if(!imm)
            Unitatea_Logica_Aritmetica.sub(Registri[rx_1] ,Registri[rx_2]);
        else
            Unitatea_Logica_Aritmetica.sub(Registri[rx_1] , imm);
}
void OP_STORE(uint64_t rx_1 , uint64_t rx_2 , uint64_t imm){
    if(!imm)
            Memorie->Write_Memory(Registri[rx_1] , Registri[rx_2]);
        else
            Memorie->Write_Memory(Registri[rx_1] , imm);
}

void OP_HALT(){
    this->RUNING = false;
}

void OP_JMP(uint64_t rx_1 , uint64_t imm){
     if(!imm)
            Registri[VM::REG_PC] = Registri[rx_1];
        else
            Registri[VM::REG_PC] = imm;
        this->JUMP = true;
}
};

