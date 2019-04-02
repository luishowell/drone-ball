#ifndef mux_h
#define mux_h

#include "mbed.h"


class Mux
{
public:
    
    Mux(PinName selA_pin, PinName selB_pin, PinName disable_pin);

    void set_output(int output_sel);

    void enable_output(bool en);

private:

    DigitalOut selA;
    DigitalOut selB;
    DigitalOut disable;
};

#endif