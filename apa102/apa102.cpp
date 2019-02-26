#include "apa102.h"
#include "mbed.h"

#define DO_N(N, F) for( int repetier = 0; repetier < N; repetier++) { F; }

//------------------------------------------------------------------------------------------------------------
apa102::apa102(PinName CKI, PinName SDI, int STRIP_LENGTH, int reset_delay)
: _CKI(CKI), _SDI(SDI), _STRIP_LENGTH(STRIP_LENGTH), _reset_delay(reset_delay) {
    
    _level=100;
    
    spi_class = new BurstSPI(SDI, PE_5,CKI);
    spi_class->format(16, 0);
    spi_class->frequency(1000000);  //default to 1 MHz
    
    this->clear();
    wait_us(_reset_delay);
}
//------------------------------------------------------------------------------------------------------------
void apa102::setFrequency(uint32_t frequency) {
    spi_class->frequency(frequency);    
}
//------------------------------------------------------------------------------------------------------------
void apa102::post(unsigned int *strip_colors) {
    this->preamble();

    unsigned int scaled_level = 0xE0 | (_level * 31 / 100);
    scaled_level = (scaled_level << 8) & 0xFF00;
    
    unsigned int MSB, LSB;
    
    for(int LED_number = 0 ; LED_number < _STRIP_LENGTH ; LED_number++) {
        //this->write_word(scaled_level, 8);
        //this->write_word(strip_colors[LED_number], 24);
        MSB = scaled_level | (strip_colors[LED_number] & 0xFF);
        //MSB = 0xF0FF;
        LSB = (strip_colors[LED_number] & 0xFF00) | ((strip_colors[LED_number] >> 16) & 0xFF);
        //LSB = 0xFFFF;
        spi_class->fastWrite(MSB);//0xF0FF);
        spi_class->fastWrite(LSB);//0xFFFF);
    }
    
    this->afterword();
    wait_us(_reset_delay); //Wait for 1ms to go into reset
}
//------------------------------------------------------------------------------------------------------------
void apa102::clear(void) {
    this->preamble();
    //DO_N(_STRIP_LENGTH, this->write_word(0xE0000000, 32));
    for(int i = 0; i < _STRIP_LENGTH; i++)
    {
        spi_class->fastWrite(0xE000);
        spi_class->fastWrite(0x0000);
    }
    this->afterword();
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
int apa102::delay(uint32_t reset_delay)
{
if(reset_delay <= 0xffffffff)_reset_delay = reset_delay;
return _reset_delay;
}
//------------------------------------------------------------------------------------------------------------
inline void apa102::write_hi() {
    this->write_bit(1);
}
inline void apa102::write_lo() {
    this->write_bit(0);
}
inline void apa102::write_bit(bool bit) {
    _CKI = 0;
    _SDI = bit;
    _CKI = 1;
}
inline void apa102::write_word(uint32_t word, int bits) {
    for(char color_bit = bits - 1 ; color_bit != 255 ; color_bit--) {
        this->write_bit(
            (word & (1 << color_bit)) ? 1 : 0
        );
    }
}
inline void apa102::write_done() {
    _CKI = 0;
}
inline void apa102::preamble() {
    //DO_N(32, this->write_lo());
    spi_class->fastWrite(0x0000);
    spi_class->fastWrite(0x0000);
}
inline void apa102::afterword() {
    //DO_N(32, this->write_hi());
    //this->write_done();
    spi_class->fastWrite(0xFFFF);
    spi_class->fastWrite(0xFFFF);
}
//---------EOF---------------EOF------------------------------------------------------------------------------