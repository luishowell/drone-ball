#include "mbed.h"
#include "FATFileSystem.h"	//SD card
#include "SDBlockDevice.h"	//SD card
#include "bitmap_image.hpp"	//bitmap lib
#include "apa102.h"	//LED strip driver
#include "stepper.hpp"
#include "mux.hpp"

#include <string>

#define STRIP_LENGTH 144
#define DISPLAY_STEPS 200
#define STRIP_NUMBER 4

Serial pc(USBTX, USBRX); // tx, rx

SDBlockDevice sd(PC_12, PC_11, PC_10, PD_2); //SPI3
FATFileSystem fs("sd", &sd);

apa102 led_strip(PF_7, PF_9, PF_8, STRIP_LENGTH);	//sclk, mosi, miso SPI5

Stepper stepper_motor(200, PD_4, PD_5, PD_6, PD_7, PG_2, PG_3); //input 1,2,3,4,en1,en2

Mux mux(PF_3, PC_3, PC_0);

DigitalOut warning_led(LED1);
AnalogIn battery_voltage_ain(PA_3);

Ticker adc_ticker;


void voltage_check()
{
	float voltage_div_factor = 17.0/3.3;
	float min_battery_voltage = 13.5;

	float battery_voltage = battery_voltage_ain*3.3*voltage_div_factor;

	if (battery_voltage<min_battery_voltage)
	{
		warning_led = 1;
		// TURN STUFF OFF
		// return 1;
	}
}


int main()
{
	adc_ticker.attach(&voltage_check, 2.0); //check battery voltage every 2 seconds for under voltage

	led_strip.level(10);
	led_strip.setFrequency(32000000);

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
	unsigned int strip_array[STRIP_LENGTH];

    //stepper_motor.setSpeed(350);  //rpm
	stepper_motor.ramp_speed(0, 350, -1);
	mux.enable_output(true);

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

		image.fill_array_rgb(strip_array, display_counter[0], STRIP_LENGTH);
		mux.set_output(0);
		led_strip.post(strip_array);

		image.fill_array_rgb(strip_array, display_counter[1], STRIP_LENGTH);
		mux.set_output(1);
		led_strip.post(strip_array);

		image.fill_array_rgb(strip_array, display_counter[2], STRIP_LENGTH);
		mux.set_output(2);
		led_strip.post(strip_array);

		image.fill_array_rgb(strip_array, display_counter[3], STRIP_LENGTH);
		mux.set_output(3);
		led_strip.post(strip_array);

		stepper_motor.step(-1);
		display_counter[0]++;
	}

	return 1;
}