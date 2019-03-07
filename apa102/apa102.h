 /*
Copyright (c) 2011 Thomas Olsson.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
 
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
 
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef APA102_H
#define APA102_H

#include "mbed.h"
#include <BurstSPI.h>

#define APA102_MAX_LEVEL 100


class apa102 
{
public:
    /** Create a new apa102 object
    * 
    *     
    * @param SCLK_pn SPI clock pin
    * @param MOSI_pn SPI MOSI pin
    * @param MISO_pn SPI MISO pin
    * @param STRIP_LENGTH number of apa102 IC's i strip or array defaults to 144
    * @param reset_delay delay in us to allow latching data defaults to 0
    * @returns nothing   
    */
    apa102(PinName SCLK_pn, PinName MOSI_pn, PinName MISO_pn, int STRIP_LENGTH=144, int reset_delay=0);
    
    /** write RGB color data to strip or array
    * 
    * color data for each LED is 3 bytes int the order of rrggbb.
    * array must have STRIP_LENGTH number of elements
    *
    * @param strip_colors array of color data
    */
    void post(unsigned int *strip_colors);
    
    /** clears the array or strip (all off)
    */    
    void clear(void);

    /** set level of the entire array 0-100%
    * 
    * at low levels the colors may change because R,G or B
    * reaches 0.
    *
    * @param level level in percent
    * @returns current level
    */
    int level(int level=0);
    
    /** 
    * 
    * @brief Set the SPI frequency
    *
    * @param frequency Desired SPI frequency in Hz
    */    
    void setFrequency(uint32_t frequency);
  
private:  
    int _STRIP_LENGTH;
    int _level;
    int _reset_delay;

    BurstSPI spi_class;

    inline void send_start_frame();
    inline void send_end_frame();       
};

#endif