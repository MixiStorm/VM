#include"VM_Config.h"
#include"Cpu.h"
#include"Gpu.h"
#include"Memory.h"


void StartCpu(CPU & c){
    c.Start();
}

int main(){
    CPU cpu(Output_name);
    std::thread cpuThread(StartCpu , std::ref(cpu));

    Memory* mem = cpu.GetMem();

    GPU Unitatea_De_Procesare_Grafica(cpu.GetReg() , mem->GetRom() , mem->GetRam() , &cpu);



    Unitatea_De_Procesare_Grafica.Start();

    // 3. După ce închidem fereastra, așteptăm CPU-ul să termine
    if(cpuThread.joinable()) {
        cpuThread.join();
    }

        
}
// Comanda de compilare : g++ Vm.cpp -o Vm -O3^S -std=c++20
