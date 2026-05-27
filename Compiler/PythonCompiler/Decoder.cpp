#include<stdio.h>
#include<cstdint>
#include<fstream>
using namespace std;

ifstream fin ("C:\\Users\\goguj\\Desktop\\VM\\VM\\CODE\\ROM.rm");

int main(){
    uint64_t inst;
    int i = 0 ;
    if(!fin.is_open()){
        printf("Ceva nu a mers bine si fisierul nu sa deschis");
        return 1;
    }

    while(fin.read(reinterpret_cast<char*>(&inst) , sizeof(uint64_t))){
        printf("Addr : %d  | 0x%016llx \n" , i  , inst);
        i++;
    }
    



}
