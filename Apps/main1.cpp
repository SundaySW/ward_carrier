//
// Created by outlaw on 22.08.2022.
//

#include "Ward_Carrier.hpp"

uint32_t adc_value_left_param1 = 0;
uint32_t adc_value_left_param2 = 0;
uint32_t adc_value_right_param1 = 0;
uint32_t adc_value_right_param2 = 0;

uint32_t L_HALL_values[2] = {0,};
uint32_t R_HALL_values[2] = {0,};

extern "C"{
    Button btn_fwrd = Button(BTN_FRWD, BTN_FWD_Pin);
    Button btn_rvrs = Button(BTN_RVRS, BTN_RVS_Pin);
    Button btn_left = Button(BTN_LEFT, BTN_LEFT_Pin);
    Button btn_right = Button(BTN_RIGHT, BTN_RIGHT_Pin);
    Ward_Carrier wardCarrier;

    void initDevice(){
        HAL_TIM_IC_Start_DMA(&htim1, TIM_CHANNEL_2, (uint32_t*)L_HALL_values, sizeof(L_HALL_values));
        HAL_TIM_IC_Start_DMA(&htim8, TIM_CHANNEL_2, (uint32_t*)R_HALL_values, sizeof(R_HALL_values));
        wardCarrier.initDevice();
        wardCarrier.getMovController().initMotors(&htim3, &htim4);
        wardCarrier.getMovController().initHALLSensors(L_HALL_values, R_HALL_values);
    }

    void EXTI_clear_enable(){
        __HAL_GPIO_EXTI_CLEAR_IT(BTN_LEFT_Pin);
        NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
        HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

        __HAL_GPIO_EXTI_CLEAR_IT(BTN_RVS_Pin);
        NVIC_ClearPendingIRQ(EXTI1_IRQn);
        HAL_NVIC_EnableIRQ(EXTI1_IRQn);

        __HAL_GPIO_EXTI_CLEAR_IT(BTN_FWD_Pin);
        NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
        HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

        __HAL_GPIO_EXTI_CLEAR_IT(BTN_RIGHT_Pin);
        NVIC_ClearPendingIRQ(EXTI3_IRQn);
        HAL_NVIC_EnableIRQ(EXTI3_IRQn);
    }

    volatile uint32_t time_btn_rvrs_pressed;
    void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
    {
        switch (GPIO_Pin) {
            case BTN_FWD_Pin:
                wardCarrier.frwd_btn_action(btn_fwrd);
                btn_fwrd.changeState();
                break;
            case BTN_RVS_Pin:
                wardCarrier.rvrs_btn_action(btn_rvrs);
                btn_rvrs.changeState();
                break;
            case BTN_LEFT_Pin:
                wardCarrier.left_btn_action(btn_left);
                btn_left.changeState();
                break;
            case BTN_RIGHT_Pin:
                wardCarrier.right_btn_action(btn_right);
                btn_right.changeState();
                break;
            default:
                break;
        }
    }

    void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
    {
        if(htim->Instance == TIM1){
            wardCarrier.update();
        }
        if(htim->Instance == TIM3){
    //        HAL_IWDG_Refresh(&hiwdg);
        }
    }

    void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
    {
        if((htim->Instance == TIM3) || (htim->Instance == TIM4))
            wardCarrier.motor_refresh(htim);
    }

    void while1_in_mainCpp(){

    }
};