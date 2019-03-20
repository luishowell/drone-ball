#include "hc05.h"


//constructor
hc05::hc05(PinName tx, PinName rx)
{
    //save the pins
    this->m_tx = tx;
    this->m_rx = rx;

    //create the bluetooth element
    bt = new Serial(tx, rx);

    //theres no pc
    this->pcConnected = false;

}

//constructor
hc05::hc05(PinName tx, PinName rx, Serial* pc)
{
    //save the pins
    this->m_tx = tx;
    this->m_rx = rx;

    //create the bluetooth element
    bt = new Serial(tx, rx);

    //save the pc value
    this->m_pc = pc;
    this->pcConnected = true;

}