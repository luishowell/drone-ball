#include "mux.hpp"


Mux::Mux(PinName selA_pin, PinName selB_pin, PinName disable_pin)
: selA(selA_pin), selB(selB_pin), disable(disable_pin)
{
    disable = 1;
    selA = 0;
    selB = 0;
}

void Mux::set_output(int output_sel)
{
    switch (output_sel)
    {
        case 0:
            selA = 0;
            selB = 0;
            break;
        case 1:
            selA = 1;
            selB = 0;
            break;
        case 2:
            selA = 0;
            selB = 1;
            break;
        case 3:
            selA = 1;
            selB = 1;
            break;
    
        default:
            break;
    }
}

void Mux::enable_output(bool en)
{
    disable = !en;
}