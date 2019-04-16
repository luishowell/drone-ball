#include "hc05.h"


//constructor
hc05::hc05(PinName tx, PinName rx)
{
    //save the pins
    this->m_tx = tx;
    this->m_rx = rx;

    //create the bluetooth element
    m_bt = new RawSerial(tx, rx);

    //theres no pc
    this->pcConnected = false;

}

//constructor
hc05::hc05(PinName tx, PinName rx, RawSerial* pc)
{
    //save the pins
    this->m_tx = tx;
    this->m_rx = rx;

    //create the bluetooth element
    m_bt = new RawSerial(tx, rx);

    //save the pc value
    this->m_pc = pc;
    this->pcConnected = true;

}

//get the character
char hc05::readCharacter()
{
    //get the character from the serial port
    char val = this->m_bt->getc();

    return val;
}


//send a file
void hc05::sendFile(FILE *fSend)
{
    if(this->pcConnected == true){
        this->m_pc->printf("Sending file over bluetooth\n");
    }
    //send the file
    //TODO replace to send over m_bt
    char buff[1] = {0};
	while (!feof(fSend)){
		int size = fread(&buff[0], 1, 1, fSend);
        if(this->pcConnected == true){
            this->m_pc->printf("%c",buff[0]);
        }
	}

}