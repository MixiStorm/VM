#include"VM_Config.h"
#include"Cpu.h"
#include"Gpu.h"
#include"Memory.h"
#include"KeyBoard.h"

class MotherBoard{
    Memory mem;
    CPU cpu;
    GPU gpu;
    Intrerupere INT;
    KeyBoard Key;
private:
//Sistemul de intreruperi     

public:
    MotherBoard(std::string & ROM ): mem(ROM) , cpu(&mem , &INT) , gpu(&mem){
    }

    void StartUP(){
        //Trebuie sa creeam un nou thread care sa ruleze separat pentru cpu iar gpu-ul o sa ruleze pe threadul main 
        std::thread cpuThread(&MotherBoard::StartCPU , this, std::ref(this->cpu));

        //Acum initializam si pornim GPU-ul
        gpu.Start();

        // 3. După ce închidem fereastra, așteptăm CPU-ul să termine
        if(cpuThread.joinable()) 
            cpuThread.join();
    }

private:
    void StartCPU(CPU & cpu){cpu.Start();}    

};



int main(){
    
    MotherBoard MyPC(Output_name);
    MyPC.StartUP();
        
}
// Comanda de compilare : g++ Vm.cpp -o Vm -O3^S -std=c++20
