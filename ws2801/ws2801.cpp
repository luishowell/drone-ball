#include "ws2801.h"
#include "mbed.h"


//------------------------------------------------------------------------------------------------------------
ws2801::ws2801(PinName CKI, PinName SDI, int STRIP_LENGTH, int reset_delay) : _CKI(CKI), _SDI(SDI), _STRIP_LENGTH(STRIP_LENGTH), _reset_delay(reset_delay)
{
_level=100;
//ws2801.clear();
for(int bits = 0; bits <= _STRIP_LENGTH*24; bits++)
  {
  _CKI = 0;
  _SDI = 0;
  _CKI = 1;
  }
_CKI = 0;
wait_us(_reset_delay);
}
//------------------------------------------------------------------------------------------------------------
void ws2801::post(int *strip_colors)
{

int strip_colors_leveled[_STRIP_LENGTH];
uint32_t mask;

for(int LED_number = 0 ; LED_number < _STRIP_LENGTH ; LED_number++)
  {
  if(_level != 100)
    {
    int R = ((strip_colors[LED_number] & 0x00ff0000) >> 16)*_level/100;
    int G = ((strip_colors[LED_number] & 0x0000ff00) >> 8)*_level/100;
    int B = (strip_colors[LED_number] & 0x000000ff)*_level/100;
    strip_colors_leveled[LED_number] = (R << 16)|(G << 8)|B;
    }
  else strip_colors_leveled[LED_number] = strip_colors[LED_number];
  int this_led_color = strip_colors_leveled[LED_number]; //24 bits of color data
  for(char color_bit = 23 ; color_bit != 255 ; color_bit--)
    { 
    _CKI = 0;
    mask = 1 << color_bit;
    if(this_led_color & mask)_SDI = 1;
    else _SDI = 0;
    _CKI = 1; //Data is latched when clock goes high
    }
  }
_CKI = 0;
wait_us(_reset_delay); //Wait for 1ms to go into reset
}
//------------------------------------------------------------------------------------------------------------
void ws2801::clear(void)
{
for(int bits = 0; bits <= _STRIP_LENGTH*24; bits++)
  {
  _CKI = 0;
  _SDI = 0;
  _CKI = 1;
  }
_CKI = 0;
wait_us(_reset_delay);
}
//------------------------------------------------------------------------------------------------------------
int ws2801::level(int level)
{
if((level <= 100) && level)
  {
  _level = level;
  return _level;
  }
return 0;
}

//------------------------------------------------------------------------------------------------------------
int ws2801::delay(uint32_t reset_delay)
{
if(reset_delay <= 0xffffffff)_reset_delay = reset_delay;
return _reset_delay;
}
//---------EOF---------------EOF------------------------------------------------------------------------------