//
// Created by outlaw on 22.08.2022.
//

#ifndef WARD_CARRIER_DCMOTOR_HPP
#define WARD_CARRIER_DCMOTOR_HPP

#include "functional"

#define MAX_SPEED_RATIO     1.0
#define MAX_FREQ_VALUE      500
#define PULSE_WIDTH_ONLY    true


struct MotorCfg
{
    float maxRatio;
    uint32_t mSecAccTime;
    TIM_HandleTypeDef *htim;
    uint32_t timChannel_L;
    uint32_t timChannel_R;
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
    {}

    void init(MotorCfg &config, Collable incomeFunc){
        htim = config.htim;
        timChannel_l = config.timChannel_L;
        timChannel_r = config.timChannel_R;
        configMaxRation = config.maxRatio;
        mSecAccelTime = config.mSecAccTime;
        callBackOnEvent = std::move(incomeFunc); //TODO check here!
        timerFreq = SystemCoreClock / (htim->Instance->PSC);
        timerARR = __HAL_TIM_GET_AUTORELOAD(htim);
        setMaxSpeedRatio(config.maxRatio);
    }

    /**
     * @brief calculating addition factor for acceleration and braking which will be
     * used for pwm generation registers calculation in regValueCalc() function
     */
    void speedAdderCalc() {
        uint32_t pulseFreq = timerFreq / timerARR;
        float pulseCount = pulseFreq * (float(mSecAccelTime)/1000);
        if (pulseCount > 1)
            speedAdder = maxRatio / pulseCount;
        else{
            static_assert("too low pulse freq");
            speedAdder = MAX_SPEED_RATIO;
        }
    }

    inline void motor_OnTimer(){
        if(mode == in_ERROR) return;
        if(mode != CONST) {
            reCalcSpeed();
            regValueCalc();
        }
    }

    inline bool isMyTimer(TIM_HandleTypeDef *income_htim) const{
        return htim == income_htim;
    }

    void slowDown(float value = 0){
        if(value > 0 && value < maxRatio)
            setMaxSpeedRatio(maxRatio-value);
        mode = DECCEL;
        changingDir = false;
    }

    void speedCorrection(float newRatioValue){
        setMaxSpeedRatio(newRatioValue);
    }

    void setMaxSpeedRatio(float newValue){
        if(newValue < 0) maxRatio = 0;
        else if(newValue > MAX_SPEED_RATIO) maxRatio = MAX_SPEED_RATIO;
        else maxRatio = newValue;
        speedAdderCalc();
    }
    /**
     * @brief   restore Max value for SpeedRatio which was written in init block from config
     *          +make motor return to acceleration phase or normally start motor
     */
    void fullSpeed(){
        setMaxSpeedRatio(configMaxRation);
        if(motorMoving) mode = ACCEL;
        else startMotor(currentDirection);
    }

    [[nodiscard]] inline bool isMotorMoving() const{
        return motorMoving;
    }

    inline void move(MOTOR_DIRECTION incomeDir){
        if(motorMoving){
            if(incomeDir != currentDirection) changeDir();
            else mode = ACCEL;
        }else
            startMotor(incomeDir);
    }

    inline void changeDir(){
        changingDir = true;
        mode = DECCEL;
    }

    [[nodiscard]] inline MOTOR_EVENT getEvent() const {
        return event;
    }

    void forcedStop(){
        stopMotor();
    }

    inline MOTOR_DIRECTION getDirection()const {
        return currentDirection;
    }

protected:
private:
    MOTOR_IOS R_EN;
    MOTOR_IOS L_EN;
    MOTOR_IOS R_PWM;
    MOTOR_IOS L_PWM;

    float SpeedRatio = 0;
    float maxRatio = MAX_SPEED_RATIO;
    float configMaxRation = MAX_SPEED_RATIO;
    float speedAdder = 0;

    MOTOR_DIRECTION currentDirection = FORWARD;
    MODE mode = IDLE;
    MOTOR_EVENT event = EVENT_STOP;
    bool motorMoving = false;
    bool changingDir = false;

    TIM_HandleTypeDef *htim;
    uint32_t timChannel_l;
    uint32_t timChannel_r;
    uint32_t timerFreq;
    uint32_t timerARR;
    uint32_t mSecAccelTime;

    inline void startMotor(MOTOR_DIRECTION direction){
        currentDirection = direction;
        if(motorMoving){
            HAL_TIM_PWM_Stop_IT(htim, timChannel_l);
            HAL_TIM_PWM_Stop_IT(htim, timChannel_r);
        }
        SpeedRatio = 0;
        mode = MODE::ACCEL;
        motorMoving = true;
        regValueCalc();
        R_EN.setValue(HIGH);
        L_EN.setValue(HIGH);
        if(direction == FORWARD)
            HAL_TIM_PWM_Start_IT(htim, timChannel_l);
        else if(direction == BACKWARDS)
            HAL_TIM_PWM_Start_IT(htim, timChannel_r);
    }

    inline void stopMotor(){
            HAL_TIM_PWM_Stop_IT(htim, timChannel_l);
            HAL_TIM_PWM_Stop_IT(htim, timChannel_r);
            L_EN.setValue(LOW);
            R_EN.setValue(LOW);
            motorMoving = false;
            mode = MODE::IDLE;
            event = EVENT_STOP;
            callBackOnEvent(this);
    }
    /**
     * @brief  Calc and set values into pwm generation registers
     * @note   2 options here:
     *         if @PULSE_WIDTH_ONLY - only pulse width will be changed
     *         else freq of modulation will be changed with same 50% pulse width
     */
    inline void regValueCalc() {
        if(PULSE_WIDTH_ONLY){
            auto newCompareValue = (uint32_t)(timerARR * SpeedRatio);
            if(newCompareValue >= 0 && newCompareValue < UINT16_MAX){
                __HAL_TIM_SET_COMPARE(htim, timChannel_l, newCompareValue);
                __HAL_TIM_SET_COMPARE(htim, timChannel_r, newCompareValue);
            }
        }else{
            auto newARRValue = (uint32_t)(MAX_FREQ_VALUE * SpeedRatio);
            if(newARRValue > 0 && newARRValue < UINT16_MAX){
                __HAL_TIM_SET_AUTORELOAD(htim, newARRValue);
                __HAL_TIM_SET_COMPARE(htim, timChannel_l, newARRValue/2);
                __HAL_TIM_SET_COMPARE(htim, timChannel_r, newARRValue/2);
            }
        }
    }

    inline void reCalcSpeed(){
        if (mode == IDLE) return;
        switch (mode)
        {
            case MODE::ACCEL:
                SpeedRatio += speedAdder;
                if (SpeedRatio >= maxRatio){
                    SpeedRatio = maxRatio;
                    event = EVENT_CSS;
                    mode = MODE::CONST;
                }
            break;

            case MODE::CONST:
                break;

            case MODE::DECCEL:
                SpeedRatio -= speedAdder;
                if (SpeedRatio <= 0){
                    if(changingDir){
                        changingDir = false;
                        startMotor(currentDirection ? BACKWARDS : FORWARD);
                    }
                    else stopMotor();
                    break;
                }
            break;

            default:
                break;
        }
    }
};

#endif //WARD_CARRIER_DCMOTOR_HPP
