cmake_minimum_required(VERSION 3.10)

project(VM_app)

#Adaugam Raylib 
add_subdirectory(raylib)

#Aici adaugam executabilul pentru VM
file(GLOB VM_SOURCE "VM/VM_SRC/*.cpp")

add_executable(VM ${VM_SOURCE})

# Dacă tot dă eroarea, forțăm limbajul C++ pe acest target:
set_target_properties(VM PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(VM PRIVATE
   raylib/src 
   Resurse
   VM/VM_SRC
   VM)

target_link_libraries(VM PRIVATE raylib)



#aici adaugam executabilul pentru Compiler
add_executable(Compiler Compiler/Compiler.cpp)

#Adaugam librariile necesare pentru compiler 
target_include_directories(Compiler PRIVATE 
    Resurse
    Compiler 
    Compiler/Compiler_Resurces 
    raylib/src
    )
                

if(WIN32)
    target_link_libraries(VM PRIVATE opengl32 gdi32 winmm)
endif()