#include "mbed-os/mbed.h"

//#include "FATFileSystem.h"	//SD card
//#include "SDBlockDevice.h"	//SD card
#include "SDFileSystem.h"
#include "hc05.h"
#include "bitmap_image.hpp"	//bitmap lib
#include "apa102.h"	//LED strip driver
#include "stepper.hpp"
#include "mux.hpp"
#include <string>
#include <stdio.h>
#include <errno.h> 

#define STRIP_LENGTH 120
#define DISPLAY_STEPS 200
#define STRIP_NUMBER 4
#define MAX_RPM 350

//appCode defines
#define startDisplayCode 0x10
#define stopDisplayCode 0x21
#define requestFilesCode 0x32
#define selectImageCode 0x43
#define sendImageCode 0x54
#define setRotationCode 0x65
#define sendTestCode 0xFF

RawSerial pc(USBTX, USBRX); // tx, rx

apa102 led_strip(PF_7, PF_9, PF_8, STRIP_LENGTH);	//sclk, mosi, miso SPI5

Mux mux(PE_3, PE_6); // A, B

//setup bluetooth module
hc05 bt(PE_8, PE_7, &pc); //tx, rx: UART3, serial pc
DigitalOut bt_led(LED2);

DigitalOut warning_led(LED1);
AnalogIn battery_voltage_ain(PA_0);

//This is used for the adc monitioring
Ticker adc_ticker;
//create an event queue
EventQueue queue(32 * EVENTS_EVENT_SIZE);
EventQueue queueVoltage;

//sd card declaration
// SDBlockDevice sd(PC_12, PC_11, PC_10, PD_2); //SPI3
// FATFileSystem fs("sd", &sd);
SDFileSystem *sd;

//boolean to indicate another transfer is ongoing
bool btSendOngoing = false;
bool run_flag = true;
char appCode;

void interperetCommand();


void voltage_check()
{
	float voltage_div_factor = 1.055/0.18;
	float min_battery_voltage = 13.5;

	float battery_voltage = battery_voltage_ain.read()*3.3*voltage_div_factor;
	//pc.printf("Battery Voltage: %.2f\n", battery_voltage);

    //send the voltage over bluetooth to the app unless the file list is being transferred
    if(btSendOngoing == false){
        btSendOngoing = true;
        bt.sendCharacter('#'); //start character 
        bt.sendFloat(battery_voltage); //battery voltage
        bt.sendCharacter('~'); //end char 
        btSendOngoing = false;
    }

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

void do_something(){
    bt_led = !bt_led;
    appCode = bt.readCharacter();
    queue.call(&interperetCommand);
}

void interperetCommand() {
    char savedAppCode = appCode;
    pc.printf("text received 0x%x\n\r", savedAppCode);
    switch(savedAppCode){
        case startDisplayCode : pc.printf("Start the display\n\r");
                    break; 
        case stopDisplayCode : pc.printf("Stop the display\n\r");
					run_flag = false;
                    break; 
        case requestFilesCode : pc.printf("Request file list\n\r");
                    FILE *fileList;
                    fileList = sd->getBmpFileList("/sd/LoadedImages");
                    //while(btSendOngoing == true){
                    //    pc.printf("Waiting on ticker\n");
                    //}
                    btSendOngoing = true;
                    bt.sendCharacter('!'); //start char
                    bt.sendFile(fileList); //the actual file 
                    bt.sendCharacter('~'); //end char
                    btSendOngoing = false;

                    break; 
        case selectImageCode :{ 
					pc.printf("Select image from file list\n\r");
					bt.m_bt->attach(0); //detach the interrupt
					char selected_file = bt.readCharacter();
					bt.m_bt->attach(do_something); //reattach the interrupt
                    break;} 
        case sendImageCode :{ pc.printf("Sending new image\n\r");
                    bt.m_bt->attach(0); //detach the interrupt
                    string filename = bt.receiveFilename(".bmp");
                    pc.printf("Filename is: %s\n", filename.c_str());
                    bt.m_bt->attach(do_something); //reattach the interrupt
                    break; }
        case setRotationCode :{ pc.printf("Set rotation speed\n\r");
                    bt.m_bt->attach(0); // detatch the interrupt
                    char char_rpm = bt.readCharacter(); //read the next character 
					int int_rpm = (int(char_rpm)*MAX_RPM)/100;
                    pc.printf("Rotation speed is: %i\n", int_rpm); //print the character 
                    bt.m_bt->attach(do_something); //reattach the interrupt
                    break; }
        case sendTestCode : pc.printf("Send test code\r\n");
                    break;
        default : pc.printf("Unknown code: 0x%x\n\r", savedAppCode);
                    break; 
    }
}


int main()
{
	pc.printf("\n\nStarted!\n");

	led_strip.level(10);
	led_strip.setFrequency(32000000);
	// ensure all led strips are clear
	for (int i=0; i<4; i++)
	{
		mux.set_output(i);
		led_strip.clear();
	}	

	Stepper stepper_motor(200, PD_4, PD_5, PG_3, PG_2, PD_6, PD_7); //input 1,2,3,4,en1,en2

    //create thread that'll run the event queue's dispatch function
    Thread eventThread;
    eventThread.start(callback(&queue, &EventQueue::dispatch_forever));

    //attach the adc interrupt
    warning_led = 0;
	Thread adcThread;
	adcThread.start(callback(&queueVoltage, &EventQueue::dispatch_forever));
	adc_ticker.attach(&battery_isr, 2.0); //check battery voltage every 2 seconds for under voltage

    //wrap calls in queue event to automatically defer to the queue's thread
    bt.m_bt->attach(do_something);	
	bt_led = 0;

    //setup the sd card
    sdPins_t sdPins;
    sdPins.MOSI = PC_12;
    sdPins.MISO = PC_11;
    sdPins.SCLK = PC_10;
    sdPins.CS = PD_2;

    sd = new SDFileSystem(sdPins, &pc);
    sd->getDirectory("/sd/LoadedImages");

	bitmap_image image;


	pc.printf("\nOpening default image\n");
	sd->changeImage(image, "/sd/default.bmp");
	
	if (!image)
	{		
		return 1;
	}
	else{
		pc.printf("Default image open success\n");
	}
	
	int display_counter[STRIP_NUMBER] = {0,0,0,0};
	unsigned int strip_array[STRIP_LENGTH];

	int dir = 1;
	stepper_motor.ramp_speed(0, 200, dir);

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

		stepper_motor.step(dir);
		display_counter[0]++;
	}

	pc.printf("\nOFF!\n");

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