#include<stdio.h>
#include<iostream>
#include<cstdint>


uint64_t Test = 131072;

struct test{
    int a = 0 ;
    int b = 0 ;
};

int main(){

    int a = 10  ,   B = 10;
    if((a != 10) ^ (B != 10)){
        std::cout<<"Adevarat\n";
    }
    else
        std::cout<<"Fals\n";
    return 0 ;
}