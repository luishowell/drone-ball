#ifndef mux_h
#define mux_h

#include "mbed.h"


class Mux
{
public:
    
    Mux(PinName selA_pin, PinName selB_pin);

    void set_output(int output_sel);

private:

    DigitalOut selA;
    DigitalOut selB;    
};

#endif