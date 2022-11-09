//
// Created by 79162 on 03.04.2022.
//

#ifndef MICROSCOPE_A4_BOARD_PIN_HPP
#define MICROSCOPE_A4_BOARD_PIN_HPP

#include <cstdint>
#include <stm32g431xx.h>
#include "type_traits"

enum LOGIC_LEVEL{
    LOW = 0,
    HIGH = 1,
};
enum INPUT_TYPE{
    R_HAL_SENSOR,
    L_HAL_SENSOR,
    R_R_IS,
    R_L_IS,
    L_R_IS,
    L_L_IS
};
enum MOTOR_OUTS{
    R_EN,
    L_EN,
    R_PWM,
    L_PWM,
};

struct PinReadable{};

struct PinWriteable{};

template<typename PinName, typename InterfaceType>
class PIN{
public:
    LOGIC_LEVEL currentState = LOW;
    const PinName pinName;

    template<typename T = InterfaceType>
        requires(std::is_base_of<PinReadable, T>::value)
    inline constexpr LOGIC_LEVEL getValue() const {
        LOGIC_LEVEL retVal;
        if ((port->IDR & pin) != (uint32_t)LOGIC_LEVEL::LOW) retVal = LOGIC_LEVEL::HIGH;
        else retVal = LOGIC_LEVEL::LOW;
        if(inverted) return (retVal ? LOGIC_LEVEL::LOW : LOGIC_LEVEL::HIGH);
        else return retVal;
    }

    template<typename T = InterfaceType>
        requires(std::is_base_of<PinReadable, T>::value)
    inline LOGIC_LEVEL refresh(){
        currentState = getValue();
        return currentState;
    }

    template<typename T = InterfaceType>
        requires(std::is_base_of<PinWriteable, T>::value)
    inline void setValue(LOGIC_LEVEL value){
        if(inverted){
            if (value) port->BRR = (uint32_t)pin;
            else port->BSRR = (uint32_t)pin;
        }else{
            if (value) port->BSRR = (uint32_t)pin;
            else port->BRR = (uint32_t)pin;
        }
        currentState = value;
    }

    template<typename T = InterfaceType>
        requires(std::is_base_of<PinWriteable, T>::value)
    inline void togglePinState(){
        uint32_t odr = port->ODR;
        port->BSRR = ((odr & pin) << 16U) | (~odr & pin);
    }

    PIN() = delete;
    const PIN& operator=(const PIN &) = delete;
    PIN& operator=(PIN &) = delete;

    void setInverted() {
        inverted = true;
    }

    constexpr explicit PIN(PinName incomeName, GPIO_TypeDef* incomePortPtr, uint16_t incomePin):
            pinName(incomeName),
            port(incomePortPtr),
            pin(incomePin)
    {};
protected:
private:
    GPIO_TypeDef* port;
    uint16_t pin;
    bool inverted = false;
};

#endif //MICROSCOPE_A4_BOARD_PIN_HPP
