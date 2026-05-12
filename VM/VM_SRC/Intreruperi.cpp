#include"Intreruperi.h"


Intrerupere::Intrerupere(){}

bool Intrerupere::CheckIntrerupere(){return IsIntrerupere;}

bool Intrerupere::SetIntrerupere(uint64_t CodIntrerupere){ 
    if(!IsIntrerupere){    
        this->ID_Intrerupere = CodIntrerupere; 
        IsIntrerupere = true;
        return true;
    }
    return false;
}

uint64_t Intrerupere::GetIntrerupere(){return ID_Intrerupere;}

void Intrerupere::DisableIntreruperi(){ IsIntrerupere = false;}