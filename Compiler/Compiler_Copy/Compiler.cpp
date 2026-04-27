#include<fstream>
#include<vector>
#include<cstdint>
#include<stdio.h>
#include<strstream>
#include<sstream>
#include<algorithm>
#include"Help.h"
#include"VM_Config.h"


#define DEFAULT_SOURCE_CODE "Code.src"
// Definim map-ul pentru OpCodes
std::map<std::string, uint8_t> OPCODES = {
    {"add", 0x01}, {"sub", 0x02}, {"cmp", 0x03}, {"set", 0x04}, 
    {"load", 0x05}, {"store", 0x06}, {"halt", 0x07}, {"rom_read", 0x08}, 
    {"rom_write", 0x09}, {"mov", 0x0A}, {"push", 0x0B}, {"pop", 0x0C}, 
    {"nop", 0x0D}, {"jmp", 0x0f}, {"jmq", 0x10}, {"jml", 0x11}, 
    {"jmm", 0x12}, {"mul", 0x13}, {"div", 0x14}, {"inc", 0x15}, 
    {"dec", 0x16}, {"xor", 0x17}, {"and", 0x18}, {"or", 0x19}, 
    {"not", 0x1a}, {"shl", 0x1b}, {"shr", 0x1c}, {"call", 0x1d}, 
    {"ret", 0x1e}, {"mem_lock" , 0x1f}
};

std::map<std::string , VM::ARG_PATTERN> OpcodeArgsType = {
		{"add" , VM::ARG_PATTERN::R_OR_I} , 
		{"sub" , VM::ARG_PATTERN::R_OR_I} , 
		{"cmp" , VM::ARG_PATTERN::R_OR_I} ,
		{"mul" , VM::ARG_PATTERN::R_OR_I} , 
		{"div" , VM::ARG_PATTERN::R_OR_I} , 
		{"set" , VM::ARG_PATTERN::RI    } ,
		{"load", VM::ARG_PATTERN::R_OR_I} , 
		{"sto" , VM::ARG_PATTERN::R_OR_I} , 
		{"halt", VM::ARG_PATTERN::NONE  } ,
		{"rom_read" , VM::ARG_PATTERN::RRR} ,
		{"rom_write", VM::ARG_PATTERN::RRR} , 
        {"mem_lock" , VM::ARG_PATTERN::RR } ,
		{"mov" , VM::ARG_PATTERN::RR    } ,
		{"push", VM::ARG_PATTERN::R_OR_I} , 
		{"pop" , VM::ARG_PATTERN::R     } ,
		{"nop" , VM::ARG_PATTERN::NONE  } , 
		{"jmp" , VM::ARG_PATTERN::R_OR_I} , 
		{"jmq" , VM::ARG_PATTERN::R_OR_I} ,
		{"jmm" , VM::ARG_PATTERN::R_OR_I} , 
		{"jml" , VM::ARG_PATTERN::R_OR_I} , 
		{"inc" , VM::ARG_PATTERN::R     } ,
		{"dec" , VM::ARG_PATTERN::R     } , 
		{"xor" , VM::ARG_PATTERN::RRR   } , 
		{"and" , VM::ARG_PATTERN::RRR   } ,
		{"or"  , VM::ARG_PATTERN::RRR   } , 
		{"shl" , VM::ARG_PATTERN::RRR   } , 
		{"shr" , VM::ARG_PATTERN::RRR   } ,
		{"not" , VM::ARG_PATTERN::RR    } ,
	};

// Definim map-ul pentru Registri
std::map<std::string, uint8_t> REGISTRI = {
    {"R0" , 0} , {"R1" , 1} , {"R2" , 2} , {"R3" , 3} , {"R4" , 4} , {"R5" , 5} , 
    {"R6" , 6} , {"R7" , 7} , {"R8" , 8} , {"R9" , 9} , {"R10" , 10} , 
    {"R11" , 11} , {"R12" , 12} , {"EBP" , 13} , {"SP" , 14} ,{"PC" , 15} 
};

struct IntermediateLine {
    int original_line_nr;     // Util pentru mesaje de eroare
    uint64_t address;         // Adresa la care va sta în RAM/ROM
    std::string mnemonic;      // "set", "add", etc.
    std::vector<std::string> args; // ["R1", "100"]
};

uint64_t Pack_Data(uint64_t in[] ){

    uint64_t cod = 0;
    cod |= (uint64_t)in[0];
    cod |= ((uint64_t)in[1] << 8);
    cod |= ((uint64_t)in[2] << 12);
    cod |= ((uint64_t)in[3] << 16);
    cod |= static_cast<uint64_t>(in[4] << 20);
    
    //printf("PACK : 0x%016llX\n" , cod);
    return cod;
}

std::vector<std::string> Read_File(std::string filename  ){
    std::ifstream file(filename);
    std::string line;
    std::vector<std::string> Lines;

    // ADAUGĂ ACEASTĂ VERIFICARE:
    if (!file.is_open()) {
        std::cerr << "EROARE CRITICA: Nu am putut deschide fisierul sursa la calea: " << filename << std::endl;
        return {}; // Returnează un vector gol
    }

    while(std::getline(file , line)){
        Lines.push_back(line);
    }
    return Lines;
}

//Preprocesarea se ocupa cu directivele de tip define 
std::vector<std::string> Pre_Procesare(std::vector<std::string>& Lines , std::map<std::string,std::string>& Define ){
    std::string linie ;
    std::string dir_define = "#define";
    std::vector<std::string> Procesed_Source;

    for(size_t i = 0 ; i < Lines.size() ; i++){
        linie = Lines[i];

        //Mai intai vom verifica pentru directiva define 

        size_t Define_pos = linie.find(dir_define);
        if(Define_pos != std::string::npos){
            linie = linie.substr(dir_define.size(), linie.size());
            std::stringstream ss(linie);
            std::string Word;
            std::string value;
            
            
            if (ss >> Word && ss >> value ){
                Define[Word] = value;
                //EX : #define GPU_Adres 0x7750
                continue;
            }
            else
            {
                std::cout<<"Eroare la citirea define : " <<Word<<std::endl;
                exit(10);
            }
                
        }

        // Comentariile vor fii ca si in cpp 
        size_t coment_pos = linie.find("//");
        if(coment_pos != std::string::npos)
            linie = linie.substr(0 , coment_pos);

        std::stringstream ss(linie);
        std::string word;
        std::vector<std::string> tokens;
        while(ss >> word) tokens.push_back(word);

        for(auto s : tokens){
            if(Define.count(s)){
                size_t def_poz = linie.find(s);
                linie.replace(def_poz , s.length() , Define[s]);
            }
        }
        if(!linie.empty())
            Procesed_Source.push_back(linie);
    }
    return Procesed_Source;
}


std::vector<IntermediateLine> FirstPass(std::vector<std::string>& Data , std::map<std::string , uint64_t>& Labels){

    std::vector<IntermediateLine> program;
    uint64_t curent_address = 0;
    int line_counter = 0;
   

    for(size_t i = 0 ; i < Data.size() ; i++){
        line_counter++;

        size_t label_poz = Data[i].find(":");
        if(label_poz != std::string::npos){
            Labels[Data[i].substr(0 , label_poz)] = curent_address;
            continue;
        }
        /*
        ====================================DOCUMENTATIE======================================
         
        Sectiuni de memorie :
            Vom sectiona memoria astfel incat sa avem o sectiune atat pentru variabile cat si pentru executabil 
            Pentru compilare fiecare sectiune de memorie imparte va avea un ID general care descrie cea ce face 
            
            ID_TABLE: 
                ID: 1   : Este dedicat doar pentru sectiunea org care specifica adresa la care va pune tot codul de dupa el 
                ID: 2   : Este dedicat pentru sectiunea de date in care se permite sa se scrie si citeasca date in acelasi timp 
                ID: 3   : Este dedicat pentru sectiunea de date care sunt read_only pentru variabilele constante , in aceasta sectiune 
                          daca se va incerca scrieerea de date , VM-ul in sine va opri exectuia si va arunca un cod de eroare pentru ca 
                          se incearca scrierea de date intr-o zona de memorie specificata ca fiind read_only
                ID: 10  : Este dedicata pentru sectiunea de variabile sau date in genereal , mai exact tot ce se afla dupa .db vor fii copiate 
                          bit by bit in memorie
                          [ATENTIE]
                          Aceasta sectiune este de sine statatoare si independenta , trebuie utilzata doar in interiorul unor sectiuni de date 
                          dedicata ".text , .data" pentru ca altfel daca ajunge cumva VM-ul sa citeasca acele date le va interpreta ca si instructiuni 
                          si va incerca sa le decodeze si execute , evident , ca poti folosi .db pentru a scrie instructiuni direct in cod de masina 
                          spre exemplu , numarul 1 reprezinta instructiunea ADD , mai exact ADD R0 R0 , sau poti sa folosesti 2,097,153 care este echivalenta cu ADD R0 2
                          Daaaaa , dupa cum se vede trebuie sa fii nebun sa faci asta , dar daca gresesti numarul , o sa interpreteze numarul ca si o instructiune random , sau inexistenta 
                          cea ce va duce la un comportament neasteptat .

        */
        std::map<std::string , uint8_t> sectiuni_memorie = 
        {{"org" , 1} , {"data" , 2} , {"text" , 3} , {".db" , 10}};

        std::stringstream ss(Data[i]);
        std::string word;
        std::vector<std::string> tokens;
        while(ss >> word) tokens.push_back(word);

        if(tokens.empty()) continue;

        // 3. Salvăm datele intermediare
        IntermediateLine il;
        il.original_line_nr = line_counter;
        il.address = curent_address++;
        il.mnemonic = tokens[0];
        // Restul tokenurilor sunt argumente (R1, R2, etc.)
        for(size_t i = 1; i < tokens.size(); i++) il.args.push_back(tokens[i]);

        program.push_back(il);
    
    }
    return program;
}


std::vector<uint64_t> SecondPass(std::vector<IntermediateLine> & program , std::map<std::string , uint64_t>& Labels){
    std::vector<uint64_t> binary_data;
    
    std::cout<<"SecondPass Program_size : "<<program.size()<<std::endl;
    for(auto & il : program){
        uint8_t Opcode ;
        
        //Verificamd aca exista instructiunea in opcode 
        if(OPCODES.count(ToLow(il.mnemonic))){
            Opcode = OPCODES[ToLow(il.mnemonic)];
        }else{
            std::cerr<<"Eroare la linia "<<il.original_line_nr<<" : Instructiune necunoscuta!!!" <<std::endl;
            break;
        }
        
        //Verificam daca avem o instructiune fara argumente , daca nu avem trebuie sa setam parametri instructiuni 
        uint64_t instructiune[7] = {0};
        instructiune[0] = Opcode; 
        if(Opcode != OPCODES["nop"] && Opcode != OPCODES["halt"])
        {   
            auto is_nr = [](const std::string& s) {
                        return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
                        };
            
            int i = 1;
            for(auto & a : il.args)
            {   
                //std::cout<<"Argumente pentru instructiune : OP: "<<(int)Opcode<<" Arg["<<i<<"]: "<<a<<std::endl;
                if(REGISTRI.count(ToUP(a)))
                    instructiune[i] = REGISTRI[ToUP(a)];
                else if(is_nr(a)){
                    //std::cout<<"Am detectat ca acset argument este un numar : "<<a<< "Stoull: "<<std::stoull(a)<<std::endl;
                    instructiune[4] = std::stoull(a); // Imm este hadrcoded la pozitia 4
                    break;
                }else if(Labels.count(a))
                {   
                    //Am gasit labelul 
                    instructiune[4] = Labels[a];
                    break;
                }
                else{
                    printf("Argumentul acesta este invalid Linia : %d\n" , il.original_line_nr);
                    exit(100);
                }
                i++;
            }
        }

        uint64_t d  = Pack_Data(instructiune);
        binary_data.push_back(d);
    }

    return binary_data;
}



int main(int arg , char** args){

    std::string File_name = "C:\\Users\\goguj\\Desktop\\VM\\VM\\CODE\\Code.src";
    std::string Output_name = "C:\\Users\\goguj\\Desktop\\VM\\VM\\CODE\\ROM.rm";

    std::vector<IntermediateLine> it;
    std::vector<uint64_t> Raw_data;

    std::map<std::string , uint64_t> LabelMap;
    std::map<std::string , std::string> Directiva_Define;

    std::vector<std::string> Lines = Read_File(File_name);
    Lines = Pre_Procesare(Lines , Directiva_Define);

    it = FirstPass(Lines , LabelMap);
    //Debug
    for(auto i : it){
        std::cout<<"\n\nIT: "<<std::endl<<
        "Org_line: "<<i.original_line_nr<<"\n"<<
        "Addr: "<<i.address<<"\n"<<
        "mnemonic: "<<i.mnemonic<<"\n"<<
        "args: ";
        for(auto arg : i.args)
            std::cout<<arg<<" ";
        std::cout<<std::endl;
    }
    std::cout<<"\n"<<"Introdu ori ce pentru a inchide programul :" ;
    int a ;
    std::cin >> a;
    
    // Raw_data = SecondPass(it , LabelMap);


    // //Afisam textul codului sursa preprocesat 
    // for(auto l : Lines){
    //     std::cout<<l<<std::endl;
    // }

    // std::ofstream file(Output_name , std::ios::binary);
    // int addres = 0;
    // for(auto r : Raw_data){
    //     printf("Intstructiune  %d : 0x%016llX\n" ,addres, r);
    //     addres++;
    //     file.write(reinterpret_cast<const char*>(&r), sizeof(uint64_t));
    // }

    // std::cout<<"Programul a fost compilat , numar total de instructiuni : "<<Raw_data.size()<<std::endl;
    
}