#include "mbed-os/mbed.h"
#include "FATFileSystem.h"	//SD card
#include "SDBlockDevice.h"	//SD card
#include "bitmap_image.hpp"	//bitmap lib
#include "apa102.h"	//LED strip driver
#include "stepper.hpp"

#include <string>

#define STRIP_LENGTH 144
#define DISPLAY_STEPS 200
#define STRIP_NUMBER 4

Serial pc(USBTX, USBRX); // tx, rx

SDBlockDevice sd(PC_12, PC_11, PC_10, PD_2); //SPI3
FATFileSystem fs("sd", &sd);

apa102 strip_1(PA_5, PA_7, PA_6, STRIP_LENGTH);	//sclk, mosi, miso SPI1
apa102 strip_2(PB_13, PB_15, PC_2, STRIP_LENGTH);	//sclk, mosi, miso SPI2
apa102 strip_3(PE_2, PE_6, PE_5, STRIP_LENGTH);	//sclk, mosi, miso SPI4
apa102 strip_4(PF_7, PF_9, PF_8, STRIP_LENGTH);	//sclk, mosi, miso SPI5


void convert_pixels(bitmap_image& image, unsigned int *input_array, int col)
{
	rgb_t colour;
	unsigned int pixel_rgb;
	int red;
	int green;
	int blue;

	for (int y = 0; y < STRIP_LENGTH; y++)
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

void change_image(bitmap_image& image, string filename)
{
    image.file_name_ = filename;
    image.load_bitmap();
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


int main()
{
	int level=10;
	unsigned int freq = 32000000;
	setup_strips(level, freq);

    Stepper stepper_motor(200, PD_4, PD_5, PD_6, PD_7);
    stepper_motor.setSpeed(350);  //rpm

	pc.printf("\nOpening default image\n");
	bitmap_image image("/sd/default.bmp");
	
	if (!image)
	{
		//pc.printf("Failed to open image\n");		
		return 1;
	}
	else{
		pc.printf("Default image open success\n");
	}
	
	int display_counter[STRIP_NUMBER] = {0,0,0,0};
	unsigned int strip_1_array[STRIP_LENGTH];
	unsigned int strip_2_array[STRIP_LENGTH];
	unsigned int strip_3_array[STRIP_LENGTH];
	unsigned int strip_4_array[STRIP_LENGTH];

	while(1)
	{
		if (display_counter[0]==DISPLAY_STEPS)
		{
			display_counter[0]=0;
		}
		for (int i=1; i<STRIP_NUMBER; i++)
		{
			display_counter[i] = display_counter[0] + ((DISPLAY_STEPS/STRIP_NUMBER)*i);
			if (display_counter[i]>=DISPLAY_STEPS)
			{
				display_counter[i] = display_counter[i] - DISPLAY_STEPS;
			}
		}

		convert_pixels(image, strip_1_array, display_counter[0]);
		strip_1.post(strip_1_array);
		convert_pixels(image, strip_2_array, display_counter[1]);
		strip_2.post(strip_2_array);
		convert_pixels(image, strip_3_array, display_counter[2]);
		strip_3.post(strip_3_array);
		convert_pixels(image, strip_4_array, display_counter[3]);
		strip_4.post(strip_4_array);

		stepper_motor.step(-1);
		display_counter[0]++;
	}

	return 1;
}