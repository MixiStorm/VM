#include "comp.h"

//Vectori care sa memoreze sectiunile de date 
std::vector<std::string> Section_data;
std::vector<std::string> Section_text;

//Functia de preprocesare
std::vector<std::string> Preprocesare(std::vector<std::string> RD){
    //Functia va returna codul sursa doar ca fara section fara comentari si fara define 
    //Sectiunile vor avea un identificator specific : ->section. [type]
    //Continutul sectiunilor se va afla intre acolade { }
    std::string id_section = "section.";
    std::string id_define  = "#define";
    std::string id_coment  = "//";

    std::vector<std::string> Raw_Data;

    for(size_t i = 0 ; i < RD.size() ; i++){
        std::string line = RD[i];

        //Verificam pentru comentarii 
        size_t coment_pos = line.find(id_coment);
        if(coment_pos != std::string::npos){
            line = line.substr(0 , coment_pos);
            
            if(line.empty())
                continue;
        }

        //Verificam pentru directiva define 
        size_t dir_define_pos = line.find(id_define);
        if(dir_define_pos != std::string::npos){
            line = line.replace(dir_define_pos , id_define.size() , ""); // #define R3 3 -> R3 3
            std::stringstream ss(line);
            std::string word;
            std::string value;

            ss>>word>>value;
            Directiva_Define[word] = value;
            continue;
        }

        //Verificam pentru sectiunea de date
        size_t section_pos = line.find(id_section);
        if(section_pos != std::string::npos){
            line = line.substr((section_pos + id_section.size()) , line.size());
            std::string section_type;
            std::stringstream ss(line);

            ss>>section_type;

            if(section_type != "data" && section_type != "text"){
                std::cout<<"Timpul de sectiune de date invalid : "<<section_type<<std::endl;
                exit(100);
            }
            std::vector<std::string> sectiune;

            //Verificam daca gasim pe aceasta linie {  sau este pe urmatoarea 
            size_t BL = line.find("{");
            size_t BR = line.find("}");
            if(BL == std::string::npos)
                line = RD[++i];

            BL = line.find("{");
            if(BL == std::string::npos){
                std::cout<<"Nu am gasit { la inceputul sectiuni de date de la liniea : "<<i<<std::endl;
                exit(100);
            }
            line = line.replace(0 , BL + 1 , "");

            while(BR == std::string::npos ){
                sectiune.push_back(line);
                i++;
                line = RD[i];
                BR = line.find("}");

                if(BR != std::string::npos){
                    line.replace(BR , 1, "");
                    sectiune.push_back(line);
                    break;
                }
            }


            if(section_type == "data")
                Section_data = sectiune;
            else
                Section_text = sectiune;

        }


        auto is_only_space = [](const std::string& l){
            for(char c : l){
                if(!std::isspace(c))
                    return false;
            }
            return true;
        };        

        if(!line.empty() && !is_only_space(line))
            Raw_Data.push_back(line);
    }

    //Vom crea o lista de elemente pe care le vom elimina ne fiind admise  practic le ignoram 
    std::vector<std::string> Not_Alawd_Simbols = {
        "[" , "]" , "{" , "}" , "/" , "*" , "=" , "+" , "-"};
    // ADD R0 [R1+6] => ADD R0  R1 6 
    //Acum vom inlocui toate directivele difine din Raw_Data 
    for(auto & line : Raw_Data){
        std::string word;
        std::stringstream ss(line);

        while(ss >> word){
            size_t Not_Allawd_Pos = 0;

            for(auto & simbols : Not_Alawd_Simbols){
                Not_Allawd_Pos = word.find(simbols);
                if(Not_Allawd_Pos != std::string::npos){
                    word.replace(Not_Allawd_Pos , 1 , " ");                    
                }
            }

            std::stringstream ss2(word);
            std::string _word;

            while(ss2 >> _word){
                if(Directiva_Define.count(_word))
                    line.replace(line.find(_word) , _word.size() , Directiva_Define[_word]);
            }

        }
    }

    Raw_Data = Expansion_Pass(Raw_Data);
    return Raw_Data;
}

//First pass ->Returneaza doar o lista de instrucitune pura 
std::vector<Procesed_Line> FirstPass(std::vector<std::string> RD , bool Procesare_Sectiuni = true){
    //BTW RD = RawData
  
    
    auto ProcessDataSection = [&](std::vector<std::string> & section){
        //Trebuie sa rezolvam sectiunile de date 
        Section_Var My_Var;
        //O sa incepem cu munca pe sectiunea de date din date
        for(auto & data : section){
            //Incercam sa citim datele din fisier 
            std::stringstream ss(data);
            std::string word;
            
            size_t var_name_pos = 0 ;
            bool exist_var_name = false;
            std::string var_name = "";
            uint64_t _data = 0;

            while(ss>>word){
                if(!exist_var_name){
                    var_name_pos = word.find(":");

                    if(var_name_pos == std::string::npos){
                        std::cout<<"In sectiunea de date nu sa gasit numele unei variabile"<<std::endl;
                        exit(100);
                    }

                    exist_var_name= true; 
                    var_name = word.substr(0 , var_name_pos);
                    
                    My_Var.type = "data";
                    My_Var.var_name = var_name;
                    
                    continue;
                }

                //Urmatorul token ar trebui sa fie .db 
                if(word == "db.")
                {
                    std::string restulLiniei = data.substr(data.find("db.") + 3 , data.length());
                    
                    size_t firstQuote = restulLiniei.find('"');
                    size_t secondQuote = restulLiniei.rfind('"');

                    //Verificam daca datele sunt de tip text 
                    if(firstQuote != std::string::npos && secondQuote != std::string::npos && firstQuote != secondQuote){
                        std::string text = restulLiniei.substr(firstQuote + 1 , secondQuote - 2);

                        for(char c : text){
                            Variabile[My_Var].push_back(static_cast<uint64_t>(c)); 
                        }
                    }
                    else{

                        // Daca nu este text atunci este un sir de numere 
                        std::replace(restulLiniei.begin() , restulLiniei.end() , ',' , ' ');

                        std::stringstream ss_rest(restulLiniei);

                        std::string number ; 
                        while(ss_rest >> number){
                            try{
                                uint64_t nr = std::stoull(number , nullptr , 0);
                                Variabile[My_Var].push_back(nr);
                            }catch(...){
                                std::cout<<"Ignoram : "<<number<<std::endl;
                            }

                        }

                    }
                    break;
                }
            }
        }    
    };
    
    if(Procesare_Sectiuni){
        //Procesam sectiunile de date 
        ProcessDataSection(Section_data);
        ProcessDataSection(Section_text);
    }


    //Procesam codul curatat si il transformam intr-o structura de date pentru a putea fii
    //  procesata in secondpass si convertita in cod binar 
    std::vector<Procesed_Line> new_RD;
    std::string line;
    uint64_t curent_address = 0;
    for(size_t i = 0 ; i < RD.size() ; i++){
        Procesed_Line PL;
        line = RD[i];

        size_t label_poz = line.find(":");
        if(label_poz != std::string::npos){
            std::string label_name = line.substr(0 , label_poz);
            label_name.erase(0, label_name.find_first_not_of(" \t\r\n"));
            label_name.erase(label_name.find_last_not_of(" \t\r\n") + 1);

            Labels[label_name] = curent_address;
            continue;
        }


        std::stringstream ss(line);
        std::string args;
        ss >> PL.memnonic;

        if(!OPCODES.count(PL.memnonic)){
            std::cout<<"Am identificat un opcode invalid : "<<PL.memnonic<<std::endl;
            exit(100);
        }

        while(ss >> args){
            PL.args.push_back(args);
        }


        PL.complit_line = line;
        PL.original_line = i;
        PL.addres = curent_address++;
        new_RD.push_back(PL);
    }

    //Inainte de finalizare trebuie sa verificam existenta etichetei _start
    if(!Labels.count("_start")){
        std::cout<<"[FATAL ERROR]\nLipseste entry point _start"<<std::endl;
        exit(0xDEAD);
    }

    return new_RD;
} 

std::vector<uint64_t> SecondPass(std::vector<Procesed_Line>& PL){
    std::vector<uint64_t> Binar;
    //Creeam boot_loader 
    uint64_t boot_size_ofset = 9;

    uint64_t boot_Start_adress = boot_size_ofset + VM::RAM_SIZE;// Alegem din start 9 pentru ca aceasta este dimensiunea boot-loaderului pe care nu vrem sa o copiem dinou in RAM 
    uint64_t boot_End_adress = 0; 
    uint64_t boot_Target_adress = boot_size_ofset;// Alegem din start 9 pentru ca aceasta este dimensiunea boot-loaderului pe care nu vrem sa o copiem dinou in RAM

    uint64_t boot_Stack_init = 0;

    uint64_t boot_Mem_lock_start = 0;
    uint64_t boot_Mem_lock_end = 0;

    uint64_t Executabel_Size = PL.size(); // Pentru ca in PL se afala strict doar instructiuni 

    //Trebuie sa calculam dimensiunea sectiunilor de date 
    uint64_t Total_section_data_size = 0;
    uint64_t Section_type_data_size = 0;

    for(auto & [key , value] : Variabile){
        if(key.type == "data")
            Section_type_data_size += value.size();
        Total_section_data_size += value.size();
    }
    
    
    //De tinut cont , sectiunea de date de tip Date este pusa prima dupa boot loader 
    // iar cea de text pe urma inainte de codul sursa
    uint64_t Total_Offset = Total_section_data_size + boot_size_ofset; 
    boot_End_adress       = Total_Offset + Executabel_Size + VM::RAM_SIZE;
    boot_Mem_lock_start   = boot_size_ofset         + Section_type_data_size;
    boot_Mem_lock_end     = Total_Offset            + Executabel_Size;
    boot_Stack_init = boot_Mem_lock_end + 1;


    std::vector<std::string> Boot_Loader = {
        "set R0 " + std::to_string(boot_Start_adress) , 
        "set R1 " + std::to_string(boot_End_adress),
        "set R2 " + std::to_string(boot_Target_adress),
        "rom_read R0 R1 R2" , 
        "set SP " + std::to_string(boot_Stack_init),
        "set R3 " + std::to_string(boot_Mem_lock_start),
        "set R4 " + std::to_string(boot_Mem_lock_end),
        "mem_lock R3 R4",
        "jmp _start"  
    };

    //Acum calculam si schimbam ofseturile 
    for(auto & [lab, value] : Labels){
        value += Total_Offset;
    }

    //Acum va trebui sa trecem prin toate datele procesate sa ilocuim labelurile si ofseturile 
    auto inlocuire_labels = [](std::vector<std::string> RD){
        for(auto & line : RD){
            std::stringstream ss(line);
            std::string word;


            while(ss>> word){
                if(Labels.count(word)){
                    size_t word_pos = line.find(word);
                    size_t word_size = word.size();
                    line.replace(word_pos ,word_size , std::to_string(Labels[word])); 
                }
            }
        }
        return RD;
    };  
    
    Boot_Loader = inlocuire_labels(Boot_Loader); //Codul sursa boot_loader procesat 
    std::vector<Procesed_Line> Boot_Loader_PL; 
    Boot_Loader_PL = FirstPass(Boot_Loader , false);

    //Am uitat de variabile si ca trebuie sa le atribui o adresa si lor :((
    std::map<std::string , uint64_t> Var_addr;
    uint64_t Last_var_addr = 9;
    
    //Acum trebuie sa iteram prin toate variabilele 
    std::string var_name;
    for(auto& [key , value] : Variabile){
        var_name = key.var_name;
        Var_addr[var_name] = Last_var_addr;
        Last_var_addr += value.size();
    }
    
    //Acum sa inlocuim numele variabilelor in adrese 
    for(auto& p: PL)
        for(auto& arg : p.args)
            if(Var_addr.count(arg))
                arg = std::to_string(Var_addr[arg]);

    //Inlocuin Labels
    for(auto& p: PL)
        for(auto& arg : p.args)
            if(Labels.count(arg))
                arg = std::to_string(Labels[arg]);

    //Generam binar pentru boot_loader 
    std::vector<uint64_t> Binar_Bot_loader = ToBin(Boot_Loader_PL);
    std::vector<uint64_t> Binar_Source_Code = ToBin(PL);
    Binar.reserve(boot_Mem_lock_end);

    //Generam intreg binarul acum 
    Binar.insert( Binar.end(),Binar_Bot_loader.begin() , Binar_Bot_loader.end()); 
    for(auto& [key , value] : Variabile){
        Binar.insert(Binar.end() , value.begin() , value.end());
    }
    Binar.insert(Binar.end() , Binar_Source_Code.begin() , Binar_Source_Code.end());
        
    


    return Binar;
}


int main(){
    std::vector<std::string> Raw_Data = Read_File(File_name) ;
    std::vector<Procesed_Line> Source_Procesed;

    if(Debug_Mode){
        std::cout<<"?Raw_Data size : "<<Raw_Data.size()<<std::endl;
        for(auto& l : Raw_Data)
            std::cout<<l<<std::endl;
    
        std::cout<<std::endl;    
    }

    Raw_Data = Preprocesare(Raw_Data);


    if(Debug_Mode){
        std::cout<<"?Raw_Data size : "<<Raw_Data.size()<<std::endl;
        for(auto& l : Raw_Data)
            std::cout<<l<<std::endl;
    
        std::cout<<std::endl;    
    }
    Source_Procesed = FirstPass(Raw_Data);
    std::cout<<"Datele procesate aflate in Procesed_Line: "<<std::endl;
    for(auto p : Source_Procesed){
        std::cout<<p.memnonic<<" ";
        for(auto a : p.args){
            std::cout<<a<<" ";
        }
        std::cout<<std::endl;
    }

    std::cout<<"Suntem inainte sa apelam SecondPass"<<std::endl;
    std::vector<uint64_t> Binar;
    Binar = SecondPass(Source_Procesed);
    std::cout<<"Dimensiune Binar: "<<Binar.size()<<std::endl;

    std::ofstream file(Output_name , std::ios::binary);
    int addres = 0;
    for(auto r : Binar){
        printf("Intstructiune  %d : 0x%016llX\n" ,addres, r);
        addres++;
        file.write(reinterpret_cast<const char*>(&r), sizeof(uint64_t));
    }

    std::cout<<"Programul a fost compilat , numar total de instructiuni : "<<Binar.size()<<std::endl;

    std::cout<<"Vom afisa tabela cu labeluri sub forma : NAME:  | VALUE: "<<std::endl;
    for(auto [key , value] : Labels){
        std::cout<<"NAME: "<<key<<"  |  VALUE: "<<value<<std::endl;
    }
    

}
