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
        //std::thread cpuLog(&Check_GPU_IO , this);

        //Acum initializam si pornim GPU-ul
        gpu.Start();

        // 3. După ce închidem fereastra, așteptăm CPU-ul să termine
        if(cpuThread.joinable()) 
            cpuThread.join();

        //if(cpuLog.joinable())
          //  cpuLog.join();
    }

private:
    void StartCPU(CPU & cpu){cpu.Start();}    
    void Check_GPU_IO(){
        std::ofstream fout("CPU_LOG.txt");
        uint64_t Last_Value = mem.Read_Memory(VM::GPU_IO);
        uint64_t Curent_Value = 0xfffffffffff;
        fout<<"GPU_IO : "<<Last_Value<<std::endl;

        while(1){
            Curent_Value = mem.Read_Memory(VM::GPU_IO);
            if(Last_Value !=Curent_Value)
            {
                Last_Value = Curent_Value ;
                fout<<"GPU_IO : "<<Last_Value<<std::endl;
            }
            if(Curent_Value == 20)
                return;
        }

    }

};



int main(){
    
    MotherBoard MyPC(Output_name);
    MyPC.StartUP();
        
}
// Comanda de compilare : g++ Vm.cpp -o Vm -O3^S -std=c++20
