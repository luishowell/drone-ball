#include "stepper.hpp"

Timer step_timer;

Stepper::Stepper(int number_of_steps, PinName motor_pin_1, PinName motor_pin_2, PinName motor_pin_3, PinName motor_pin_4, PinName motor_en_pin_1, PinName motor_en_pin_2)
: motor_1(motor_pin_1), motor_2(motor_pin_2), motor_3(motor_pin_3), motor_4(motor_pin_4), motor_en_1(motor_en_pin_1), motor_en_2(motor_en_pin_2)
{
    this->step_number = 0;                   // which step the motor is on
    this->direction = 0;                     // motor direction
    this->last_step_time = 0;                // time stamp in us of the last step taken
    this->number_of_steps = number_of_steps; // total number of steps for this motor

    this->motor_1 = 0;
    this->motor_2 = 0;
    this->motor_3 = 0;
    this->motor_4 = 0;
    this->motor_en_1 = 1;
    this->motor_en_2 = 1;

    this->setSpeed(1);

    step_timer.start();
}

/*
* Sets the speed in revs per minute
*/
void Stepper::setSpeed(int whatSpeed)
{
    if (whatSpeed<1)
    {
        whatSpeed = 1;
    }
    this->step_delay = 60L * 1000L * 1000L / this->number_of_steps / whatSpeed;
    current_speed = whatSpeed;
}

long Stepper::calc_step_delay(int whatSpeed)
{
    if (whatSpeed<1)
    {
        whatSpeed = 1;
    }  
    return 60L * 1000L * 1000L / this->number_of_steps / whatSpeed;
}

void Stepper::ramp_speed(int start_speed, int end_speed, int dir)
{
    int speed_diff = end_speed-start_speed;

    float ramp_time = abs(speed_diff)*0.06;

    int speed_steps = int(ramp_time/0.5);
    if (speed_steps<1){speed_steps=1;}

    int speed_inc = speed_diff/speed_steps;

    long inc_time = static_cast<long>((ramp_time*1000*1000)/speed_steps); //in us

    if (dir>0)
    {
        dir = 1;
    }
    else
    {
        dir = -1;
    }    

    int new_speed = start_speed;
    int num_steps = 0;
    for (int i=0; i<speed_steps; i++)
    {
        new_speed = new_speed + speed_inc;

        // new motor bodge
        if ((new_speed>40)&&(new_speed<80)&&(end_speed>=80))
        {
            new_speed = 80;
        }

        num_steps = inc_time/calc_step_delay(new_speed);
        if (num_steps<1)
        {
            num_steps = 1;
        }
        this->setSpeed(new_speed);
        this->step(dir*num_steps);
    }

    this->setSpeed(end_speed);   
}

void Stepper::enable_motor(bool enable)
{
    if (enable)
    {
        this->motor_en_1 = 1;
        this->motor_en_2 = 1;
    }
    else
    {
        this->motor_en_1 = 0;
        this->motor_en_2 = 0;       
    }    
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
            step_timer.stop();
            step_timer.reset();
         
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
            stepMotor(this->step_number % 4);    

            step_timer.start();
        }
    }
}

/*
* Moves the motor forward or backwards.
*/
void Stepper::stepMotor(int thisStep)
{
    switch (thisStep)
    {
    case 0: // 0110
        this->motor_1 = 0;
        this->motor_2 = 1;
        this->motor_3 = 1;
        this->motor_4 = 0;
        break;
    case 1: //0101
        this->motor_1 = 0;
        this->motor_2 = 1;
        this->motor_3 = 0;
        this->motor_4 = 1;
        break;
    case 2: //1001
        this->motor_1 = 1;
        this->motor_2 = 0;
        this->motor_3 = 0;
        this->motor_4 = 1;
        break;
    case 3: // 1010
        this->motor_1 = 1;
        this->motor_2 = 0;
        this->motor_3 = 1;
        this->motor_4 = 0;
        break;
    }    
}