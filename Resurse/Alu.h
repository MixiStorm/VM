#pragma once 
#include"VM_Config.h"


class ALU{
private:
    //Pointer catre toti registri 
    std::vector<uint64_t> * Registri = nullptr;
    int8_t* CPU_FLAG = nullptr;
public:
    ALU(std::vector<uint64_t>* Reg , int8_t* Flag):Registri(Reg) , CPU_FLAG(Flag){

    }


private:
    void Set_Falg(uint64_t a , uint64_t b){
        if(a == b)
            *CPU_FLAG = 0;
        else if(a < b)
            *CPU_FLAG = -1;
        else if(a > b)
            *CPU_FLAG = 1;
    }

public:
    uint64_t add(uint64_t a , uint64_t b){ Set_Falg(a , b); return a + b; }
    uint64_t sub(uint64_t a , uint64_t b){ Set_Falg(a , b); return a - b; }
    uint64_t mul(uint64_t a , uint64_t b){ Set_Falg(a , b); return a * b; }
    uint64_t div(uint64_t a , uint64_t b){ Set_Falg(a , b); return a / b; }
    uint64_t inc(uint64_t a){ return add(a , 1); }
    uint64_t dec(uint64_t a){ return sub(a , 1); }

    //Operatii pe biti 
    uint64_t XOR(uint64_t a , uint64_t b){ return a ^ b; }
    uint64_t AND(uint64_t a , uint64_t b){ return a & b; }
    uint64_t OR(uint64_t a , uint64_t b){ return a | b; }
    uint64_t NOT(uint64_t a ){ return ~a ; }
    uint64_t SHL(uint64_t a ,uint64_t val){ return a << val ; }
    uint64_t SHR(uint64_t a ,uint64_t val){ return a >> val ; }
};