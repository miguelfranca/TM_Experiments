
#include "Tools.hpp"

#include <sys/stat.h> //mkdir
#include <sstream> //ostringstream
#include <fstream> //ifstream
#include <iomanip> //std::setfill, std::setw

void errorMsg(const std::string& msg){
    print(msg);
    throw std::runtime_error(msg.c_str());
    exit(1);
}

int createFolder(std::string path){
	int status = mkdir(path.c_str(), ACCESSPERMS);
	if(status!=0)
		switch(errno){
			case EEXIST:
				print("\nFOLDER " + path + " ALREADY EXISTS");
				return 0;
				break; //folder already exists
			default:
				errorMsg("FOLDER " + path + " COULD NOT BE CREATED");
				break;
		}
	print("\nFOLDER " + path + " CREATED WITH SUCCESS");
	return 1; //success
}

bool fileExists(const std::string& path){
    std::ifstream infile(path);
    return infile.good();
}

unsigned fileSize(const std::string& file){
    std::ifstream infile(file, std::ifstream::ate | std::ifstream::binary);
    return infile.tellg(); 
}

unsigned fileSize(std::ifstream& file){
	std::streampos save = file.tellg();
	file.seekg(0,std::ios::end);
	unsigned out = file.tellg();
	file.seekg(save);
	return out;
}

unsigned fileSize_Beg(std::ifstream& file){
	file.seekg(0,std::ios::end);
	unsigned out = file.tellg();
	file.seekg(0,std::ios::beg);
	return out;
}

std::string checkFileName(const std::string& name, const std::string& extension){
	std::ostringstream str;
	if(name!=""){
		str << name;
		unsigned size = extension.size();
		if(name.substr( name.size()-size, size ) != extension)
			str << extension;
	}
	else{
		/*time_t date_t = time(NULL);
		struct tm * date = gmtime(&date_t);
		str << "NEAT_Image_";
		str << std::setw(2) << std::setfill('0') << date->tm_mday;
		str << std::setw(2) << std::setfill('0') << date->tm_mon+1;
		str << std::setw(2) << std::setfill('0') << date->tm_year-100;
		str << "_" << std::setw(2) << std::setfill('0') << date->tm_hour;
		str << std::setw(2) << std::setfill('0') << date->tm_min;
		str << std::setw(2) << std::setfill('0') << date->tm_sec;
		str << "_" << (int)clock()%1000; 
		str << extension; */
	}
	return str.str();
}