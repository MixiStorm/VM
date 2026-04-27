#include"Gpu.h"
#include"Cpu.h"
#include"Raylib_CPP.h"


GPU::GPU(std::vector<uint64_t>* reg , std::vector<uint64_t>* rom , std::vector<uint64_t>* ram , CPU * cpu):
    registri(reg) , ROM(rom) , RAM(ram) , cpu(cpu)
{
        //Initializam fereastra de debug 
        InitWindow(VMWINDOW::window_lenght , VMWINDOW::window_height , VMWINDOW::window_name);
        
        //Setam fps-ul 
        SetTargetFPS(VMWINDOW::FPS);

}

void GPU::Start(){
        Color culoare = {245,245,245,255};
        int delay = 0 ; //Vreau ca la fiecare 10 frameuri sa imi verifice daca apas o tasta la un interval de 10 frameuri 
        bool Check_key = true;

        Text txt;
        txt.Add_Text("Sistem Debug Started..." , {10 , 10} , RED , 50);
        txt.Add_Text("Ana are mere" , {10 , 10} , {200 , 100 , 30 , 255} , 30);

        //Creeam bucla de run al ferestrei 
        while(!WindowShouldClose()){
            //Logica de update 
            if (!Check_key){
                delay++;
                // Adaugam un delay de 16 frameuri pana cand verificam umratoarea tasta apasata 
                if(delay % 11 == 0){
                    Check_key = true;
                    delay = 0;
                }
            }
            
            //------Logica de desenare--------
            BeginDrawing();
            
            //Curatam fereastra 
            ClearBackground(culoare);//Punem culoarea alba bydefault ca si fundal 
            
            //Desenam un text 
           
            txt.Draw_Text();

            DrawLine(0 , 100 , VMWINDOW::window_lenght , 100 , RED);
            
            //O sa vrem sa blocam sau sa facem step in pentru cpu doar inainte de a afisa datele 
            //Ca sa activam cpu trigger , practic sa faca un pas inainte doar la comanda Apasari tastei ENTER
            // Mai intai apasam tasta D pentru a activa debug mode 
            if(IsKeyPressed(KEY_D) && Cpu_triger == false && Check_key){
                Cpu_triger = true;
                Check_key = false;
            }

            if(Cpu_triger){
                culoare = {245 , 200 ,200 ,255};
                bool * stepIn = cpu->GetStepIn(); // Obtinem variabila din clasa cpu care va opri sau va rula executia programului 
                //Inainte ca sa verificam daca apasam tasta entere trebuie sa oprim executia procesorului 
                {
                    std::lock_guard<std::mutex> lock(cpu->GetMutex());
                    *stepIn = false;

                    if(IsKeyPressed(KEY_ENTER))
                        *stepIn = 1;
                }

                if(IsKeyPressed(KEY_D) && Check_key){
                    Cpu_triger = false;
                    Check_key = false;
                    *stepIn = 1;
                }
            }
            else{
                culoare = {245 , 245 ,245 ,255};
            }

            //auto start = std::chrono::high_resolution_clock::now();
            // 700 ns de delay din cauza acestui mutex
            {
                //Afisam registri 
                std::lock_guard<std::mutex> lock(cpu->GetMutex());
                REG = *registri;
            }
            //auto end = std::chrono::high_resolution_clock::now();
            //auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            //std::cout<<"Timpul de blocare mutex de catre gpu : "<<time.count()<<" ns"<<std::endl;
            PrintRegistri(REG);
            //Terminam zona de desenat pe ecran 
            EndDrawing();

        }
        CloseWindow();

}

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