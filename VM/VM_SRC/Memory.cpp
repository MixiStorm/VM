#include"Memory.h"
#include"VM_Config.h"

//#include<string>


Memory::Memory(std::string FileName ):RAM(VM::RAM_SIZE) , ROM(VM::ROM_SIZE) , ROM_FILE_NAME(FileName){

        //Incarcam datele din fisier in ROM 
        Load_Rom();

        //Incarcam primele 8 instruciuni in RAM din ROM
        Load_From_Rom_In_Ram_Start_UP();
        if(ROM_FILE_NAME == "")
            std::cout<<"Nu sa introdus numele fisierului ROM"<<std::endl;
}

Memory::~Memory(){
    Save_Rom();
}

void Memory::Load_Rom(){
    std::ifstream file(ROM_FILE_NAME , std::ios::binary);
	if(file.is_open()){
		uint64_t data ;
		int i = 0;

		while(file.read(reinterpret_cast<char*>(&data) , sizeof(uint64_t)) && i < VM::ROM_SIZE){
			ROM[i++] = data;
		}
		file.close();
	}
}

void Memory::Save_Rom(){
    std::ofstream file(ROM_FILE_NAME , std::ios::binary);
	int last_val_pos = 0;
	for(int i = 0 ; i < ROM.size() ; i++)
		if(ROM[i] != 0)
			last_val_pos = i;
	if(file.is_open()){
		for(int i = 0 ; i <= last_val_pos ; i++){
				file.write(reinterpret_cast<char*>(&ROM[i]) , sizeof(uint64_t));
		}
		file.close();
	}    
}

void Memory::Load_From_Rom_In_Ram_Start_UP(){
    int Block_Size = 1000;
	for(int i = 0 ; i < Block_Size ; i++)
		RAM[i] = ROM[i];
}

uint64_t Memory::Relativ_Adres(uint64_t adres , uint64_t data , bool ret ){

    if(!ret){
        if(adres >= VM::RAM_SIZE)
            ROM[adres - VM::RAM_SIZE] = data;
        else
            RAM[adres] = data;
    }
    else
        if(adres >= VM::RAM_SIZE)
            return ROM[adres - VM::RAM_SIZE];
        else
            return RAM[adres];
    return 0;
}

uint64_t  Memory::Read_Memory(uint64_t addres){
    //Extrem de imporatnt de lucrat pe aceasta functie 
    return Relativ_Adres(addres , 0 , 1);
}

//Functie pentru a scrie in memoria RAM
void Memory::Write_Memory(uint64_t addres , uint64_t data){

    //Trebuie sa verificam daca adresa la cre vrem sa scriem este blocata sau nu 
    if(!this->LockedMem.empty())
    {
        for(size_t i = 0 ; i < this->LockedMem.size() ; i++){
            if(addres >= LockedMem[i].start && addres <= LockedMem[i].end)
            {
                std::cout<<"[ERROR] Trying to write to an invalid adres space : "<<addres<<" core dumped"<<std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(5));
                exit(0xDEAD);
            }
        }
    }

    //De adaugat dinou functionalitati in aceasta functie 
    Relativ_Adres(addres , data , 0);
}

void Memory::SetLockedMemory(uint64_t adr_start , uint64_t adr_end){
    MemLock lock;
    if(adr_start < adr_end){
        lock.start = adr_start;
        lock.end = adr_end;
    }
    else{
        lock.start = adr_end;
        lock.end = adr_start;
    }

    this->LockedMem.push_back(lock);
}

//===================Functi-DEBUG==============================
void Memory::PrintMemory(){
    printf("Ram : \n");
    for(size_t i = 0 ; i < VM::RAM_SIZE ; i++){
        if(RAM[i] == 0 && i > 10){
            break;
        }else if(RAM[i] != 0 && i > 10)
        {
            printf("Adress: %d   0x%016llx\n" , i , RAM[i]);
        }
        else
            printf("Adress: %d   0x%016llx\n" , i , RAM[i]);
    }

    printf("\n\nRom : \n");
    for(size_t i = 0 ; i < VM::RAM_SIZE ; i++){
        if(ROM[i] == 0 && i > 10){
            break;
        }else if(ROM[i] != 0 && i > 10)
        {
            printf("Adress: %d   0x%016llx\n" , i , ROM[i]);
        }
        else
            printf("Adress: %d   0x%016llx\n" , i , ROM[i]);
    }
}
