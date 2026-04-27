#include<strstream>
#include<sstream>
#include<algorithm>
#include"Help.h"
#include"VM_Config.h"

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

//Creeam o mapa pentru directivele define 
std::map<std::string , std::string> Directiva_Define;

struct Section_Var
{
	std::string type = "none";
	std::string var_name = "";

    // Adaugă acest operator pentru ca map-ul să poată sorta cheile
    bool operator<(const Section_Var& other) const {
        // Compară întâi numele variabilei, apoi tipul
        if (var_name != other.var_name) {
            return var_name < other.var_name;
        }
        return type < other.type;
    }
};

struct Procesed_Line {
    int original_line = 0 ;
    uint64_t addres = 0 ;
    std::string memnonic ;
    std::vector<std::string> args;
    std::string complit_line ;
};

//Creeam o mapa pentru variabilele din sectiunile de date 
std::map<Section_Var , std::vector<uint64_t> > Variabile;

//Creeam o mapa pentru Labeluri 
std::map<std::string , uint64_t> Labels;


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

//Trecerea de expansiune care se ocupa cu [REG + ofset]
std::vector<std::string> Expansion_Pass(std::vector<std::string>& Lines){
    std::vector<std::string> Exp;

    std::string line;
    for(auto l : Lines){
        line = l;

        //Vom verigica daca exista o paranteaza dreapta deschisa 
        size_t LB = line.find("[");
        size_t RB = line.find("]");
        if((LB == std::string::npos) & (RB == std::string::npos)) 
        {
           //Perfect nu avem o extensie 
           Exp.push_back(line);
           continue;
        }
        else if(LB == std::string::npos){
            //Inseamna ca avem paranteza de inchidere dar nu avem paranteza de deschidere 
            std::cerr<<"Nu exista paranteza de deschidere la aceasta linie : "<<line<<std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(10));
            exit(100);
        }
        else if(RB == std::string::npos){
            //Inseamna ca avem paranteza de inchidere dar nu avem paranteza de deschidere 
            std::cerr<<"Nu exista paranteza de inchidere la aceasta linie : "<<line<<std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(10));
            exit(100);
        
        }

        //Daca am ajuns aici inseamna ca avem atat paranteza deschisa cat si inchisa [ ]

        //Vom verigica daca exista o paranteaza dreapta deschisa 
        size_t add = line.find("+");
        size_t sub = line.find("-");
        if((add == std::string::npos) & (sub == std::string::npos))     
        {
            //Inseamna ca avem paranteza de inchidere dar nu avem paranteza de deschidere 
            std::cerr<<"Nu se efectueaza nici o operatie  la aceasta linie : "<<line<<std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(10));
            exit(100);
        }

        int op_type = 0; // Tipul de operaatie , 1 pentru adunare si 2 pentru scadere 
        if(add != std::string::npos){
            op_type = 1;
            line.replace(add , 1 , " ");
        }
        else{
            op_type = 2;
            line.replace(sub , 1 , " ");
        }
        
        //Prima data eliminam parantezele 
        line.replace(RB , 1 , " ");
        line.replace(LB , 1 , " ");

        std::string REG;
        std::string ofset;
        std::string Main_line; // Asta va fi linia ce o vom inlocui gen MOV R0 [SP + 5]
        std::string temp = line.substr(LB , RB);
        std::string instructiunea_1; // Aceasta este prima instructiune 
        std::string instructiunea_2; // Aceasta este a doua instructiune 
        std::stringstream ss(temp);

        ss>>REG>>ofset;
        
        Main_line = line.substr(0 , LB);
        Main_line += " " + REG;

        if(op_type == 1){
            instructiunea_1 = "\tadd " + REG + " " + ofset;
            instructiunea_2 = "\tsub " + REG + " " + ofset;
        }else{
            instructiunea_1 = "\tsub " + REG + " " + ofset;
            instructiunea_2 = "\tadd " + REG + " " + ofset;    
        }

        Exp.push_back(instructiunea_1);
        Exp.push_back(Main_line);
        Exp.push_back(instructiunea_2);
    }
    return Exp;
}

#if  defined(_WIN32) || defined(_WIN64)
    inline std::string File_name = "C:\\Users\\goguj\\Desktop\\VM\\VM\\CODE\\Code.src";
    inline std::string Output_name = "C:\\Users\\goguj\\Desktop\\VM\\VM\\CODE\\ROM.rm";
#elif defined(__linux__)
    inline std::string File_name = "/home/mixistorm/Desktop/VM/VM/CODE/Code.src";
    inline std::string Output_name = "/home/mixistorm/Desktop/VM/VM/CODE/ROM.rm";
#else
    #error "Sistem de operare nesuportat"
#endif

inline bool Debug_Mode = true;