//
// Created by 79162 on 25.09.2021.
//

#ifndef MICROSCOPE_A4_BOARD_BUTTON_HPP
#define MICROSCOPE_A4_BOARD_BUTTON_HPP

#include <main.h>
struct Button{
    using BTN_PIN = PIN<BTN_TYPE, PinReadable>;
    explicit Button(BTN_PIN&& income_Pin): Pin(income_Pin){}
    BTN_PIN Pin;
    bool _state = false;
    inline operator bool() const{return Pin.getValue();}
    inline bool operator()() const {return Pin.getValue();}
    inline constexpr BTN_TYPE getType(){return Pin.pinName;}
    inline constexpr bool getState(){return _state;}
    inline BTN_PIN& getPin(){return Pin;}
    inline constexpr void setOff(){_state = false;}
    inline void changeState(){_state = !_state;}
};

#endif //MICROSCOPE_A4_BOARD_BUTTON_HPP
