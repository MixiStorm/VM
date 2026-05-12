#pragma once
#include"VM_Config.h"
#include"Intreruperi.h"


class KeyBoard{
public:
    KeyBoard();

    int16_t GetCharKey();

    void Start();

};