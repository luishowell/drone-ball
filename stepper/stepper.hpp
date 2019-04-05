#ifndef Stepper_h
#define Stepper_h

#include "mbed.h"


class Stepper {
  public:
    // constructor:
    Stepper(int number_of_steps, PinName motor_pin_1, PinName motor_pin_2, PinName motor_pin_3, PinName motor_pin_4, PinName motor_en_pin_1, PinName motor_en_pin_2);

    // speed setter method:
    void setSpeed(long whatSpeed);

    // mover method:
    void step(int number_of_steps);

    // accelerate or deccelerate the stepper, ramp time in s, speeds in rpm, dir 1 or -1 for direction
    void ramp_speed(int start_speed, int end_speed, int dir = -1, float ramp_time = 5.0, int speed_steps = 40); 

    //enable or disable the motor
    void enable_motor(bool enable);

  private:
    void stepMotor(int this_step);

    //retuns step delay in us
    long calc_step_delay(long whatSpeed);

    int direction;            // Direction of rotation
    long step_delay;          // delay between steps, in us, based on speed
    int number_of_steps;      // total number of steps this motor can take
    int pin_count;            // how many pins are in use.
    int step_number;          // which step the motor is on
    
    DigitalOut motor_1;
    DigitalOut motor_2;
    DigitalOut motor_3;
    DigitalOut motor_4;
    DigitalOut motor_en_1;
    DigitalOut motor_en_2;

    long last_step_time; // time stamp in us of when the last step was taken
};

#endif

