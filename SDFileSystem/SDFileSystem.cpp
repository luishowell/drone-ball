#include "SDFileSystem.h"

#include <sstream>
namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}


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
    image.change_image(filename);
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

//function to give the filename for the give index
std::string SDFileSystem::getSelectedFilename(const char * srchFile, int index)
{
    //open the given file
    this->m_bmpFiles = fopen(srchFile, "r");

    //convert the index to a string
    std::string i = patch::to_string(index);

    //loop through the file line by line
    std::string line = "";
    while (!feof(this->m_bmpFiles)) 
    {
        int c = fgetc(this->m_bmpFiles);
        //if end of line inspect the line
        if(c == '\n')
        {
            //if the index is in the line, return this filename
            if (line.find(i) != std::string::npos) {
                std::string filename = line.substr(line.find("\t")+1);
                return filename;
            }
            line = "";
        }
        //if not the end of line at the char to the line
        else
        {
            line += c;
        }
    }

    //if the line has not been found, return the default image
    return "default.bmp";    
}