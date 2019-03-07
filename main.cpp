#include "mbed-os/mbed.h"
#include "FATFileSystem.h"	//SD card
#include "SDBlockDevice.h"	//SD card
#include "bitmap_image.hpp"	//bitmap lib
#include "apa102.h"	//LED strip driver#

#define STRIP_LENGTH 144
#define DISPLAY_STEPS 200
#define STRIP_NUMBER 4

int dir=1, level=10;
unsigned int freq = 32000000;

Serial pc(USBTX, USBRX); // tx, rx

Timer t;

Thread thread1;
Thread thread2;
Thread thread3;
Thread thread4;

apa102 strip_1(PA_5, PA_7, PA_6, STRIP_LENGTH);	//sclk, mosi, miso SPI1
apa102 strip_2(PB_13, PB_15, PC_2, STRIP_LENGTH);	//sclk, mosi, miso SPI2
apa102 strip_3(PE_2, PE_6, PE_5, STRIP_LENGTH);	//sclk, mosi, miso SPI4
apa102 strip_4(PF_7, PF_9, PF_8, STRIP_LENGTH);	//sclk, mosi, miso SPI5

unsigned int strip_1_array[STRIP_LENGTH];
unsigned int strip_2_array[STRIP_LENGTH];
unsigned int strip_3_array[STRIP_LENGTH];
unsigned int strip_4_array[STRIP_LENGTH];

SDBlockDevice sd(PC_12, PC_11, PC_10, PD_2); //SPI3
FATFileSystem fs("sd", &sd);

bitmap_image image("/sd/input2.bmp");


void convert_pixels(unsigned int *input_array, int col)
{
	rgb_t colour;
	unsigned int pixel_rgb;
	int red;
	int green;
	int blue;

	for (unsigned int y = 0; y < STRIP_LENGTH; ++y)
	{
		image.get_pixel(col, y, colour);

		red = int(colour.red);
		green = int(colour.green);
		blue = int(colour.blue);

		pixel_rgb = red;
		pixel_rgb = (pixel_rgb<<8) + green;
		pixel_rgb = (pixel_rgb<<8) + blue;

		input_array[y] = pixel_rgb;				
	}	
}

void setup_strips(int level, unsigned int freq)
{
	strip_1.level(level);
    strip_1.setFrequency(freq);     //set spi freq
	strip_2.level(level);
    strip_2.setFrequency(freq);     //set spi freq
	strip_3.level(level);
    strip_3.setFrequency(freq);     //set spi freq
	strip_4.level(level);
    strip_4.setFrequency(freq);     //set spi freq
}


void strip_1_thread()
{
	convert_pixels(strip_1_array, 0);
	strip_1.post(strip_1_array);	
}
void strip_2_thread()
{
	convert_pixels(strip_2_array, 0);
	strip_2.post(strip_2_array);	
}
void strip_3_thread()
{
	convert_pixels(strip_3_array, 0);
	strip_3.post(strip_3_array);	
}
void strip_4_thread()
{
	convert_pixels(strip_4_array, 0);
	strip_4.post(strip_4_array);	
}


int main()
{
	pc.printf("\nOpening input2.bmp\n");	
	
	if (!image)
	{
		pc.printf("Failed to open image\n");		
		return 1;
	}
	else{
		pc.printf("Image open success\n");
	}

	setup_strips(level, freq);
	

	t.start();
	convert_pixels(strip_1_array, 0);
	strip_1.post(strip_1_array);
	convert_pixels(strip_2_array, 0);
	strip_2.post(strip_2_array);
	convert_pixels(strip_3_array, 0);
	strip_3.post(strip_3_array);
	convert_pixels(strip_4_array, 0);
	strip_4.post(strip_4_array);
    t.stop();
    pc.printf("The time taken for serial was %f seconds\n", t.read());
	t.reset();

	t.start();
	thread1.start(strip_1_thread);
	thread2.start(strip_2_thread);
	thread3.start(strip_3_thread);
	thread4.start(strip_4_thread);
	thread1.join();
	thread2.join();
	thread3.join();
	thread4.join();
    t.stop();
    pc.printf("The time taken for threads was %f seconds\n", t.read());
	t.reset();


	return 1;
}

