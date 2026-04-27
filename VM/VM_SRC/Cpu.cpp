#include"Cpu.h"

CPU::CPU(std::string ROM_NAME): ROM_FILE_NAME(ROM_NAME) , Registri(16 , 0), Memorie(ROM_NAME) ,
    Unitatea_Logica_Aritmetica(&Registri , &CPU_FLAG) {}

CPU::~CPU(){}


void CPU::DECODER(uint64_t & instructiune){
//Functia doar va decoada instrucitunea si va apela functiile necesare/modulele necesare pentru a executa instructiune 
        
        uint8_t opcode = static_cast<uint8_t>(instructiune & 0xFF); // Obtinem OpCodul din instructiune 
		uint8_t rx_1 = static_cast<uint8_t>((instructiune >> 8  ) & 0x0F); // Obtinem primul registru 
		uint8_t rx_2 = static_cast<uint8_t>((instructiune >> 12 ) & 0x0F); // Obtinem al doilea registru 
		uint8_t rx_3 = static_cast<uint8_t>((instructiune >> 16 ) & 0x0F); // Obtinem al treilea registru
		uint64_t imm = static_cast<uint64_t>(instructiune >> 20 ); // Obtinem datele/ adresele 
        bool JUMP = false;

        //Determinam de la inceput daca o sa incarcam datele in instructiune din imm sau din registri 
        bool Select_data = false ; //Daca este fals atunci vom folosi datele din registri altfel din immm
        if(imm != 0) Select_data = true;

        if(opcode == 0){
			std::cerr<<"Opcode == 0 , vlaoare invalida , procesul se opreste "<<std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(5));
			exit(100);
		}
        
        switch(static_cast<VM::OpCode>(opcode)){

            case VM::OpCode::Add:
                if(!Select_data)
                    Registri[rx_1] = Unitatea_Logica_Aritmetica.add(Registri[rx_1] ,Registri[rx_2] );
                else
                    Registri[rx_1] =  Unitatea_Logica_Aritmetica.add(Registri[rx_1] , imm );
                break;

            case VM::OpCode::SET:
				Registri[rx_1] = imm;
				break;

            case VM::OpCode::SUB:
                if(!Select_data)
                    Registri[rx_1] = Unitatea_Logica_Aritmetica.sub(Registri[rx_1] ,Registri[rx_2] );
                else
                    Registri[rx_1] =  Unitatea_Logica_Aritmetica.sub(Registri[rx_1] , imm );
                break;
            
            case VM::OpCode::MUL:
                if(!Select_data)
                    Registri[rx_1] = Unitatea_Logica_Aritmetica.mul(Registri[rx_1] ,Registri[rx_2] );
                else
                    Registri[rx_1] =  Unitatea_Logica_Aritmetica.mul(Registri[rx_1] , imm );
                break;

            case VM::OpCode::DIV:
                if(!Select_data)
                    Registri[rx_1] = Unitatea_Logica_Aritmetica.div(Registri[rx_1] ,Registri[rx_2] );
                else
                    Registri[rx_1] =  Unitatea_Logica_Aritmetica.div(Registri[rx_1] , imm );
                break;

            case VM::OpCode::MOV:
				Registri[rx_1] = Registri[rx_2];
				break;

            case VM::OpCode::CMP:
                if(!Select_data)
                    Unitatea_Logica_Aritmetica.sub(Registri[rx_1] ,Registri[rx_2]);
                else
                    Unitatea_Logica_Aritmetica.sub(Registri[rx_1] , imm);
                break;
            
            case VM::OpCode::STORE:
                if(!Select_data)
                    Memorie.Write_Memory(Registri[rx_1] , Registri[rx_2]);
                else
                    Memorie.Write_Memory(Registri[rx_1] , imm);
                break;
            
            case VM::OpCode::HALT:
                this->RUNING = false;
                break;

            case VM::OpCode::JMP:
                if(!Select_data)
                    Registri[VM::REG_PC] = Registri[rx_1];
                else
                    Registri[VM::REG_PC] = imm;
                JUMP = true;
                break;

            case VM::OpCode::JML:
                if(CPU_FLAG == - 1){
                    if(!Select_data)
                        Registri[VM::REG_PC] = Registri[rx_1];
                    else
                        Registri[VM::REG_PC] = imm;
                    JUMP = true;
                }
                break;
                    
            
            case VM::OpCode::JMM:
                if(CPU_FLAG == 1){
                    if(!Select_data)
                        Registri[VM::REG_PC] = Registri[rx_1];
                    else
                        Registri[VM::REG_PC] = imm;
                    JUMP = true;
                }
                break;

            case VM::OpCode::JMQ:
                if(CPU_FLAG == 0){
                    if(!Select_data)
                        Registri[VM::REG_PC] = Registri[rx_1];
                    else
                        Registri[VM::REG_PC] = imm;
                    JUMP = true;
                }
                break;

            case VM::OpCode::PUSH:
				Registri[VM::REG_SP] ++;
				if(!Select_data){
					Memorie.Write_Memory(Registri[VM::REG_SP] , Registri[rx_1]);
				}
                Memorie.Write_Memory(Registri[VM::REG_SP] , imm);
				break;

			case VM::OpCode::POP:
				if(Select_data){
					Registri[rx_1] = Memorie.Read_Memory( Registri[VM::REG_SP]);
				}

                Registri[VM::REG_SP] --;
                break;
            
            case VM::OpCode::NOP:
                break;
            
            case VM::OpCode::LOAD:
                if(!Select_data){
                    Registri[rx_1] = Memorie.Read_Memory(Registri[rx_2]);
                }
                else
                    Registri[rx_1] = Memorie.Read_Memory(imm);
                break;

            case VM::OpCode::ROM_Read:
				{
				    int j = 0;
                    std::cout<<"Vom scrie din ROM de la adresa : "<<Registri[rx_1]
                    <<" pana la adresa : "<<Registri[rx_2]<<" in RAM de la adresa : "<<Registri[rx_3]<<std::endl;
                    
                    for(size_t i = Registri[rx_1] ; i <= Registri[rx_2] ; i++){
                    
                        size_t adresa = i;
                        uint64_t data = Memorie.Read_Memory(adresa);
                    
                        Memorie.Write_Memory(Registri[rx_3] + j , Memorie.Read_Memory(adresa));
				    	j++;
				    }
                }
				break;

			case VM::OpCode::ROM_Write:
				{
				int j = 0 ;
				for(size_t i = Registri[rx_1] ; i <= Registri[rx_2] ; i++){
                    int addresa = Registri[rx_3] + j + VM::RAM_SIZE;
				    Memorie.Write_Memory( addresa ,  Memorie.Read_Memory(i));
					j++;
				}
				}
				break;
            
            case VM::OpCode::INC:
                if(!Select_data){
                    Registri[rx_1] = Unitatea_Logica_Aritmetica.inc(Registri[rx_1]);
                }
                else
                    Registri[rx_1] = Unitatea_Logica_Aritmetica.inc(imm);
                break;

            case VM::OpCode::DEC:
                if(!Select_data){
                    Registri[rx_1] = Unitatea_Logica_Aritmetica.dec(Registri[rx_1]);
                }
                else
                    Registri[rx_1] = Unitatea_Logica_Aritmetica.dec(imm);
                break;
            
            case VM::OpCode::XOR:
                Registri[rx_1] = Unitatea_Logica_Aritmetica.XOR(Registri[rx_2] , Registri[rx_2]);
                break;
                
            case VM::OpCode::OR:
                Registri[rx_1] = Unitatea_Logica_Aritmetica.OR(Registri[rx_2] , Registri[rx_2]);
                break;

            case VM::OpCode::AND:
                Registri[rx_1] = Unitatea_Logica_Aritmetica.AND(Registri[rx_2] , Registri[rx_2]);
                break;

            case VM::OpCode::NOT:
                Registri[rx_1] = Unitatea_Logica_Aritmetica.NOT(Registri[rx_2]);
                break;
            
            case VM::OpCode::SHL:
                Registri[rx_1] = Unitatea_Logica_Aritmetica.SHL(Registri[rx_2] , Registri[rx_2]);
                break;

            case VM::OpCode::SHR:
                Registri[rx_1] = Unitatea_Logica_Aritmetica.SHR(Registri[rx_2] , Registri[rx_2]);
                break;

            case VM::OpCode::CALL:
                JUMP = true; //Important  ca sa nu marim automat program counterul 

                Registri[VM::REG_SP]++;
                Memorie.Write_Memory(Registri[VM::REG_SP] ,Registri[VM::REG_PC] + 1  );
                if(!Select_data)    
                    Registri[VM::REG_PC] = Registri[rx_1];
                else
                    Registri[VM::REG_PC] = imm;
                
                break;
            case VM::OpCode::RET:
                JUMP = true; //Important  ca sa nu marim automat program counterul 
                Registri[VM::REG_PC] = Memorie.Read_Memory(Registri[VM::REG_SP]);

                Registri[VM::REG_SP]--;
                break;
                
            case VM::OpCode::MEM_LOCK:
                Memorie.SetLockedMemory(Registri[rx_1] , Registri[rx_2]);
                break;
            }   

        if(!JUMP)
            Registri[VM::REG_PC]++;

}


void CPU::Start(){
    Memorie.PrintMemory();
        
    while(this->RUNING){
        
        if(StepIn)
        {
            //Punem aceste functi in bloccul acesta de cod pentru a evita ca gpu-ul sa aceseze memoria cand 
            // Cpu face el insusi acces la memoria respectiva 
            //Prin punera de "{}" creeam un nou block de cod si atunci mutex iese din scop la finalizarea acestui block de cod 
            uint64_t instructiune;
            {
                std::lock_guard<std::mutex> lock(cpu_mutex);
                instructiune = Memorie.Read_Memory(Registri[VM::REG_PC]);
            }
            printf("Instructiune: 0x%016llx \n" , instructiune);
            DECODER(instructiune); //Cand face decode atunci face un pas inainte 
            //Memorie.PrintMemory();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    }
    printf("Programul sa oprit \n");
    Memorie.PrintMemory();
}