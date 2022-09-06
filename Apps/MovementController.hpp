//
// Created by AlexKDR on 28.08.2022.
//

#ifndef WARD_CARRIER_MOVEMENTCONTROLLER_HPP
#define WARD_CARRIER_MOVEMENTCONTROLLER_HPP
extern uint32_t adc_value_left_param1;
extern uint32_t adc_value_left_param2;
extern uint32_t adc_value_right_param1;
extern uint32_t adc_value_right_param2;

class MovementController{
    using MOTOR_PIN = PIN<MOTOR_OUTS, PinWriteable>;
    enum DEVICE_DIRECTION{
        TURN = 0,
        STRAIGHT = 1,
    };
public:
    void initMotors(TIM_HandleTypeDef *htim_l, TIM_HandleTypeDef *htim_r){
        MotorCfg cfgLeft{};
        cfgLeft.A = 30;
        cfgLeft.Vmax = 300;
        cfgLeft.htim = htim_l;
        cfgLeft.timChannel_L = TIM_CHANNEL_1;
        cfgLeft.timChannel_R = TIM_CHANNEL_2;
        left_motor.init(cfgLeft, [this](DCMotor* m){OnMotorEvent(m);});

        MotorCfg cfgRight{};
        cfgRight.A = 30;
        cfgRight.Vmax = 300;
        cfgRight.htim = htim_r;
        cfgRight.timChannel_L = TIM_CHANNEL_1;
        cfgRight.timChannel_R = TIM_CHANNEL_2;
        right_motor.init(cfgRight, [this](DCMotor* m){OnMotorEvent(m);});
    }

    void initADCSensors(uint32_t *adc_l_p1, uint32_t *adc_l_p2, uint32_t *adc_r_p1, uint32_t *adc_r_p2)
    {

    }

    template<uint8_t N>
    constexpr void initHALLSensors(uint32_t(&hall_l)[N], uint32_t (&hall_r)[N]){
        l_hall_values = hall_l;
        r_hall_values = hall_r;
        hall_values_size = N;
    }

    void OnMotorEvent(DCMotor *motor){
        MOTOR_EVENT event = motor->getEvent();
    }

    void OnMotorStep(DCMotor *motor){
        if(currentDirection){

        }
    }

    void moveForward(){
        currentDirection = STRAIGHT;
        right_motor.move(DCMotor::FORWARD);
        left_motor.move(DCMotor::FORWARD);
    }

    void moveBackwards(){
        currentDirection = STRAIGHT;
        right_motor.move(DCMotor::BACKWARDS);
        left_motor.move(DCMotor::BACKWARDS);
    }

    void turnLeft(){
        currentDirection = TURN;
        if(left_motor.isMotorMoving())
            left_motor.slowDown(10);
        else left_motor.move(DCMotor::FORWARD);
    }

    void turnRight(){
        currentDirection = TURN;
        if(right_motor.isMotorMoving())
            right_motor.slowDown(10);
        else right_motor.move(DCMotor::FORWARD);
    }

    void straightDirection(){
        right_motor.fullSpeed();
        left_motor.fullSpeed();
    }

    void stop(){
        right_motor.stopMotor();
        left_motor.stopMotor();
    }

    void motor_refresh(TIM_HandleTypeDef *check_htim) {
        if(right_motor.isMyTimer(check_htim)) right_motor.motor_OnTimer();
        else if(left_motor.isMyTimer(check_htim)) left_motor.motor_OnTimer();
    }

protected:
private:
    uint32_t* l_hall_values;
    uint32_t* r_hall_values;
    uint8_t hall_values_size;
    uint32_t* adc_l_v1;
    uint32_t* adc_l_v2;
    uint32_t* adc_r_v1;
    uint32_t* adc_r_v2;

    DEVICE_DIRECTION currentDirection;
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
