#ifndef __UTILS_H__
#define __UTILS_H__

#include <cstdlib> 
#include <sys/stat.h> 
#include <iostream> 
#include <fstream> 
#include <sstream>

bool fileExists(const char *filename){
  //std::ifstream(filename);
  struct stat buffer;
  return (stat(filename,&buffer)==0);
}


#endif
