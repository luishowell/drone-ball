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

//send a character
void hc05::sendCharacter(char toSend)
{
    //send the character to the serial port
    this->m_bt->putc(toSend);

}


//send a float
void hc05::sendFloat(float toSend)
{
    //send the float to the serial port
    this->m_bt->printf("%3.2f", toSend);
}


//send a file
void hc05::sendFile(FILE *fSend)
{
    if(this->pcConnected == true){
        this->m_pc->printf("Sending file over bluetooth\n");
    }
    //send the file
    char buff[1] = {0};
	while (!feof(fSend)){
		int size = fread(&buff[0], 1, 1, fSend);
        this->m_bt->printf("%c",buff[0]);
        if(this->pcConnected == true){
            this->m_pc->printf("%c",buff[0]);
        }
	}

}


//receive a filename
string hc05::receiveFilename(string extension)
{
    if(this->pcConnected == true){
        this->m_pc->printf("Receiving the filename\n");
    }
    char buff;
    bool complete = false;
    string filename = "";
    while(complete == false)
    {
        buff = this->readCharacter();
        filename += buff;

        //check if the filename contains the extension
        if(filename.find(extension) != std::string::npos){
            complete = true;
        }
    }
    return filename;

}