//
// Created by outlaw on 22.08.2022.
//

#ifndef WARD_CARRIER_DCMOTOR_HPP
#define WARD_CARRIER_DCMOTOR_HPP

#include "functional"

struct MotorCfg
{
    int Vmin;
    int Vmax;
    int A;
    TIM_HandleTypeDef *htim;
    uint32_t timChannel_L;
    uint32_t timChannel_R;
};

class DCMotor{
    using MOTOR_IOS = PIN<MOTOR_OUTS, PinWriteable>;
    using Collable = std::function<void (DCMotor* motor)>;
public:
    Collable callBackOnEvent;
    Collable callBackOnStep;

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

    int getCurrent() const {
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

    inline bool isMyTimer(TIM_HandleTypeDef *income_htim) const{
        return htim == income_htim;
    }

    void slowDown(uint8_t value){

    }
    void fullSpeed(){

    }

    [[nodiscard]] inline bool isMotorMoving() const{
        return motorMoving;
    }

    inline void move(MOTOR_DIRECTION dir){
        startMotor(dir);
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

    int V = 0;
    int Vmin;
    int Vmax;
    int A;
    int current;

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

    inline void startMotor(MOTOR_DIRECTION direction){
        accel_step = 0;
        currentStep = 0;
        mode = MODE::ACCEL;
        motorMoving = true;
        regValueCalc();
        if(direction){
            R_EN.setValue(HIGH);
            L_EN.setValue(HIGH);
            HAL_TIM_PWM_Start_IT(htim, timChannel_l);
        }else{
            R_EN.setValue(HIGH);
            L_EN.setValue(HIGH);
            HAL_TIM_PWM_Start_IT(htim, timChannel_r);
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
                __HAL_TIM_SET_COMPARE(htim, timChannel_l, buf/2);
                __HAL_TIM_SET_COMPARE(htim, timChannel_r, buf/2);
            }
        }
    }

    inline void setDirection(MOTOR_DIRECTION newDirection){
        currentDirection = newDirection;
//        if(directionInverted) direction.setValue(LOGIC_LEVEL((currentDirection ? BACKWARDS : FORWARD)));
//        else direction.setValue(LOGIC_LEVEL(currentDirection));
    }

    inline void reCalcSpeed(){
        if (mode == IDLE) return;
        switch (mode)
        {
            case MODE::ACCEL:
            if (V >= Vmax)
            {
                V = Vmax;
                event = EVENT_CSS;
                mode = MODE::CONST;
            }else
                V += A;
            accel_step++;
                break;

            case MODE::CONST:
                break;

            case MODE::DECCEL:
            if (accel_step <= 0)
            {
                stopMotor();
                mode = MODE::IDLE;
                event = EVENT_STOP;
                break;
            }else{
                V -= A;
                if (V < Vmin) V = Vmin;
                accel_step--;
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
