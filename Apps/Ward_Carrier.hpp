//
// Created by outlaw on 15.08.2022.
//

#ifndef WARD_WHEELER_WARDWHEELER_HPP
#define WARD_WHEELER_WARDWHEELER_HPP

#include <PIN.hpp>
#include "main.h"
#include "DCMotor.hpp"
#include "Button.hpp"
#include "tim.h"

extern uint32_t adc_value_left_param1;
extern uint32_t adc_value_left_param2;
extern uint32_t adc_value_right_param1;
extern uint32_t adc_value_right_param2;

static inline void delay(uint32_t delay){
    __HAL_TIM_DISABLE_IT(&htim1, TIM_IT_UPDATE);
    HAL_TIM_Base_Start(&htim6);
    TIM6->CNT = 0;
    while(TIM6->CNT < delay){}
    HAL_TIM_Base_Stop(&htim6);
    __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_UPDATE);
}

class Ward_Carrier {
    using MOTOR_PIN = PIN<MOTOR_OUTS, PinWriteable>;
public:
    const Ward_Carrier& operator=(const Ward_Carrier &) = delete;
    Ward_Carrier& operator=(Ward_Carrier &) = delete;
    Ward_Carrier() = default;

    void initDevice(){
        MotorCfg cfgLeft{};
        cfgLeft.A = 30;
        cfgLeft.Vmax = 300;
        cfgLeft.htim = &htim3;
        cfgLeft.timChannel_L = TIM_CHANNEL_1;
        cfgLeft.timChannel_R = TIM_CHANNEL_2;
        cfgLeft.adc_value_param1 = &adc_value_left_param1;
        cfgLeft.adc_value_param2 = &adc_value_left_param2;
        left_motor.init(cfgLeft, [this](DCMotor* m){OnMotorEvent(m);});

        MotorCfg cfgRight{};
        cfgRight.A = 30;
        cfgRight.Vmax = 300;
        cfgRight.htim = &htim4;
        cfgRight.timChannel_L = TIM_CHANNEL_1;
        cfgRight.timChannel_R = TIM_CHANNEL_2;
        cfgRight.adc_value_param1 = &adc_value_right_param1;
        cfgRight.adc_value_param2 = &adc_value_right_param2;
        right_motor.init(cfgRight, [this](DCMotor* m){OnMotorEvent(m);});
    }

    void frwd_btn_action(bool state){

    }
    void rvrs_btn_action(bool state){

    }
    void left_btn_action(bool state){

    }
    void right_btn_action(bool state){

    }

    void OnMotorEvent(DCMotor *motor){
        MOTOR_EVENT event = motor->getEvent();
    }

private:
    MOTOR_PIN R_REN_PIN = MOTOR_PIN(R_EN, R_R_EN_GPIO_Port, R_R_EN_Pin);
    MOTOR_PIN R_LEN_PIN = MOTOR_PIN(L_EN, R_L_EN_GPIO_Port, R_L_EN_Pin);
    MOTOR_PIN R_RPWM_PIN = MOTOR_PIN(R_PWM, R_RPWM_GPIO_Port, R_RPWM_Pin);
    MOTOR_PIN R_LPWM_PIN = MOTOR_PIN(L_PWM, R_LPWM_GPIO_Port, R_LPWM_Pin);
    DCMotor right_motor = DCMotor(R_REN_PIN, R_LEN_PIN, R_RPWM_PIN, R_LPWM_PIN);

    MOTOR_PIN L_REN_PIN = MOTOR_PIN(R_EN, L_R_EN_GPIO_Port, L_R_EN_Pin);
    MOTOR_PIN L_LEN_PIN = MOTOR_PIN(L_EN, L_L_EN_GPIO_Port, L_L_EN_Pin);
    MOTOR_PIN L_RPWM_PIN = MOTOR_PIN(R_PWM, L_RPWM_GPIO_Port, L_RPWM_Pin);
    MOTOR_PIN L_LPWM_PIN = MOTOR_PIN(L_PWM, L_LPWM_GPIO_Port, L_LPWM_Pin);
    DCMotor left_motor = DCMotor(L_REN_PIN, L_LEN_PIN, L_RPWM_PIN, L_LPWM_PIN);
};


#endif //WARD_WHEELER_WARDWHEELER_HPP
