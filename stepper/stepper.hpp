/*
 * The sequence of control signals for 4 control wires is as follows:
 *
 * Step C0 C1 C2 C3
 *    1  1  0  1  0
 *    2  0  1  1  0
 *    3  0  1  0  1
 *    4  1  0  0  1
 *
 * The sequence of controls signals for 2 control wires is as follows
 * (columns C1 and C2 from above):
 *
 * Step C0 C1
 *    1  0  1
 *    2  1  1
 *    3  1  0
 *    4  0  0
 *
 * The circuits can be found at
 *
 * http://www.arduino.cc/en/Tutorial/Stepper
 */

// ensure this library description is only included once
#ifndef Stepper_h
#define Stepper_h

#include "mbed-os/mbed.h"

// library interface description
class Stepper {
  public:
    // constructors:
    Stepper(int number_of_steps, PinName motor_pin_1, PinName motor_pin_2);
    Stepper(int number_of_steps, PinName motor_pin_1, PinName motor_pin_2,PinName motor_pin_3, PinName motor_pin_4);

    // speed setter method:
    void setSpeed(long whatSpeed);

    // mover method:
    void step(int number_of_steps);
 

  private:
    void stepMotor(int this_step);

    int direction;            // Direction of rotation
    unsigned long step_delay; // delay between steps, in ms, based on speed
    int number_of_steps;      // total number of steps this motor can take
    int pin_count;            // how many pins are in use.
    int step_number;          // which step the motor is on
    
    DigitalOut motor_1;
    DigitalOut motor_2;
    DigitalOut motor_3;
    DigitalOut motor_4;

    unsigned long last_step_time; // time stamp in us of when the last step was taken
};

#endif

