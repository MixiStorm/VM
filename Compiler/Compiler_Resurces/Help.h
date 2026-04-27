#pragma once
#include<string>
#include<iostream>
#include<sstream>

std::string ToUP(std::string d){
    std::string ret;
    char c ;
    for(int i = 0 ; i < d.size() ; i++)
    {
        c = d[i];
        if(c >= 'a' && c <= 'z')
            c -= 32;
        ret += c;
    }
    if(ret.size() == 0)
        ret = d;
    return ret;
}

std::string ToLow(std::string d){
    std::string ret;
    char c ;
    for(int i = 0 ; i < d.size() ; i++)
    {
        c = d[i];
        if(c >= 'A' && c <= 'Z')
            c -= 32;

        ret += c;
    }
    if(ret.size() == 0)
        ret = d;
    return ret;
}



void PrintAsciNr(std::string s){
    char c ;
    for(int i = 0 ; i < s.size() ; i++){
        c = s[i];
        std::cout<<(int)c<<" ";
    }
    std::cout<<std::endl;
}
