//
// Created by AlexKDR on 28.08.2022.
//

#ifndef WARD_CARRIER_MOVEMENTCONTROLLER_HPP
#define WARD_CARRIER_MOVEMENTCONTROLLER_HPP

#define DIFF_CRITICAL_VALUE 200

class MovementController{
    using MOTOR_PIN = PIN<MOTOR_OUTS, PinWriteable>;
public:
    enum DEVICE_DIRECTION{
        STOP = 0,
        FORWARD_DIRECTION = 1,
        BACKWARDS_DIRECTION = 2
    };
    enum DEVICE_SIDE{
        CENTER = 0,
        TURN_LEFT = 1,
        TURN_RIGHT = 2,
    };

    void initMotors(TIM_HandleTypeDef *htim_l, TIM_HandleTypeDef *htim_r){
        MotorCfg cfgLeft{};
        cfgLeft.mSecAccTime = 100;
        cfgLeft.maxRatio = 0.4;
        cfgLeft.htim = htim_l;
        cfgLeft.timChannel_L = TIM_CHANNEL_1;
        cfgLeft.timChannel_R = TIM_CHANNEL_2;
        left_motor.init(cfgLeft, [this](DCMotor* m){OnMotorEvent(m);});

        MotorCfg cfgRight{};
        cfgRight.mSecAccTime = 100;
        cfgRight.maxRatio = 0.4;
        cfgRight.htim = htim_r;
        cfgRight.timChannel_L = TIM_CHANNEL_1;
        cfgRight.timChannel_R = TIM_CHANNEL_2;
        right_motor.init(cfgRight, [this](DCMotor* m){OnMotorEvent(m);});
    }

    template<uint8_t N>
    constexpr void setADCSensorsVars(uint32_t(&adc_values_in)[N]){
        adc_values = adc_values_in;
        adc_values_size = N;
    }

    template<uint8_t N>
    constexpr void setHALLSensorsVars(uint32_t(&hall_l)[N], uint32_t (&hall_r)[N]){
        l_hall_values = hall_l;
        r_hall_values = hall_r;
        hall_values_size = N;
    }

    void OnMotorEvent(DCMotor *motor){
        MOTOR_EVENT event = motor->getEvent();
    }

    void OnMotorStep(DCMotor *motor){
    }

    inline void moveForward(){
        currentDirection = FORWARD_DIRECTION;
        right_motor.move(DCMotor::FORWARD);
        left_motor.move(DCMotor::FORWARD);
    }

    inline void moveBackwards(){
        currentDirection = BACKWARDS_DIRECTION;
        right_motor.move(DCMotor::BACKWARDS);
        left_motor.move(DCMotor::BACKWARDS);
    }

    void turnLeft(){
        if(currentSide == TURN_LEFT) finishTurn();
        else{
            currentSide = TURN_LEFT;
            left_motor.move(DCMotor::BACKWARDS);
            right_motor.move(DCMotor::FORWARD);
        }
    }

    void turnRight(){
        if(currentSide == TURN_RIGHT) finishTurn();
        else{
            currentSide = TURN_RIGHT;
            left_motor.move(DCMotor::FORWARD);
            right_motor.move(DCMotor::BACKWARDS);
        }
    }

    void straightDirection(){
        right_motor.fullSpeed();
        left_motor.fullSpeed();
    }

    void finishTurn(){
        currentSide = CENTER;
        if(currentDirection == STOP) stop();
        else{
            if(currentDirection == FORWARD_DIRECTION) moveForward();
            else if(currentDirection == BACKWARDS_DIRECTION) moveBackwards();
        }
    }

    void stop(){
        currentDirection = STOP;
        right_motor.slowDown();
        left_motor.slowDown();
    }

    void update(){
        if(currentDirection == FORWARD_DIRECTION){
            checkHALLSensors();
        }
    }

    inline void checkHALLSensors(){
        uint32_t AveragePulseL = 0;
        uint32_t AveragePulseR = 0;
        for(uint8_t i = 0; i < hall_values_size; i++){
            AveragePulseL += (uint32_t)(*l_hall_values);
            AveragePulseR += (uint32_t)(*r_hall_values);
        }
        AveragePulseL /= hall_values_size;
        AveragePulseR /= hall_values_size;
        uint32_t diff = AveragePulseL > AveragePulseR ? (AveragePulseL - AveragePulseR) : (AveragePulseR - AveragePulseL);
//        if(diff > DIFF_CRITICAL_VALUE){
//            if(AveragePulseL > AveragePulseR) left_motor.speedCorrection(calcSpeedDelta(diff));
//            else right_motor.speedCorrection(calcSpeedDelta(diff));
//        }
    }

    uint16_t calcSpeedDelta(uint32_t diff){
        return 0;
    }

    inline void checkCurrent(){
        if(adc_values_size >= 4){
            if(right_motor.getDirection()){
                uint16_t adc_r_1 = adc_values[0];
                uint16_t adc_l_1 = adc_values[2];
            }else{
                uint16_t adc_r_2 = adc_values[1];
                uint16_t adc_l_2 = adc_values[3];
            }
        }else static_assert("check adc values array size");
    }

    /**
     * @brief stop PWM generation. To normally stop use slowDown()
     */
    void carrierForcedStop(){
        left_motor.forcedStop();
        right_motor.forcedStop();
    }

    void motor_refresh(TIM_HandleTypeDef *check_htim) {
        if(right_motor.isMyTimer(check_htim)) right_motor.motor_OnTimer();
        else if(left_motor.isMyTimer(check_htim)) left_motor.motor_OnTimer();
    }

    DEVICE_DIRECTION getDeviceDirection(){
        return currentDirection;
    }

protected:
private:
    uint32_t* l_hall_values;
    uint32_t* r_hall_values;
    uint8_t hall_values_size;
    uint32_t* adc_values;
    uint8_t adc_values_size;

    DEVICE_DIRECTION currentDirection = STOP;
    DEVICE_SIDE currentSide = CENTER;
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

#endif //WARD_CARRIER_MOVEMENTCONTROLLER_HPP
