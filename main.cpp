#include "mbed-os/mbed.h"
#include <stdio.h>
#include <errno.h> 
#include <string>

//#include "FATFileSystem.h"	//SD card
//#include "SDBlockDevice.h"	//SD card
#include "SDFileSystem.h"
#include "hc05.h"
#include "bitmap_image.hpp"	//bitmap lib
#include "apa102.h"	//LED strip driver

#define STRIP_LENGTH 144

//appCode defines
#define startDisplayCode 0x10
#define stopDisplayCode 0x21
#define requestFilesCode 0x32
#define selectImageCode 0x43
#define sendImageCode 0x54
#define setRotationCode 0x65
#define sendTestCode 0xFF

int dir=1, level=10;

RawSerial pc(USBTX, USBRX); // tx, rx

DigitalOut led1(LED1);
DigitalOut led2(LED2);
//DigitalOut led3(LED3);
DigitalOut led5(LED4);

//apa102 mystrip(PE_2, PE_6, PE_5, STRIP_LENGTH);	//sclk, mosi, miso

//SDBlockDevice sd(p5, p6, p7, p8);
//FATFileSystem fs("sd", &sd);

//setup bluetooth module
hc05 bt(p13, p14, &pc);

//sd card declaration
SDFileSystem *sd;

//create an event queue
EventQueue queue(32 * EVENTS_EVENT_SIZE);

void interperetCommand();

char appCode;



//This is used for the adc monitioring
Ticker adc_ticker;
EventQueue queueVoltage;

//check voltage pins
DigitalOut warning_led(LED3);
AnalogIn battery_voltage_ain(p20);

//boolean to indicate another transfer is ongoing
bool btSendOngoing = false;

void voltage_check()
{
	float voltage_div_factor = 17.0/3.3;
	float min_battery_voltage = 13.5;

	float battery_voltage = battery_voltage_ain.read()*3.3*voltage_div_factor;

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
    led1 = !led1;
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
        case selectImageCode : pc.printf("Select image from file list\n\r");
                    break; 
        case sendImageCode :{ pc.printf("Sending new image\n\r");
                    bt.m_bt->attach(0); //detach the interrupt
                    string filename = bt.receiveFilename(".bmp");
                    pc.printf("Filename is: %s\n", filename.c_str());
                    bt.m_bt->attach(do_something); //reattach the interrupt
                    break; }
        case setRotationCode :{ pc.printf("Set rotation speed\n\r");
                    bt.m_bt->attach(0); // detatch the interrupt
                    char rpm = bt.readCharacter(); //read the next character 
                    pc.printf("Rotation speed is: 0x%x\n", rpm); //print the character 
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

    //create thread that'll run the event queue's dispatch function
    Thread eventThread;
    eventThread.start(callback(&queue, &EventQueue::dispatch_forever));

    //wrap calls in queue event to automatically defer to the queue's thread
    bt.m_bt->attach(do_something);



    //attach the adc interrupt
    warning_led = 0;
	Thread adcThread;
	adcThread.start(callback(&queueVoltage, &EventQueue::dispatch_forever));
	adc_ticker.attach(&battery_isr, 2.0); //check battery voltage every 2 seconds for under voltage

	pc.printf("\n\n\n\n\n\n\n");

    //setup the sd card
    sdPins_t sdPins;
    sdPins.MOSI = p5;
    sdPins.MISO = p6;
    sdPins.SCLK = p7;
    sdPins.CS = p8;


    sd = new SDFileSystem(sdPins, &pc);
    sd->getDirectory("/sd/LoadedImages");


    //check that the file is returned properly
    FILE *testFile;
    testFile = sd->getBmpFileList("/sd/LoadedImages");//, testFile);

    //send the file over bluetooth
    //bt.sendFile(testFile);


    //play around with bluetooth
    
    //pc.printf("Read the bluetooth module\n");
    //for(unsigned int i = 0; i < 15; i++){

        //pc.printf("Enter character: ");
        //char testchar = bt.readCharacter();

       //pc.printf("Testchar is: %x\n", testchar);
    //}

	//bitmap_image image("/sd/input2.bmp");

	// pc.printf("Welcome to the filesystem example.\n");

	// pc.printf("Opening a new file, numbers.txt.");
	// FILE* fd = fopen("/sd/numbers.txt", "w+");
	// errno_error(fd);

	// for (int i = 0; i < 20; i++){
	// 	pc.printf("Writing decimal numbers to a file (%d/20)\r", i);
	// 	fprintf(fd, "%d\n", i);
	// }
	// pc.printf("Writing decimal numbers to a file (20/20) done.\n");

	// pc.printf("Closing file.");
	// fclose(fd);
	// pc.printf(" done.\n");

	// pc.printf("Re-opening file read-only.");
	// fd = fopen("/sd/numbers.txt", "r");
	// errno_error(fd);

	// pc.printf("Dumping file to screen.\n");
	// char buff[16] = {0};
	// while (!feof(fd)){
	// 	int size = fread(&buff[0], 1, 15, fd);
	// 	fwrite(&buff[0], 1, size, stdout);
	// }
	// pc.printf("EOF.\n");

	// pc.printf("Closing file.");
	// fclose(fd);
	// pc.printf(" done.\n");

	// pc.printf("Opening root directory.");
	// DIR* dir = opendir("/sd/");
	// errno_error(fd);

	// struct dirent* de;
	// pc.printf("Printing all filenames:\n");
	// while((de = readdir(dir)) != NULL){
	// 	pc.printf("  %s\n", &(de->d_name)[0]);
	// }

	// pc.printf("Closeing root directory. ");
	// error = closedir(dir);
	// return_error(error);
	// pc.printf("Filesystem Demo complete.\n");



	//if (!image)
	//{
	//	pc.printf("Failed to open image\n");		
	//	return 1;
	//}
	//else{
	//	pc.printf("Image open success\n");
	//}

	//unsigned int strip_array[STRIP_LENGTH];

	// const unsigned int height = image.height();
	// const unsigned int width = image.width();

	//rgb_t colour;
	//unsigned int pixel_rgb;
	//int red;
	//int green;
	//int blue;

	//for (unsigned int y = 0; y < STRIP_LENGTH; ++y)
	//{
		//image.get_pixel(0, y, colour);

		//red = int(colour.red);
		//green = int(colour.green);
		//blue = int(colour.blue);

		//pixel_rgb = red;
		//pixel_rgb = (pixel_rgb<<8) + green;
		//pixel_rgb = (pixel_rgb<<8) + blue;

		//strip_array[y] = pixel_rgb;				
	//}	

	//pc.printf("Sending strip array to LED strip\n");
	//mystrip.level(level);
    //mystrip.setFrequency(32000000);     //set spi freq

	//mystrip.post(strip_array);
    while(1){
        wait(1);
    }

	pc.printf("done");

	return 1;
}

