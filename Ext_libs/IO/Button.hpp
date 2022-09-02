//
// Created by 79162 on 25.09.2021.
//

#ifndef MICROSCOPE_A4_BOARD_BUTTON_HPP
#define MICROSCOPE_A4_BOARD_BUTTON_HPP

#include <main.h>
struct Button{
    explicit Button(BTN_TYPE incomeType, uint16_t incomeGPIO_Pin): type(incomeType), GPIO_Pin(incomeGPIO_Pin){}
    const BTN_TYPE type;
    const uint16_t GPIO_Pin;
    bool _state = false;
    inline operator bool() const{return _state;}
    inline bool operator()() const {return _state;}
    inline constexpr BTN_TYPE getType(){return type;}
    inline constexpr bool getState(){return _state;}
    inline void changeState(){_state = !_state;}
};

#endif //MICROSCOPE_A4_BOARD_BUTTON_HPP
