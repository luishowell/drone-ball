#include "mbed-os/mbed.h"
#include "stepper.hpp"

Timer step_timer;

// Stepper::Stepper(int number_of_steps, PinName motor_pin_1, PinName motor_pin_2)
// : motor_1(motor_pin_1), motor_2(motor_pin_2)
// {
//     this->step_number = 0;                   // which step the motor is on
//     this->direction = 0;                     // motor direction
//     this->last_step_time = 0;                // time stamp in us of the last step taken
//     this->number_of_steps = number_of_steps; // total number of steps for this motor

//     // pin_count is used by the stepMotor() method:
//     this->pin_count = 2;
// }

/*
*   constructor for four-pin version
*   Sets which wires should control the motor.
*/
Stepper::Stepper(int number_of_steps, PinName motor_pin_1, PinName motor_pin_2, PinName motor_pin_3, PinName motor_pin_4)
: motor_1(motor_pin_1), motor_2(motor_pin_2), motor_3(motor_pin_3), motor_4(motor_pin_4)
{
    this->step_number = 0;                   // which step the motor is on
    this->direction = 0;                     // motor direction
    this->last_step_time = 0;                // time stamp in us of the last step taken
    this->number_of_steps = number_of_steps; // total number of steps for this motor

    this->motor_1 = 0;
    this->motor_2 = 0;
    this->motor_3 = 0;
    this->motor_4 = 0;

    // pin_count is used by the stepMotor() method:
    this->pin_count = 4;

    this->setSpeed(100);

    step_timer.start();
}

/*
* Sets the speed in revs per minute
*/
void Stepper::setSpeed(long whatSpeed)
{
    this->step_delay = 60L * 1000L * 1000L / this->number_of_steps / whatSpeed;
}

/*
* Moves the motor steps_to_move steps.  If the number is negative,
* the motor moves in the reverse direction.
*/
void Stepper::step(int steps_to_move)
{
    int steps_left = abs(steps_to_move); // how many steps to take

    //determine direction based on whether steps_to_mode is + or -:
    if (steps_to_move > 0)
    {
        this->direction = 1;
    }
    if (steps_to_move < 0)
    {
        this->direction = 0;
    }

    // decrement the number of steps, moving one step each time:
    while (steps_left > 0)
    {       
        // move only if the appropriate delay has passed:
        
        if (step_timer.read_us() >= this->step_delay)
        {
            //pc.printf("%i\n", step_timer.read_us());
         
            // increment or decrement the step number,
            // depending on direction:
            if (this->direction == 1)
            {
                this->step_number++;
                if (this->step_number == this->number_of_steps)
                {
                    this->step_number = 0;
                }
            }
            else
            {
                if (this->step_number == 0)
                {
                    this->step_number = this->number_of_steps;
                }
                this->step_number--;
            }
            // decrement the steps left:
            steps_left--;
            // step the motor to step number 0, 1, ..., {3 or 10}
            step_timer.stop();
            step_timer.reset();
            step_timer.start();

            stepMotor(this->step_number % 4);    

        }
    }
}

/*
* Moves the motor forward or backwards.
*/
void Stepper::stepMotor(int thisStep)
{
    // if (this->pin_count == 2)
    // {
    //     switch (thisStep)
    //     {
    //     case 0: // 01
    //         digitalWrite(motor_pin_1, LOW);
    //         digitalWrite(motor_pin_2, HIGH);
    //         break;
    //     case 1: // 11
    //         digitalWrite(motor_pin_1, HIGH);
    //         digitalWrite(motor_pin_2, HIGH);
    //         break;
    //     case 2: // 10
    //         digitalWrite(motor_pin_1, HIGH);
    //         digitalWrite(motor_pin_2, LOW);
    //         break;
    //     case 3: // 00
    //         digitalWrite(motor_pin_1, LOW);
    //         digitalWrite(motor_pin_2, LOW);
    //         break;
    //     }
    // }
    if (this->pin_count == 4)
    {
        switch (thisStep)
        {
        case 0: // 1010
            this->motor_1 = 1;
            this->motor_2 = 0;
            this->motor_3 = 1;
            this->motor_4 = 0;
            break;
        case 1: // 0110
            this->motor_1 = 0;
            this->motor_2 = 1;
            this->motor_3 = 1;
            this->motor_4 = 0;
            break;
        case 2: //0101
            this->motor_1 = 0;
            this->motor_2 = 1;
            this->motor_3 = 0;
            this->motor_4 = 1;
            break;
        case 3: //1001
            this->motor_1 = 1;
            this->motor_2 = 0;
            this->motor_3 = 0;
            this->motor_4 = 1;
            break;
        }
    }
}