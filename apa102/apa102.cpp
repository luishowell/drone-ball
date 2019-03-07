#include "apa102.h"
#include "mbed.h"


//------------------------------------------------------------------------------------------------------------
apa102::apa102(PinName SCLK_pn, PinName MOSI_pn, PinName MISO_pn, int STRIP_LENGTH, int reset_delay)
: _STRIP_LENGTH(STRIP_LENGTH), _reset_delay(reset_delay), spi_class(MOSI_pn, MISO_pn, SCLK_pn)
{    
    _level=100;  

    spi_class.format(16, 0);
    spi_class.frequency(1000000);  //default to 1 MHz
    
    this->clear();
    wait_us(_reset_delay);
}
//------------------------------------------------------------------------------------------------------------
void apa102::setFrequency(uint32_t frequency) 
{
    spi_class.frequency(frequency);    
}
//------------------------------------------------------------------------------------------------------------
void apa102::post(unsigned int *strip_colors) 
{
    this->send_start_frame();

    unsigned int scaled_level = 0xE0 | (_level * 31 / 100);
    scaled_level = (scaled_level << 8) & 0xFF00;
    
    unsigned int MSB, LSB;
    
    for(int LED_number = 0 ; LED_number < _STRIP_LENGTH ; LED_number++) 
    {
        MSB = scaled_level | (strip_colors[LED_number] & 0xFF);
        LSB = (strip_colors[LED_number] & 0xFF00) | ((strip_colors[LED_number] >> 16) & 0xFF);

        spi_class.fastWrite(MSB);//0xF0FF);
        spi_class.fastWrite(LSB);//0xFFFF);
    }
    
    this->send_end_frame();
    wait_us(_reset_delay); 
}
//------------------------------------------------------------------------------------------------------------
void apa102::clear(void) 
{
    this->send_start_frame();
   
    for(int i = 0; i < _STRIP_LENGTH; i++)
    {
        spi_class.fastWrite(0xE000);
        spi_class.fastWrite(0x0000);
    }
    this->send_end_frame();
}
//------------------------------------------------------------------------------------------------------------
int apa102::level(int level)
{
    if((level <= 100) && level)
    {
    _level = level;
    return _level;
    }
    return 0;
}
//------------------------------------------------------------------------------------------------------------
inline void apa102::send_start_frame() 
{
    spi_class.fastWrite(0x0000);
    spi_class.fastWrite(0x0000);
}

inline void apa102::send_end_frame() 
{
    spi_class.fastWrite(0x0000);
    spi_class.fastWrite(0x0000);

    int end_frames = _STRIP_LENGTH/16.0;

    for (int i=0;i<end_frames;i++)
    {
     spi_class.fastWrite(0x0000);   
    }
}
//---------EOF---------------EOF------------------------------------------------------------------------------