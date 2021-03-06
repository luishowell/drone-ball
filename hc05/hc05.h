/**
 * @file hc05.h
 * @author Joe Walker
 * @brief The class hc05 provides methods for controlling the hc05 bluetooth module.
 * @version 0.1
 * @date 2019-03-20
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef hc05__
#define hc05__

#include "mbed-os/mbed.h"
#include <string>

class hc05
{
    public:

        /**
         * @brief hc05 object constructor
         * 
         * @param tx Transfer pin
         * @param rx Receive pin
         */
        hc05(PinName tx, PinName rx);


        /**
         * @brief hc05 object constructor
         * 
         * @param tx Transfer pin
         * @param rx Receive pin
         * @param pc Pointer to a serial connection for pc.printf for debug
         */
        hc05(PinName tx, PinName rx, RawSerial* pc);


        /**
         * @brief Function to retrieve the received character
         * 
         * @return Character that has been read
         */
        char readCharacter();

        /**
         * @brief Function to send a character
         * 
         * @param toSend The character to be sent
         */
        void sendCharacter(char toSend);

        /**
         * @brief Function to send a float
         * 
         * @param toSend The flaot to be sent
         */
        void sendFloat(float toSend);

        /**
         * @brief Function to send a file over bluetooth
         * 
         * @param fSend File that is to be sent
         */
        void sendFile(FILE *fSend);

        /**
         * @brief Function to receive a filename of give type
         * 
         * @param extension String containing the desired file extension to look for
         */
        string receiveFilename(string extension);

        //bluetooth object
        RawSerial *m_bt;        


    private:

        //save the pins internally
        PinName m_tx;
        PinName m_rx;

        //pc print if needed
        RawSerial* m_pc;

        //util values
        int err;
        bool pcConnected;

};

#endif