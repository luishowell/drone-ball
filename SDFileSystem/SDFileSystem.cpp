#include "SDFileSystem.h"


//constructor
SDFileSystem::SDFileSystem(sdPins_t pins)
{
    //save the pins internally
    m_pins = new sdPins_t;
    this->m_pins = &pins;

    //setup the file system
    sd = new SDBlockDevice(pins.MOSI, pins.MISO, pins.SCLK, pins.CS);
    fs = new FATFileSystem("sd", sd);

    //theres no pc
    this->pcConnected = false;

}


//constructor
SDFileSystem::SDFileSystem(sdPins_t pins, Serial* pc)
{
    //save the pins internally
    m_pins = new sdPins_t;
    this->m_pins = &pins;

    //setup the file system
    this->sd = new SDBlockDevice(pins.MOSI, pins.MISO, pins.SCLK, pins.CS);
    this->fs = new FATFileSystem("sd", sd);

    //save the pc value
    this->m_pc = pc;
    this->pcConnected = true;

}


//get the given directory
void SDFileSystem::getDirectory(const char * path)
{
    if(this->pcConnected == true){
        this->m_pc->printf("Opening specified directory.");
    }
	DIR* dir = opendir(path);
	errno_error(dir);

	struct dirent* de;
    if(this->pcConnected == true){
	    this->m_pc->printf("Printing all filenames:\n");
    }
	while((de = readdir(dir)) != NULL){
        if(this->pcConnected == true){
		    this->m_pc->printf("  %s\n", &(de->d_name)[0]);
        }
	}
    if(this->pcConnected == true){
	    this->m_pc->printf("Closeing specified directory. ");
    }
	err = closedir(dir);
	return_error(err);

}

//change the bitmap to given file
void SDFileSystem::changeImage(bitmap_image& image, string filename)
{
    image.file_name_ = filename;
    image.load_bitmap();
}

//return error function
void SDFileSystem::return_error(int ret_val)
{
  if (ret_val){
    if(this->pcConnected == true){
        this->m_pc->printf("Failure. %d\n", ret_val);
    }
  }else{
    if(this->pcConnected == true){
        this->m_pc->printf("done.\n");
    }
  }
}

//error number function
void SDFileSystem::errno_error(void* ret_val)
{
  if (ret_val == NULL) {
    if(this->pcConnected == true){
        this->m_pc->printf(" Failure. %d \n", errno);
    }
    while (true) __WFI();
  } else
    if(this->pcConnected == true){
        this->m_pc->printf(" done.\n");
    }
}