#include<stdio.h>
#include<iostream>
#include<cstdint>


uint64_t Test = 131072;

struct test{
    int a = 0 ;
    int b = 0 ;
};

int main(){

    printf("Afisam numarul de test : %lld\n" , Test);

    printf("Org: %lld , HEX: 0x%016llx\n" , Test, Test);
    
    printf("Acum vom  face bit shift 20 de  poziti in stanga \n");
    
    uint64_t Mask = Test;
    Test |= (uint64_t)(Test << 20)  ;
    Test = Test ^ Mask;
    printf("Org: %lld , HEX: 0x%016llx\n" , Test, Test);

    test T = {1 ,1};
    std::cout<<"Initializare  structura : "<<std::endl<< T.a <<" "<<T.b<<std::endl;

    return 0 ;
}