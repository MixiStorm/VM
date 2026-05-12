#pragma once 
#include"VM_Config.h"
#include <cstdarg>

class Text{
private:
    struct Text_Data{
        Vector2 poz = {0 , 0};
        int FontSize = 10 ;
        Color Culoare = { 0 , 0 , 0 , 255};
        std::string _Text_;

        void Clear(){
            poz = {0 , 0};
            FontSize = 10 ;
            Culoare = { 0 , 0 , 0 , 255};
            _Text_.clear();
        }
    };

private:
    Text_Data txt;
    float spacing = 2;
    std::vector<Text_Data> Text_Register;

public:
    Text(){}

    ~Text(){};

    //Adauga un nou text pe ecran relativ fata de celelalte texte introduse in acest obiect 
    void Add_Text(std::string _TXT, Vector2 _poz, Color _Culoare = { 0, 0, 0, 255 }, int _FontSize = 10) {
        Font DefaultFont = GetFontDefault();
        Vector2 Text_Size = MeasureTextEx(DefaultFont, _TXT.c_str(), (float)_FontSize, spacing);
        
        // Creăm Bounding Box-ul pentru textul nou
        Rectangle newRect = { _poz.x, _poz.y, Text_Size.x, Text_Size.y };

        // Verificăm suprapunerea cu textele deja existente
        bool overlap = true;
        while (overlap) {
            overlap = false;
            for (const auto& existing : Text_Register) {
                Vector2 existingSize = MeasureTextEx(DefaultFont, existing._Text_.c_str(), (float)existing.FontSize, spacing);
                Rectangle existingRect = { existing.poz.x, existing.poz.y, existingSize.x, existingSize.y };

                if (CheckCollisionRecs(newRect, existingRect)) {
                    // Dacă se suprapun, mutăm noul text în jos (cea mai sigură variantă)
                    newRect.y = existingRect.y + existingRect.height + 2; // +2 pixeli padding
                    overlap = true;
                    // Trebuie să re-verificăm tot vectorul pentru noua poziție
                    break; 
                }
            }
        }

        // Salvăm textul cu noua poziție calculată
        Text_Data newTxt;
        newTxt._Text_ = _TXT;
        newTxt.poz = { newRect.x, newRect.y };
        newTxt.FontSize = _FontSize;
        newTxt.Culoare = _Culoare;
        
        Text_Register.push_back(newTxt);
    }
    
    void Draw_Text(){
        for(auto txt : Text_Register){
            DrawTextEx(GetFontDefault(), txt._Text_.c_str() ,txt.poz, txt.FontSize ,spacing, txt.Culoare);
        }
    }

    std::string SetTextFormat(const char * txt , ...){
        va_list args;
        va_start(args , txt);

        const char* s = TextFormat(txt , args);
        std::cout<<"Functie: SetTextFormat: "<<s<<std::endl;
        va_end(args);

        return std::string(s); 
    }

    void  Clear_Buffer(){
        Text_Register.clear();
    }
};


