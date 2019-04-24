#include "mbed.h"
#include "FATFileSystem.h"	//SD card
#include "SDBlockDevice.h"	//SD card
#include "bitmap_image.hpp"	//bitmap lib
#include "apa102.h"	//LED strip driver
#include "stepper.hpp"
#include "mux.hpp"
#include <string>

#define STRIP_LENGTH 120
#define DISPLAY_STEPS 200
#define STRIP_NUMBER 4

Serial pc(USBTX, USBRX); // tx, rx

SDBlockDevice sd(PC_12, PC_11, PC_10, PD_2); //SPI3
FATFileSystem fs("sd", &sd);

apa102 led_strip(PF_7, PF_9, PF_8, STRIP_LENGTH);	//sclk, mosi, miso SPI5

Mux mux(PE_3, PE_6); // A, B

Ticker adc_ticker;
EventQueue queue;

DigitalOut warning_led(LED1);
AnalogIn battery_voltage_ain(PA_0);
bool run_flag = true;


void voltage_check()
{
	float voltage_div_factor = 17.0/3.3;
	float min_battery_voltage = 13.5;

	float battery_voltage = battery_voltage_ain*3.3*voltage_div_factor;

	if (battery_voltage<min_battery_voltage)
	{
		warning_led = 1;		
		//run_flag = false;
	}	
}
void battery_isr()
{
	queue.call(&voltage_check);
}


int main()
{
	led_strip.level(10);
	led_strip.setFrequency(32000000);
	// ensure all led strips are clear
	for (int i=0; i<4; i++)
	{
		mux.set_output(i);
		led_strip.clear();
	}	

	Stepper stepper_motor(200, PD_4, PD_5, PG_3, PG_2, PD_6, PD_7); //input 1,2,3,4,en1,en2

	pc.printf("Started!\n");
	
	warning_led = 0;
	Thread eventThread;
	eventThread.start(callback(&queue, &EventQueue::dispatch_forever));
	adc_ticker.attach(&battery_isr, 2.0); //check battery voltage every 2 seconds for under voltage

	pc.printf("\nOpening default image\n");
	bitmap_image image("/sd/default.bmp");
	
	if (!image)
	{		
		return 1;
	}
	else{
		pc.printf("Default image open success\n");
	}
	
	int display_counter[STRIP_NUMBER] = {0,0,0,0};
	unsigned int strip_array[STRIP_LENGTH];

	stepper_motor.ramp_speed(0, 100, -1);

	while(run_flag)
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

		for (int i=0; i<4; i++)
		{
			image.fill_array_rgb(strip_array, display_counter[i], STRIP_LENGTH);
			mux.set_output(i);
			led_strip.post(strip_array);
		}

		stepper_motor.step(-1);
		display_counter[0]++;
	}

	// turn off leds
	for (int i=0; i<4; i++)
	{
		mux.set_output(i);
		led_strip.clear();
	}	
	// turn off stepper motor
	stepper_motor.enable_motor(false);

	return 1;
}