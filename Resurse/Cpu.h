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

    //DEBUG
    bool StepIn = true; //Variabila pentru debug
    struct STATE{
        uint64_t Imm;
        uint64_t Instructiune;
        uint8_t Opcode ;
        uint8_t Rx1;
        uint8_t Rx2;
        uint8_t Rx3;
        std::vector<uint64_t>REG;
    };
    std::vector<STATE> CPU_STATE;
    int MAX_HISTORY = 50;


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
            Memorie->Write_Memory(Registri[this->rx_1] , Registri[this->rx_2] , "CPU");
        else
            Memorie->Write_Memory(Registri[this->rx_1] , this->imm , "CPU");
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
void OP_JML(){
   if(CPU_FLAG == - 1){
        if(!this->imm)
            Registri[VM::REG_PC] = Registri[this->rx_1];
        else
            Registri[VM::REG_PC] = this->imm;
        this->JUMP = true;
    }
}
void OP_JMM(){
    if(this->CPU_FLAG == 1){
        if(!this->imm)
            Registri[VM::REG_PC] = Registri[this->rx_1];
        else
            Registri[VM::REG_PC] = this->imm;
        this->JUMP = true;
    }
}
void OP_JMQ(){
    if(this->CPU_FLAG == 0){
        if(!this->imm)
            Registri[VM::REG_PC] = Registri[this->rx_1];
        else
            Registri[VM::REG_PC] = this->imm;
        this->JUMP = true;
    }
}

void OP_PUSH(){
    Registri[VM::REG_SP] ++;
	if(!this->imm)
		Memorie->Write_Memory(Registri[VM::REG_SP] , Registri[this->rx_1] ,"CPU"); 
    else
        Memorie->Write_Memory(Registri[VM::REG_SP] , this->imm ,"CPU");
}

void OP_POP(){
    if(this->imm)
	    Registri[this->rx_1] = Memorie->Read_Memory( Registri[VM::REG_SP],"CPU");
	
    Registri[VM::REG_SP] --;
}
void OP_NOP(){
    return;
}

void OP_LOAD(){
    if(!this->imm)
        Registri[this->rx_1] = Memorie->Read_Memory(Registri[this->rx_2] , "CPU");
    else
        Registri[this->rx_1] = Memorie->Read_Memory(this->imm , "CPU");
}

void OP_ROM_READ(){
    int j = 0;
    std::cout<<"Vom scrie din ROM de la adresa : "<<Registri[this->rx_1]
    <<" pana la adresa : "<<Registri[this->rx_2]<<" in RAM de la adresa : "<<Registri[this->rx_3]<<std::endl;
    
    for(size_t i = Registri[this->rx_1] ; i <= Registri[this->rx_2] ; i++){
    
        size_t adresa = i;
        uint64_t data = Memorie->Read_Memory(adresa, "CPU");
    
        Memorie->Write_Memory(Registri[this->rx_3] + j , Memorie->Read_Memory(adresa,  "CPU") , "CPU");
		j++;
	}
    Memorie->PrintMemory();
}

void OP_ROM_WRITE(){
    int j = 0 ;
	for(size_t i = Registri[this->rx_1] ; i <= Registri[this->rx_2] ; i++){
        int addresa = Registri[this->rx_3] + j + VM::RAM_SIZE;
	    Memorie->Write_Memory( addresa ,  Memorie->Read_Memory(i , "CPU") , "CPU");
		j++;
	}
}

void OP_INC(){
    if(!this->imm)
        Registri[this->rx_1] = Unitatea_Logica_Aritmetica.inc(Registri[this->rx_1]);
    else
        Registri[this->rx_1] = Unitatea_Logica_Aritmetica.inc(this->imm);
}

void OP_DEC(){
    if(!this->imm)
        Registri[this->rx_1] = Unitatea_Logica_Aritmetica.dec(Registri[this->rx_1]);
    else
        Registri[this->rx_1] = Unitatea_Logica_Aritmetica.dec(this->imm);
}

 void OP_XOR(){
    Registri[this->rx_1] = Unitatea_Logica_Aritmetica.XOR(Registri[this->rx_1] , Registri[this->rx_2]);
}            
void OP_OR(){
    Registri[this->rx_1] = Unitatea_Logica_Aritmetica.OR(Registri[this->rx_1] , Registri[this->rx_2]);
}
void OP_AND(){
    Registri[this->rx_1] = Unitatea_Logica_Aritmetica.AND(Registri[this->rx_1] , Registri[this->rx_2]);
}
void OP_NOT(){
    Registri[this->rx_1] = Unitatea_Logica_Aritmetica.NOT(Registri[this->rx_1]);
}
void OP_SHL(){
    Registri[this->rx_1] = Unitatea_Logica_Aritmetica.SHL(Registri[this->rx_1] , Registri[this->rx_2]);
}
void OP_SHR(){
    Registri[this->rx_1] = Unitatea_Logica_Aritmetica.SHR(Registri[this->rx_1] , Registri[this->rx_2]);
}
void OP_CALL(){
    this->JUMP = true; //Important  ca sa nu marim automat program counterul 
    Registri[VM::REG_SP]++;
    Memorie->Write_Memory(Registri[VM::REG_SP] ,Registri[VM::REG_PC] + 1  , "CPU");
    if(!this->imm)    
        Registri[VM::REG_PC] = Registri[this->rx_1];
    else
        Registri[VM::REG_PC] = this->imm;
}
void OP_RET(){
    this->JUMP = true; //Important  ca sa nu marim automat program counterul 
    Registri[VM::REG_PC] = Memorie->Read_Memory(Registri[VM::REG_SP] , "CPU");
    Registri[VM::REG_SP]--;
}            
void OP_MEM_LOCK(){
                Memorie->SetLockedMemory(Registri[this->rx_1] , Registri[this->rx_2]);
}   

private:
//Lista cu functi 
using OpHandler = void(CPU::*)();
static const OpHandler OP_TABLE[256];

};


