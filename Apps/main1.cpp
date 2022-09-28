//
// Created by outlaw on 22.08.2022.
//

#include "Ward_Carrier.hpp"

extern "C"{
uint32_t adc_values[4] = {0,};
uint32_t L_HALL_values[2] = {0,};
uint32_t R_HALL_values[2] = {0,};

    Ward_Carrier wardCarrier;

    Button btn_fwrd = Button(BTN_FRWD, BTN_FWD_Pin);
    Button btn_rvrs = Button(BTN_RVRS, BTN_RVS_Pin);
    Button btn_left = Button(BTN_LEFT, BTN_LEFT_Pin);
    Button btn_right = Button(BTN_RIGHT, BTN_RIGHT_Pin);

    void EXTI_clear_enable(){
        __HAL_GPIO_EXTI_CLEAR_IT(BTN_LEFT_Pin);
        NVIC_ClearPendingIRQ(EXTI1_IRQn);
        HAL_NVIC_EnableIRQ(EXTI1_IRQn);
        __HAL_GPIO_EXTI_CLEAR_IT(BTN_RVS_Pin);
        NVIC_ClearPendingIRQ(EXTI4_IRQn);
        HAL_NVIC_EnableIRQ(EXTI4_IRQn);
        __HAL_GPIO_EXTI_CLEAR_IT(BTN_FWD_Pin);
        NVIC_ClearPendingIRQ(EXTI3_IRQn);
        HAL_NVIC_EnableIRQ(EXTI3_IRQn);
        __HAL_GPIO_EXTI_CLEAR_IT(BTN_RIGHT_Pin);
        NVIC_ClearPendingIRQ(EXTI0_IRQn);
        HAL_NVIC_EnableIRQ(EXTI0_IRQn);
    }
    inline void initADC(){
//        HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
//        HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);
//        HAL_ADC_Start(&hadc2);
//        HAL_ADCEx_MultiModeStart_DMA(&hadc1, (uint32_t*)&adc_values, sizeof(adc_values)/2);
    }
    inline void initHALLSensors(){
        HAL_TIM_IC_Start_DMA(&htim1, TIM_CHANNEL_2, (uint32_t*)L_HALL_values, sizeof(L_HALL_values));
        HAL_TIM_IC_Start_DMA(&htim8, TIM_CHANNEL_2, (uint32_t*)R_HALL_values, sizeof(R_HALL_values));
    }
    /**
     * @brief all chip configured specs should be managed here in one place
     *        vars for DMA are set as global in the head
     */
    void initPerf(){
        EXTI_clear_enable();
        wardCarrier.initDevice();
        wardCarrier.getMovController().initMotors(&htim3, &htim4);
        initHALLSensors();
        wardCarrier.getMovController().setHALLSensors(L_HALL_values, R_HALL_values);
        initADC();
        wardCarrier.getMovController().setADCSensors(adc_values);
        HAL_TIM_Base_Start_IT(&htim15);
    }

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

    void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
    {
        if(hadc->Instance == ADC1)
            wardCarrier.getMovController().checkCurrent();
    }

    void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
    {
        if(htim->Instance == TIM15){
//            HAL_ADCEx_MultiModeStart_DMA(&hadc1, adc_values, sizeof(adc_values)/2);
            wardCarrier.update();
        }
        if(htim->Instance == TIM2){
    //        HAL_IWDG_Refresh(&hiwdg);
        }
    }

    void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
    {
        if(htim->Instance == TIM3 || htim->Instance == TIM4)
            wardCarrier.motor_refresh(htim);
    }

    void while1_in_mainCpp(){

    }
};