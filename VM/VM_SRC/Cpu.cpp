#include"Cpu.h"

CPU::CPU(Memory* mem , Intrerupere* INT): Memorie(mem) ,SysIntreruperi(INT)   , Registri(16 , 0) ,
    Unitatea_Logica_Aritmetica(&Registri , &CPU_FLAG) {}

CPU::~CPU(){}

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
        
        switch(static_cast<VM::OpCode>(opcode)){

            case VM::OpCode::Add:
                if(!this->imm)
                    Registri[this->rx_1] = Unitatea_Logica_Aritmetica.add(Registri[this->rx_1] ,Registri[this->rx_2] );
                else
                    Registri[this->rx_1] =  Unitatea_Logica_Aritmetica.add(Registri[this->rx_1] , this->imm );
                break;

            case VM::OpCode::SET:
				Registri[this->rx_1] = this->imm;
				break;

            case VM::OpCode::SUB:
                if(!this->imm)
                    Registri[this->rx_1] = Unitatea_Logica_Aritmetica.sub(Registri[this->rx_1] ,Registri[this->rx_2] );
                else
                    Registri[this->rx_1] =  Unitatea_Logica_Aritmetica.sub(Registri[this->rx_1] , this->imm );
                break;
            
            case VM::OpCode::MUL:
                if(!this->imm)
                    Registri[this->rx_1] = Unitatea_Logica_Aritmetica.mul(Registri[this->rx_1] ,Registri[this->rx_2] );
                else
                    Registri[this->rx_1] =  Unitatea_Logica_Aritmetica.mul(Registri[this->rx_1] , this->imm );
                break;

            case VM::OpCode::DIV:
                if(!this->imm)
                    Registri[this->rx_1] = Unitatea_Logica_Aritmetica.div(Registri[this->rx_1] ,Registri[this->rx_2] );
                else
                    Registri[this->rx_1] =  Unitatea_Logica_Aritmetica.div(Registri[this->rx_1] , this->imm );
                break;

            case VM::OpCode::MOV:
				Registri[this->rx_1] = Registri[this->rx_2];
				break;

            case VM::OpCode::CMP:
                if(!this->imm)
                    Unitatea_Logica_Aritmetica.sub(Registri[this->rx_1] ,Registri[this->rx_2]);
                else
                    Unitatea_Logica_Aritmetica.sub(Registri[this->rx_1] , this->imm);
                break;
            
            case VM::OpCode::STORE:
                if(!this->imm)
                    Memorie->Write_Memory(Registri[this->rx_1] , Registri[this->rx_2]);
                else
                    Memorie->Write_Memory(Registri[this->rx_1] , this->imm);
                break;
            
            case VM::OpCode::HALT:
                this->RUNING = false;
                break;

            case VM::OpCode::JMP:
                if(!this->imm)
                    Registri[VM::REG_PC] = Registri[this->rx_1];
                else
                    Registri[VM::REG_PC] = this->imm;
                this->JUMP = true;
                break;

            case VM::OpCode::JML:
                if(CPU_FLAG == - 1){
                    if(!this->imm)
                        Registri[VM::REG_PC] = Registri[this->rx_1];
                    else
                        Registri[VM::REG_PC] = this->imm;
                    this->JUMP = true;
                }
                break;
                    
            
            case VM::OpCode::JMM:
                if(CPU_FLAG == 1){
                    if(!this->imm)
                        Registri[VM::REG_PC] = Registri[this->rx_1];
                    else
                        Registri[VM::REG_PC] = this->imm;
                    this->JUMP = true;
                }
                break;

            case VM::OpCode::JMQ:
                if(CPU_FLAG == 0){
                    if(!this->imm)
                        Registri[VM::REG_PC] = Registri[this->rx_1];
                    else
                        Registri[VM::REG_PC] = this->imm;
                    this->JUMP = true;
                }
                break;

            case VM::OpCode::PUSH:
				Registri[VM::REG_SP] ++;
				if(!this->imm){
					Memorie->Write_Memory(Registri[VM::REG_SP] , Registri[this->rx_1] );
                    break;
				}
                Memorie->Write_Memory(Registri[VM::REG_SP] , this->imm );
				break;

			case VM::OpCode::POP:
				if(this->imm){
					Registri[this->rx_1] = Memorie->Read_Memory( Registri[VM::REG_SP]);
				}

                Registri[VM::REG_SP] --;
                break;
            
            case VM::OpCode::NOP:
                break;
            
            case VM::OpCode::LOAD:
                if(!this->imm){
                    Registri[this->rx_1] = Memorie->Read_Memory(Registri[this->rx_2]);
                }
                else
                    Registri[this->rx_1] = Memorie->Read_Memory(this->imm);
                break;

            case VM::OpCode::ROM_Read:
				{
				    int j = 0;
                    std::cout<<"Vom scrie din ROM de la adresa : "<<Registri[this->rx_1]
                    <<" pana la adresa : "<<Registri[this->rx_2]<<" in RAM de la adresa : "<<Registri[this->rx_3]<<std::endl;
                    
                    for(size_t i = Registri[this->rx_1] ; i <= Registri[this->rx_2] ; i++){
                    
                        size_t adresa = i;
                        uint64_t data = Memorie->Read_Memory(adresa);
                    
                        Memorie->Write_Memory(Registri[this->rx_3] + j , Memorie->Read_Memory(adresa));
				    	j++;
				    }
                    Memorie->PrintMemory();
                }
				break;

			case VM::OpCode::ROM_Write:
				{
				int j = 0 ;
				for(size_t i = Registri[this->rx_1] ; i <= Registri[this->rx_2] ; i++){
                    int addresa = Registri[this->rx_3] + j + VM::RAM_SIZE;
				    Memorie->Write_Memory( addresa ,  Memorie->Read_Memory(i));
					j++;
				}
				}
				break;
            
            case VM::OpCode::INC:
                if(!this->imm){
                    Registri[this->rx_1] = Unitatea_Logica_Aritmetica.inc(Registri[this->rx_1]);
                }
                else
                    Registri[this->rx_1] = Unitatea_Logica_Aritmetica.inc(this->imm);
                break;

            case VM::OpCode::DEC:
                if(!this->imm){
                    Registri[this->rx_1] = Unitatea_Logica_Aritmetica.dec(Registri[this->rx_1]);
                }
                else
                    Registri[this->rx_1] = Unitatea_Logica_Aritmetica.dec(this->imm);
                break;
            
            case VM::OpCode::XOR:
                Registri[this->rx_1] = Unitatea_Logica_Aritmetica.XOR(Registri[this->rx_2] , Registri[this->rx_2]);
                break;
                
            case VM::OpCode::OR:
                Registri[this->rx_1] = Unitatea_Logica_Aritmetica.OR(Registri[this->rx_2] , Registri[this->rx_2]);
                break;

            case VM::OpCode::AND:
                Registri[this->rx_1] = Unitatea_Logica_Aritmetica.AND(Registri[this->rx_2] , Registri[this->rx_2]);
                break;

            case VM::OpCode::NOT:
                Registri[this->rx_1] = Unitatea_Logica_Aritmetica.NOT(Registri[this->rx_2]);
                break;
            
            case VM::OpCode::SHL:
                Registri[this->rx_1] = Unitatea_Logica_Aritmetica.SHL(Registri[this->rx_2] , Registri[this->rx_2]);
                break;

            case VM::OpCode::SHR:
                Registri[this->rx_1] = Unitatea_Logica_Aritmetica.SHR(Registri[this->rx_2] , Registri[this->rx_2]);
                break;

            case VM::OpCode::CALL:
                this->JUMP = true; //Important  ca sa nu marim automat program counterul 

                Registri[VM::REG_SP]++;
                Memorie->Write_Memory(Registri[VM::REG_SP] ,Registri[VM::REG_PC] + 1  );
                if(!this->imm)    
                    Registri[VM::REG_PC] = Registri[this->rx_1];
                else
                    Registri[VM::REG_PC] = this->imm;
                
                break;
            case VM::OpCode::RET:
                this->JUMP = true; //Important  ca sa nu marim automat program counterul 
                Registri[VM::REG_PC] = Memorie->Read_Memory(Registri[VM::REG_SP]);

                Registri[VM::REG_SP]--;
                break;
                
            case VM::OpCode::MEM_LOCK:
                Memorie->SetLockedMemory(Registri[this->rx_1] , Registri[this->rx_2]);
                break;
            }   

        if(!this->JUMP)
            Registri[VM::REG_PC]++;

}


void CPU::Start(){
    Memorie->PrintMemory();
        
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

        std::this_thread::sleep_for(std::chrono::microseconds(1000));

    }
    printf("Programul sa oprit \n");
    Memorie->PrintMemory();
}