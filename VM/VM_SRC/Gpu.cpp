#include"Gpu.h"
#include"Cpu.h"
#include"Raylib_CPP.h"


GPU::GPU(Memory* mem): mem(mem)
{
        //Initializam fereastra de debug 
        InitWindow(VMWINDOW::window_lenght , VMWINDOW::window_height , VMWINDOW::window_name);
        
        //Setam fps-ul 
        SetTargetFPS(VMWINDOW::FPS);

}

void GPU::Start(){
        Color culoare = {0,0,0,255};
        //Creeam bucla de run al ferestrei 
        while(!WindowShouldClose()){
           
            //------Logica de desenare--------
            BeginDrawing();
            
            //Curatam fereastra 
            ClearBackground(culoare);//Punem culoarea alba bydefault ca si fundal 
            
            EndDrawing();

        }
        CloseWindow();

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