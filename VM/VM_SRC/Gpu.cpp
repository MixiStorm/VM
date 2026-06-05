#include"Gpu.h"
#include"Cpu.h"
#include"Raylib_CPP.h"


GPU::GPU(Memory* mem): mem(mem) , VRAM(VM::VRAM_SIZE , 0){
        //Setam fps-ul 
        SetTargetFPS(VMWINDOW::FPS);
}
bool GPU::GPU_INIT(){
        int attemps = 0;
        //Trebuie sa verificam la adresa VM::GPU_IO daca este scrisa vaLOAREA  0xFFAA care specifica ca cpu-ul a scris date acolo 
        while(1){
                /*
                        Ok , trebuie sa limitam viteza de rularea a acestui thread pentru ca e mult prea rapid si incearca de 10k ori sa 
                        initializeze GPU-ul , dar procesorul inca nu are timp sa incarce intreg programul in ram suficient de rapid, asa 
                        ca o sa limitam viteza GPU-ului in functia de initializare la 10 Khz 
                */
                std::this_thread::sleep_for(std::chrono::microseconds(1000));
                attemps ++;
                uint64_t Magic_Numbre = mem->Read_Memory(VM::GPU_IO);
                //std::cout<<"Addres:  "<<VM::GPU_IO<<" VALOARE: "<<Magic_Numbre<<" Incercari:  "<<attemps<<std::endl;
                if(Magic_Numbre == 0xFFAA){
                        //Initializam fereastra de debug 
                        InitWindow(VMWINDOW::window_lenght , VMWINDOW::window_height , VMWINDOW::window_name);
                        uint64_t MOD = mem->Read_Memory(VM::GPU_IO + 1);
                        this->GPU_MODE = MOD;
                        mem->Write_Memory(VM::GPU_IO , 0); // Curatam memoria de adresare astfel incat sa anuntam cpu-ul ca am citit totul 
                        
                        return true;
                }
                else if(attemps >= 10000) // O sa incercam de 10k ori sa initializam placa video daca nu o sa returnam false
                {
                        printf("Am efectuat 10k incercari de initializare esuate \n");
                        return false;
                }        
        }

        return false;
}    


void GPU::Start(){
        Color culoare = {0,0,0,255};

        if(this->GPU_INIT()){
                //Creeam bucla de run al ferestrei 
                while(!WindowShouldClose()){

                //------Logica de desenare--------
                BeginDrawing();
                
                //Curatam fereastra 
                ClearBackground(culoare);//Punem culoarea alba bydefault ca si fundal 
                
                if(GPU_MODE == 1)
                        TEXT_MODE_PRINT_TEXT();

                EndDrawing();

                }
                CloseWindow();
        }
        //std::cout<<"GPU_MODE: "<<(int)GPU_MODE<<std::endl;

}
/*
void GPU::PrintRegistri(std::vector<uint64_t> registri){
        int count_registri = 16; // Facem hardcoded pentru ca foar atatea o sa avem for ever (speer)
        std::vector<std::string> Reg_name = {"R0" , "R1","R2","R3","R4","R5","R6","R7","R8","R9","R10",
                                            "R11", "R12" ,"EBP" , "SP" , "PC"};

        Text txt;
        float y_start = 200;
        float x_start = 5;
        int Offset_y = 25;
        for(int i = 0 ; i < count_registri ; i++){
            std::string reg_val = Reg_name[i] + " : "+ std::to_string(registri.at(i));
            txt.Add_Text(reg_val , {x_start , y_start} ,BLACK , 30);
        }
        txt.Add_Text(std::string(TextFormat("Instructiune -> 0x%016llx" , RAM->at(registri.at(VM::REG_PC)))) , {x_start , y_start} , BLACK , 30);
        
        txt.Draw_Text();
        txt.Clear_Buffer();
}
*/

void GPU::TEXT_MODE_PRINT_TEXT(){
        //Vom citi memoria
        std::vector<uint8_t> Text;
        //Setari defaut pentru text
        uint16_t Font_Size =  20;
        uint16_t Text_Start_x = 10 ;
        uint16_t Text_Start_y = 10 ;

        //Daca a scris ceva procesorul in memorie atunci vom copia totul in VRAM
        if(CanRead()){
                uint64_t Byte_Of_Data = 0xffffffffffffffff; 
                uint64_t GPU_TEXT_MEM = VM::GPU_IO + 2;
                size_t i = 0 ; 
                while (Byte_Of_Data != 0 ){
                        Byte_Of_Data = mem->Read_Memory(GPU_TEXT_MEM++);
                        if(Byte_Of_Data != 0){
                                printf("TAR : 0x%016lX\n" , Byte_Of_Data);
                                VRAM[i++] = Byte_Of_Data;
                        }
                }
                mem->Write_Memory(VM::GPU_IO , 0);

        }
        //Acum intram in pipeline-ul de desenare a textului pe ecran 
        char Litera ;
        uint8_t Line ;
        uint8_t Column ;
        
        uint64_t V_Index = 0 ;
        uint64_t Data =VRAM[V_Index];
        //printf("DEBUG !!!!!  GPU _ DATA : 0x%016llX \n" , Data);
        //Citim intreg vramul caracter cu caracter 
        while(Data != 0 ){
                Data =VRAM[V_Index];
                if(Data == 0)
                        break;
                //printf("[DEBUG] GPU_DATA GOT : 0x%016llx  , | %ld  | VRAM addr: 0x%016llx \n" , Data , Data , V_Index);
                V_Index++;

                Litera = Data;
                Line = Data >> 8;
                Column = Data >> 16;
                char Display_Text [2] = {Litera , '\0'};
                
                DrawText(Display_Text , Text_Start_x * Column , Text_Start_y * Line , Font_Size , WHITE);
        }
        
}
