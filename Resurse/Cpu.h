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
        Daca this->imm != 0 atunci vom performa calculele folosind this->imm , ignorand 
        ori ce alt registru specificat cu exceptia registrului principal , rx_1
*/
//Variabile care vor tine loc la parametri pentru performanta 
uint8_t rx_1 = 0;
uint8_t rx_2 = 0;
uint8_t rx_3 = 0;
uint64_t imm = 0;


void OP_ADD(){
    if (this->imm)
        Registri[this->rx_1] = Unitatea_Logica_Aritmetica.add(Registri[this->rx_1 ] , this->imm);
    else
        Registri[this->rx_1] = Unitatea_Logica_Aritmetica.add(Registri[this->rx_1 ] , Registri[this->rx_2]);
}
void OP_SET(){
    Registri[this->rx_1] = this->imm;
}

void OP_SUB(){
    if(!this->imm)
        Registri[this->rx_1] = Unitatea_Logica_Aritmetica.sub(Registri[this->rx_1] ,Registri[this->rx_2] );
    else
        Registri[this->rx_1] =  Unitatea_Logica_Aritmetica.sub(Registri[this->rx_1] , this->imm );
}

void OP_MUL( ){
     if(!this->imm)
            Registri[this->rx_1] = Unitatea_Logica_Aritmetica.mul(Registri[this->rx_1] ,Registri[this->rx_2] );
        else
            Registri[this->rx_1] =  Unitatea_Logica_Aritmetica.mul(Registri[this->rx_1] , this->imm );
}
void OP_DIV(){
    if(!this->imm)
        Registri[this->rx_1] = Unitatea_Logica_Aritmetica.div(Registri[this->rx_1] ,Registri[this->rx_2] );
    else
        Registri[this->rx_1] =  Unitatea_Logica_Aritmetica.div(Registri[this->rx_1] , this->imm );
}
void OP_MOV(){
    Registri[this->rx_1] = Registri[this->rx_2];
}
void OP_CMP(){
    if(!this->imm)
            Unitatea_Logica_Aritmetica.sub(Registri[this->rx_1] ,Registri[this->rx_2]);
        else
            Unitatea_Logica_Aritmetica.sub(Registri[this->rx_1] , this->imm);
}
void OP_STORE(){
    if(!this->imm)
            Memorie->Write_Memory(Registri[this->rx_1] , Registri[this->rx_2]);
        else
            Memorie->Write_Memory(Registri[this->rx_1] , this->imm);
}

void OP_HALT(){
    this->RUNING = false;
}

void OP_JMP( ){
     if(!this->imm)
            Registri[VM::REG_PC] = Registri[this->rx_1];
        else
            Registri[VM::REG_PC] = this->imm;
        this->JUMP = true;
}
};

