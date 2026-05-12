#include"KeyBoard.h"



KeyBoard::KeyBoard(){}


int16_t KeyBoard::GetCharKey(){
    uint16_t KeyCode = GetCharPressed();

    if(KeyCode != 0 )
        return KeyCode;
        
    return -1 ;
}
