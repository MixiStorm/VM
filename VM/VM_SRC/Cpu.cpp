#include"Cpu.h"

CPU::CPU(Memory* mem , Intrerupere* INT): Memorie(mem) ,SysIntreruperi(INT)   , Registri(16 , 0) ,
    Unitatea_Logica_Aritmetica(&Registri , &CPU_FLAG) {}

CPU::~CPU(){}
//Adaugam functiile pentru instructiuni 

// Cpu.cpp - după definiția funcțiilor
const CPU::OpHandler CPU::OP_TABLE[256] = {
    nullptr,                     // 0x00
    &CPU::OP_ADD,                // 0x01
    &CPU::OP_SUB,                // 0x02
    &CPU::OP_CMP,                // 0x03
    &CPU::OP_SET,                // 0x04
    &CPU::OP_LOAD,               // 0x05
    &CPU::OP_STORE,              // 0x06
    &CPU::OP_HALT,               // 0x07
    &CPU::OP_ROM_READ,           // 0x08
    &CPU::OP_ROM_WRITE,          // 0x09
    &CPU::OP_MOV,                // 0x0A
    &CPU::OP_PUSH,               // 0x0B
    &CPU::OP_POP,                // 0x0C
    &CPU::OP_NOP,                // 0x0D
    nullptr,                     // 0x0E
    &CPU::OP_JMP,                // 0x0F
    &CPU::OP_JMQ,                // 0x10  // Dacă mai e folosit
    &CPU::OP_JML,                // 0x11
    &CPU::OP_JMM,                // 0x12
    &CPU::OP_MUL,                // 0x13
    &CPU::OP_DIV,                // 0x14
    &CPU::OP_INC,                // 0x15
    &CPU::OP_DEC,                // 0x16
    &CPU::OP_XOR,                // 0x17
    &CPU::OP_AND,                // 0x18
    &CPU::OP_OR,                 // 0x19
    &CPU::OP_NOT,                // 0x1A
    &CPU::OP_SHL,                // 0x1B
    &CPU::OP_SHR,                // 0x1C
    &CPU::OP_CALL,               // 0x1D
    &CPU::OP_RET,                // 0x1E
    &CPU::OP_MEM_LOCK,           // 0x1F
};


void CPU::INT_RUTIN(){
    uint64_t INT_ID = SysIntreruperi->GetIntrerupere();

    auto PUSH = [&](uint64_t val){
        Registri[VM::REG_SP] ++;
		Memorie->Write_Memory(Registri[VM::REG_SP] , val );	
    };
    //Dupa ce obtinem codul de interupere trebuie sa salvam toate registrele pe stiva 
    for(auto& reg : Registri)
        PUSH(reg);
    PUSH(CPU_FLAG);            
    
    //Acum procesorul trebuie sa sara la tabela din memorie care ii spune unde se afla codul ce gestioneaza intreruperile 
    uint64_t JUMP_TO = Memorie->Read_Memory(INT_ID + VM::INT_TABEL);
    
    Registri[VM::REG_PC] = JUMP_TO;

    //Dezactivam flagul de intreruperi pentru ca acum gestionam intreruperea curenta 
    SysIntreruperi->DisableIntreruperi();

}

void CPU::DECODER(uint64_t & instructiune){
//Functia doar va decoada instrucitunea si va apela functiile necesare/modulele necesare pentru a executa instructiune 
        
        uint8_t opcode = static_cast<uint8_t>(instructiune & 0xFF); // Obtinem OpCodul din instructiune 
		this->rx_1 = static_cast<uint8_t>((instructiune >> 8  ) & 0x0F); // Obtinem primul registru 
		this->rx_2 = static_cast<uint8_t>((instructiune >> 12 ) & 0x0F); // Obtinem al doilea registru 
		this->rx_3 = static_cast<uint8_t>((instructiune >> 16 ) & 0x0F); // Obtinem al treilea registru
        this->imm = static_cast<uint64_t>(instructiune >> 20 ); // Obtinem datele/ adresele 
        this->JUMP = false;


        if(opcode == 0){
			std::cerr<<"Opcode == 0 , vlaoare invalida , procesul se opreste "<<std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(5));
			exit(100);
		}
        //printf("Instructiune : 0x%016llx   | PC : %ld  | SP : %ld \n " , instructiune , Registri[VM::REG_PC] ,  Registri[VM::REG_SP]);

        (this->*OP_TABLE[opcode])();
     
        if(!this->JUMP)
            Registri[VM::REG_PC]++;

}


void CPU::Start(){
    Memorie->PrintMemory();
    auto start = std::chrono::high_resolution_clock::now();

    while(this->RUNING){

        //Daca detectam o intrerupere incepem gestionarea sa 
        if(SysIntreruperi->CheckIntrerupere()){
            INT_RUTIN();
        }

        if(StepIn)
        {
            uint64_t instructiune = Memorie->Read_Memory(Registri[VM::REG_PC]);
            DECODER(instructiune); //Cand face decode atunci face un pas inainte 
            //Memorie.PrintMemory();
        }

        //std::this_thread::sleep_for(std::chrono::microseconds(1000));

    }
     auto end = std::chrono::high_resolution_clock::now();
    auto durata = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout<<"Timp total de executie: "<<durata.count()<<" qs"<<std::endl;

    printf("Programul sa oprit \n");
    Memorie->PrintMemory();
}