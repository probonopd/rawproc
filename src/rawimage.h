#ifndef _rawimage_h
#define _rawimage_h

#include <string>
#include <map>

#ifdef USE_DCRAW
void setdcrawpath(std::string path);
std::string getdcrawpath();
//std::string dcrawpath;
#endif

std::string librawVersion();

bool _checkRAW(const char *filename);

bool _loadRAWInfo(const char *filename, 
			unsigned *width, 
			unsigned *height, 
			unsigned *numcolors, 
			unsigned *numbits, 
			std::map<std::string,std::string> &info);

char * _loadRAW(const char *filename, 
			unsigned *width, 
			unsigned *height, 
			unsigned *numcolors, 
			unsigned *numbits, 
			std::map<std::string,std::string> &info, 
			std::string params="",
			char ** icc_m=NULL, 
			unsigned  *icclength=0);

#endif


