#pragma once
#include<atomic> 
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdint>
#include<cstdlib>
#include <stdio.h>
#include<thread>
#include<mutex>
#include<chrono>
#include<map>
#include"raylib.h"

namespace VM{

    //Definim global variabilele ce determina dimensiunea ram si Rom 
    inline constexpr uint64_t RAM_SIZE = 131072; // 1 MB 131.072
    inline constexpr uint64_t ROM_SIZE = 1310720; // 10 MB 1.310.720

    inline constexpr uint64_t REG_PC = 15; // PROGRAM COUNTER
    inline constexpr uint64_t REG_SP = 14; // PROGRAM COUNTER
    inline constexpr uint64_t REG_EBP = 13; // PROGRAM COUNTER

	inline constexpr uint64_t GPU_IO = RAM_SIZE - (RAM_SIZE / 100 ) * 10; //Adresa de start din memorie dedicata comunicari cu GPU 
	inline constexpr uint64_t INT_TABEL = GPU_IO -  (RAM_SIZE / 100 ) * 5; //Adresa de start a codului pentru a menegerea intreruperile
	inline constexpr uint64_t INT_DATA = INT_TABEL - (RAM_SIZE / 100 ) * 5; //Adresa de start unde se vor pune datele de catre dispozitivele externe 

    //CPU Flag
    enum FlagBit {
        ZERO_BIT = 0,
        SIGN_BIT = 1,
        CARRY_BIT = 2,
        OVERFLOW_BIT = 3
    };
    //Registri si Enum pentru opcode si registri in sine 
    enum class Registre : uint8_t{
		R0 = 0 ,
		R1 = 1 ,
		R2 = 2 ,
		R3 = 3 ,
		R4 = 4 ,
		R5 = 5 ,
		R6 = 6 ,
		R7 = 7 ,
		R8 = 8 ,
		R9 = 9 ,
		R10 = 10 ,
		R11 = 11 ,
		R12 = 12 ,
		ESP = 13 ,
		SP  = 14,
		PC  = 15
	};
	enum class OpCode : uint8_t{
		Add = 0x01, 
		SUB = 0x02,
		CMP = 0x03,
		SET = 0x04,
		LOAD = 0x05,
		STORE = 0x06,
		HALT = 0x07,
		ROM_Read = 0x08,
		ROM_Write = 0x09,
		MOV = 0x0A,
		PUSH = 0x0B,
		POP = 0x0C,
		NOP = 0x0D,
		JMP = 0x0f,
		JMQ = 0x10,
		JML = 0x11,
		JMM = 0x12,
        MUL = 0x13,
        DIV = 0x14,
        INC = 0x15,
        DEC = 0x16,
		XOR = 0x17,
		AND = 0x18,
		OR  = 0x19,
		NOT = 0x1a,
		SHL = 0x1b,
		SHR = 0x1c,
		CALL = 0x1d,
		RET = 0x1e,
		MEM_LOCK = 0x1f

	};

	//Specificam pentru fiecare opcode ce tip de argumente trebuie sa primeasca si de care sa fie 
	// Ideea e ca R reprezinta registri , si cati de R avem atatea argumente care sunt registri in sine iar I reprezinta zona imm , care este separata 
	enum class ARG_PATTERN{
		NONE  = 0,  
		R  = 1,
		RR = 2, 
		RRR = 3,
		CUSTOM = 4
	};
 	
}

namespace VMWINDOW
{
	inline constexpr int window_height = 1300;
	inline constexpr int window_lenght = 1200;
	inline constexpr const char *window_name = "VM Debug Window";
	inline constexpr int FPS = 144;
	
} // namespace 



#if  defined(_WIN32) || defined(_WIN64)
    inline std::string File_name = "C:\\Users\\goguj\\Desktop\\VM\\VM\\CODE\\Code.src";
    inline std::string Output_name = "C:\\Users\\goguj\\Desktop\\VM\\VM\\CODE\\ROM.rm";
#elif defined(__linux__)
    inline std::string File_name = "/home/mixistorm/Desktop/VM/VM/CODE/Code.src";
    inline std::string Output_name = "/home/mixistorm/Desktop/VM/VM/CODE/ROM.rm";
#else
    #error "Sistem de operare nesuportat"
#endif