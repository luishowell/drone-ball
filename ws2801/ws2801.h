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

#ifndef WS2801_H
#define WS2801_H

#include "mbed.h"
#include "HTML_color.h"


/** WS2801 class, controlling LED strips or arrays like this http://www.sparkfun.com/products/10312
*
* the WS2801 IC's are 5V but works well connected directly to mbed IO pins.
*
* You need a 5V power supply capable of about 2A to light up a 32 led strip, do NOT power from mbed(usb).
*
* the required reset delay is a blocking wait(), it can be lowered (or set to 0) if you want to use the
* time for executing code instead, data sheet says 500us but i found it need at least 800us.
*
* Example:
* @code
#include "mbed.h"
#include "ws2801.h"

#define STRIP_LENGTH 32

ws2801 mystrip(p9, p10, STRIP_LENGTH);

int dir=1, level=10;
int rainbow[] = {0xff00ff,0xff00cc,0xff0099,0xff0066,0xff0033,0xff0000,0xff3300,0xff6600,
                 0xff9900,0xffcc00,0xffff00,0xccff00,0x99ff00,0x66ff00,0x33ff00,0x00ff00,
                 0x00ff33,0x00ff66,0x00ff99,0x00ffcc,0x00ffff,0x00ccff,0x0099ff,0x0066ff,
                 0x0033ff,0x0000ff,0x3300ff,0x6600ff,0x9900ff,0xcc00ff,0x9900ff,0x6600ff
                };
              
void move(void){
    int temp = rainbow[31];
    for (int x = (STRIP_LENGTH - 1) ; x > 0 ; x--) rainbow[x] = rainbow[x - 1];
    rainbow[0] = temp;
}    

void pulse(void){
    if(dir)
      {
      mystrip.level(level+=2);
      if(level >= 100)dir = 0;
      }
    else if(!dir)
    {
    mystrip.level(level--);
    if(level <= 5)dir = 1;    
    }
}
       
int main() {
    mystrip.level(level);
    while(1)
      {
      move();
      pulse();
      mystrip.post(rainbow);
      wait_ms(100);
      }
}
* @endcode
 */
class ws2801 
{
public:
    /** Create a new ws2801 object
    * 
    *     
    * @param CKI clock pin
    * @param SDI data  pin
    * @param STRIP_LENGTH number of ws2801 IC's i strip or array defaults to 32
    * @param reset_delay delay in us to allow latching data defaults to 800
    * @returns nothing   
    */
    ws2801(PinName CKI, PinName SDI, int STRIP_LENGTH = 32, int reset_delay = 800);
    /** write RGB color data to strip or array
    * 
    * color data for each LED is 3 bytes int the order of rrggbb.
    * array must have STRIP_LENGTH number of elements
    *
    * @param strip_colors array of color data
    */
    void post(int *strip_colors);
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
    /** get/set reset/write delay
    * 
    * mainly for experimenting with reset values without recompiling,
    * leave at default to begin with & then lower to get faster execution.
    *
    * @param delay delay in us
    * @returns delay in us
    */    
    int delay(uint32_t reset_delay=800);
  
private:  
    DigitalOut _CKI;
    DigitalOut _SDI;
    int _STRIP_LENGTH;
    int _level;
    int _reset_delay;
};

#endif