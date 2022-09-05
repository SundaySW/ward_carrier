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
#include "MovementController.hpp"

static inline void delay(uint32_t delay){
    __HAL_TIM_DISABLE_IT(&htim1, TIM_IT_UPDATE);
    HAL_TIM_Base_Start(&htim6);
    TIM6->CNT = 0;
    while(TIM6->CNT < delay){}
    HAL_TIM_Base_Stop(&htim6);
    __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_UPDATE);
}

class Ward_Carrier {
public:
    const Ward_Carrier& operator=(const Ward_Carrier &) = delete;
    Ward_Carrier& operator=(Ward_Carrier &) = delete;
    Ward_Carrier() = default;

    void initDevice(TIM_HandleTypeDef *htim_l, TIM_HandleTypeDef *htim_r){
        movementController.initMotors(htim_l, htim_r);
    }

    void frwd_btn_action(bool state){
        if(state) movementController.moveForward();
        else movementController.stop();
    }
    void rvrs_btn_action(bool state){
        if(state) movementController.moveBackwards();
        else movementController.stop();
    }
    void left_btn_action(bool state){
        if(state) movementController.turnLeft();
        else movementController.straightDirection();
    }
    void right_btn_action(bool state){
        if(state) movementController.turnRight();
        else movementController.straightDirection();
    }

    void update() {

    }

    void motor_refresh(TIM_HandleTypeDef *htim) {
        movementController.motor_refresh(htim);
    }

private:
    MovementController movementController;
};


#endif //WARD_WHEELER_WARDWHEELER_HPP