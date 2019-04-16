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
SDFileSystem::SDFileSystem(sdPins_t pins, RawSerial* pc)
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

//clean directory
void SDFileSystem::cleanDirectory(const char * path)
{
    //open the directory
	DIR* dir = opendir(path);
	errno_error(dir);

    if(this->pcConnected == true){
        this->m_pc->printf("Cleaning directory:  %s\n", path);
    }

    struct dirent* de;
    std::string sdir;

    //loop through the directory
	while((de = readdir(dir)) != NULL){
        sdir = &(de->d_name)[0];

        //if ._ is contained, remove it
        if(sdir.find("._") != std::string::npos){ 
            sdir = std::string(path) + "/" + &(de->d_name)[0];
            if(this->pcConnected == true){
                this->m_pc->printf("Deleting file:  %s\n", &(de->d_name)[0]);
            }
            remove(sdir.c_str());
        }
	}
    err = closedir(dir);
	return_error(err);
}


//get the given directory
void SDFileSystem::getDirectory(const char * path)
{
    if(this->pcConnected == true){
        this->m_pc->printf("Opening specified directory.");
    }

    //clean the directory 
    this->cleanDirectory(path);

    //open the directory
	DIR* dir = opendir(path);
	errno_error(dir);

    //create the filestructure files
    FILE* fd = fopen("/sd/files.txt", "w");
	errno_error(fd);
    FILE* fdbmp = fopen("/sd/bmpFiles.txt", "w");
	errno_error(fdbmp);

	struct dirent* de;
    if(this->pcConnected == true){
	    this->m_pc->printf("Printing all filenames:\n");
    }

    //interim variables
    unsigned int i = 0;
    std::string sdir;

    //loop through the directory
	while((de = readdir(dir)) != NULL){
        if(this->pcConnected == true){
		    this->m_pc->printf("  %s\n", &(de->d_name)[0]);
        }
        fprintf(fd, "%d\t%s\n", i, &(de->d_name)[0]);
        sdir = &(de->d_name)[0];
        if(sdir.find(".bmp") != std::string::npos){   
            fprintf(fdbmp, "%d\t%s\n", i, &(de->d_name)[0]);
            this->m_pc->printf("BMP:  %s\n", &(de->d_name)[0]);
        }
        i++;
	}

    //close the files
    fclose(fd);
    fclose(fdbmp);

    if(this->pcConnected == true){
	    this->m_pc->printf("Closing specified directory. ");
    }
	err = closedir(dir);
	return_error(err);

}

//change the bitmap to given file
void SDFileSystem::changeImage(bitmap_image& image, std::string filename)
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

//get a list of the bmp files in a given directory
FILE *SDFileSystem::getBmpFileList(const char * path)//, FILE *bmpFile)
{
    //create the list files for the directory
    this->getDirectory(path);

    //return the file list
    this->m_bmpFiles = fopen("/sd/bmpFiles.txt", "r");
    FILE *bmpFile = this->m_bmpFiles;
    return bmpFile;
}