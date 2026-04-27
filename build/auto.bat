@echo off
echo Compilam intreg proiectul 
cd ..
cmake -G Ninja -B build 
Ninja -C build
cd build
