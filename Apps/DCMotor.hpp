//
// Created by outlaw on 22.08.2022.
//

#ifndef WARD_CARRIER_DCMOTOR_HPP
#define WARD_CARRIER_DCMOTOR_HPP

#include "functional"

struct MotorCfg
{
    float Vmin;
    float Vmax;
    float A;
    TIM_HandleTypeDef *htim;
    uint32_t timChannel_L;
    uint32_t timChannel_R;
    uint32_t * adc_value_param1;
    uint32_t * adc_value_param2;
};

class DCMotor{
    using MOTOR_IOS = PIN<MOTOR_OUTS, PinWriteable>;
    using Collable = std::function<void (DCMotor* motor)>;
public:
    Collable callBackOnEvent;

    enum MOTOR_DIRECTION{
        BACKWARDS = 0,
        FORWARD = 1
    };

    enum MODE
    {
        IDLE,
        ACCEL,
        CONST,
        DECCEL,
        in_ERROR
    };

    DCMotor() = delete;
    DCMotor(MOTOR_IOS& R_EN_pin, MOTOR_IOS& L_EN_pin, MOTOR_IOS& R_PWM_pin, MOTOR_IOS& L_PWM_pin):
            R_EN(R_EN_pin), L_EN(L_EN_pin), R_PWM(R_PWM_pin), L_PWM(L_PWM_pin)
    {
    }

    void init(MotorCfg &config, Collable incomeFunc){
        Vmin = config.Vmin;
        Vmax = config.Vmax;
        htim = config.htim;
        timChannel_l = config.timChannel_L;
        timChannel_r = config.timChannel_R;
        adc_value_f = config.adc_value_param1;
        adc_value_r = config.adc_value_param2;
        callBackOnEvent = std::move(incomeFunc);
    }

    inline void motor_OnTimer(){
        if(mode == in_ERROR) return;
        if(accelerationMode) {
            reCalcSpeed();
            regValueCalc();
        }
        else currentStepPP();
    }

    float getCurrent() const {
        return current;
    }

    inline void stopMotor(){
        if(motorMoving){
            HAL_TIM_PWM_Stop_IT(htim, timChannel_l);
            HAL_TIM_PWM_Stop_IT(htim, timChannel_r);
            L_EN.setValue(LOW);
            motorMoving = false;
            mode = MODE::IDLE;
            event = EVENT_STOP;
            callBackOnEvent(this);
        }
    }

    inline void changeDirection(){
        setDirection(currentDirection ? BACKWARDS : FORWARD);
    }

    [[nodiscard]] inline MOTOR_EVENT getEvent() const {
        return event;
    }

protected:
private:
    MOTOR_IOS R_EN;
    MOTOR_IOS L_EN;
    MOTOR_IOS R_PWM;
    MOTOR_IOS L_PWM;

    float V = 0.0f;
    float Vmin;
    float Vmax;
    float current;

    int currentStep = 0;
    int accel_step = 0;

    MOTOR_DIRECTION currentDirection = FORWARD;
    MODE mode = IDLE;
    MOTOR_EVENT event = EVENT_STOP;
    bool motorMoving = false;
    bool accelerationMode = false;
    bool directionInverted = false;

    TIM_HandleTypeDef *htim;
    uint32_t timChannel_l;
    uint32_t timChannel_r;

    uint32_t * adc_value_f;
    uint32_t * adc_value_r;

    inline void startMotor(MOTOR_DIRECTION direction){
        accel_step = 0;
        currentStep = 0;
        mode = MODE::ACCEL;
        motorMoving = true;
        regValueCalc();
        if(direction){
            R_EN.setValue(LOW);
            L_EN.setValue(LOW);
            HAL_TIM_PWM_Start_IT(htim, timChannel_l);
        }else{
            R_EN.setValue(HIGH);
            L_EN.setValue(HIGH);
            HAL_TIM_PWM_Start_IT(htim, timChannel_l);
        }
    }

    inline void currentStepPP(){
        currentStep++;
        callBackOnStep(this);
    }

    inline void regValueCalc(){
        if(V > 0){
            int buf = (int) (1000000 / V);
            if(buf > 0 && buf < 65535){
                __HAL_TIM_SET_AUTORELOAD(htim, buf);
                __HAL_TIM_SET_COMPARE(htim, timChannel, buf/2);
            }
        }
    }

    inline void setDirection(MOTOR_DIRECTION newDirection){
        currentDirection = newDirection;
        if(directionInverted) direction.setValue(LOGIC_LEVEL((currentDirection ? BACKWARDS : FORWARD)));
        else direction.setValue(LOGIC_LEVEL(currentDirection));
    }

    inline void reCalcSpeed(){
        if (mode == IDLE) return;
        switch (mode)
        {
            case MODE::ACCEL:
            {
                if (V >= Vmax)
                {
                    V = Vmax;
                    event = EVENT_CSS;
                    mode = MODE::CONST;
                }else
                    V += A;
//                  V += A / abs(V);
                if (accel_step >= stepsToGo / 2)
                {
                    mode = MODE::DECCEL;
                    break;
                }
                accel_step++;
            }
                break;

            case MODE::CONST:
            {
                if (currentStep + accel_step >= stepsToGo) {
                    mode = MODE::DECCEL;
//                    event = EVENT_CSS;
                }
            }
                break;

            case MODE::DECCEL:
            {
                if (accel_step <= 0)
                {
                    stopMotor();
                    mode = MODE::IDLE;
                    event = EVENT_STOP;
//                  changeDirection();
                    break;
                }else{
//                  V -= A/abs(V);
                    V -= A;
                    if (V < Vmin) V = Vmin;
                    accel_step--;
                }
            }
                break;

            default:
                break;
        }

        if (mode == ACCEL || mode == CONST || mode == DECCEL)
            currentStepPP();
    }

};

#endif //WARD_CARRIER_DCMOTOR_HPP
