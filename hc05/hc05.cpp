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

//get the character
char hc05::readCharacter()
{
    //get the character from the serial port
    //TODO: replace with bluetooth thing
    char val = m_pc->getc();

    return val;
}