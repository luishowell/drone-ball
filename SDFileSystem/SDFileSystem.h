/**
 * @file SDFileSystem.h
 * @author Joe Walker
 * @brief The class SDFileSystem provides methods for controlling an SD card deriving from the SDBlockDevice and FATFileSystem classes.
 * @version 0.1
 * @date 2019-03-19
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef SDFileSystem__
#define SDFileSystem__


#include "FATFileSystem.h"	//SD card
#include "SDBlockDevice.h"	//SD card
#include "bitmap_image.hpp"	//bitmap lib
#include <stdio.h>
#include <errno.h> 
#include <string>
#include "mbed-os/mbed.h"


/**
 * @brief: Struct containing the pins on the sd card
 *
 */
typedef struct sdPins_t {
    PinName MOSI;
    PinName MISO;
    PinName SCLK;
    PinName CS;
} sdPins_t;



class SDFileSystem
{

    public:

        /**
         * @brief SDFileSystem object constructor
         * 
         * @param pins Struct containing the pins used for the SD card
         */
        SDFileSystem(sdPins_t pins);

        /**
         * @brief SDFileSystem object constructor
         * 
         * @param pins Struct containing the pins used for the SD card
         * @param pc Pointer to a serial connection for pc.printf for debug
         */
        SDFileSystem(sdPins_t pins, RawSerial* pc);


        /**
         * @brief getDirectory Function to get the specified directory and print to screen
         *
         * @param path Path to the director 
         */
        void getDirectory(const char * path);

        /**
         * @brief changeImage Function to change the bitmap to a given file
         * 
         * @param image The bitmap image object
         * @param filename The name of the file that is to be changed to
         */
        void changeImage(bitmap_image& image, std::string filename);

        
        /**
         * @brief getBmpFileList Function to get a list of bmp files in a given file.
         * 
         * @param path Path in which the bmp files are housed
         * @return The file containing the list
         */
        FILE *getBmpFileList(const char * path);


        /**
         * @brief getSelectedFilename Function that gives the filename of the selected file 
         *
         *
         * @param srchFile Filename of the file to be searched
         * @param index The index to be selected 
         */
        std::string getSelectedFilename(const char * srchFile, int index);


    private:

        //classes for the file system and block device
        SDBlockDevice *sd;
        FATFileSystem *fs;

        //store the pins internally
        sdPins_t *m_pins;

        //the internal functions needed for error handling
        void errno_error(void* ret_val);
        void return_error(int ret_val);

        //function to remove ._ files from directory
        void cleanDirectory(const char * path);

        //pc print if needed
        RawSerial* m_pc;

        //util values
        int err;
        bool pcConnected;

        //file to store bmp files list?
        FILE *m_bmpFiles;


};

#endif