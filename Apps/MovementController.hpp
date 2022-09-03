//
// Created by AlexKDR on 28.08.2022.
//

#ifndef WARD_CARRIER_MOVEMENTCONTROLLER_HPP
#define WARD_CARRIER_MOVEMENTCONTROLLER_HPP

extern uint32_t adc_value_left_param1;
extern uint32_t adc_value_left_param2;
extern uint32_t adc_value_right_param1;
extern uint32_t adc_value_right_param2;

extern uint32_t L_HALL_values[3];
extern uint32_t R_HALL_values[3];

class MovementController{
    using MOTOR_PIN = PIN<MOTOR_OUTS, PinWriteable>;
    enum DEVICE_DIRECTION{
        TURN = 0,
        STRAIGHT = 1,
    };
public:
    void initMotors(){
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

protected:
private:
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
