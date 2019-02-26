#include "mbed-os/mbed.h"
#include "FATFileSystem.h"	//SD card
#include "SDBlockDevice.h"	//SD card
#include "bitmap_image.hpp"	//bitmap lib
#include "apa102.h"	//LED strip driver

#define STRIP_LENGTH 144

int dir=1, level=10;

Serial pc(USBTX, USBRX); // tx, rx

apa102 mystrip(PE_2, PE_6, STRIP_LENGTH);	//sck, mosi

SDBlockDevice sd(PC_12, PC_11, PC_10, PD_2);
FATFileSystem fs("sd", &sd);


int main()
{

	pc.printf("\nOpening input2.bmp\n");

	bitmap_image image("/sd/input2.bmp");

	if (!image)
	{
		pc.printf("Failed to open image\n");		
		return 1;
	}
	else{
		pc.printf("Image open success\n");
	}

	unsigned int strip_array[STRIP_LENGTH];

	// const unsigned int height = image.height();
	// const unsigned int width = image.width();

	rgb_t colour;
	unsigned int pixel_rgb;
	int red;
	int green;
	int blue;

	for (unsigned int y = 0; y < STRIP_LENGTH; ++y)
	{
		image.get_pixel(0, y, colour);

		red = int(colour.red);
		green = int(colour.green);
		blue = int(colour.blue);

		pixel_rgb = red;
		pixel_rgb = (pixel_rgb<<8) + green;
		pixel_rgb = (pixel_rgb<<8) + blue;

		strip_array[y] = pixel_rgb;				
	}	

	pc.printf("Sending strip array to LED strip\n");
	mystrip.level(level);
    mystrip.setFrequency(32000000);     //set spi freq

	mystrip.post(strip_array);

	pc.printf("done");

	return 1;
}

