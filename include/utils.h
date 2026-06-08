#ifndef __UTILS_H__
#define __UTILS_H__

#ifndef __CINT__
#ifndef __ROOTMACRO__

#include <cstdlib> 
#include <sys/stat.h> 
#include <iostream> 
#include <fstream> 
#include <sstream>
#include <limits.h>
#include <string>
#include <vector>

inline bool fileExists(const char *filename){
  //std::ifstream(filename);
  struct stat buffer;
  return (stat(filename,&buffer)==0);
}

inline std::vector<std::string> tokenizeString(std::string path,char delimiter='/') { 
  std::istringstream ss(path);
  std::string token;
  std::vector<std::string> parts;
  //printf("fullpath = %s\n",path.c_str());
  while(std::getline(ss,token,delimiter)) {
    //printf("token = %s\n",token.c_str());
    parts.push_back(token);
  }
  return parts;
}


inline void trim(std::string& line, const std::string & trimChars = " \f\n\r\t\v") {
   //Removes the the string "trimCars" from the start or end of 'line'
  if(line.length() == 0)
    return;

  std::size_t found = line.find_first_not_of(trimChars);
  if(found != std::string::npos)
    line = line.substr(found, line.length());

  found = line.find_last_not_of(trimChars);
  if(found != std::string::npos)
    line = line.substr(0, found + 1);
}




#ifdef __LINUX__
#include <unistd.h>
inline std::string programPath(){
  char buff[PATH_MAX+1];
  size_t len = readlink("/proc/self/exe", buff, sizeof(buff)-1);
  buff[len] = '\0';

  std::string exe_path = buff;
  return exe_path.substr(0, exe_path.find_last_of('/'));
}
#endif

#ifdef __DARWIN__ 
#include <mach-o/dyld.h>
inline std::string programPath(){
  char buff[PATH_MAX];
  uint32_t len = PATH_MAX;
  _NSGetExecutablePath(buff,&len);
  std::string exe_path = buff;
  return exe_path.substr(0, exe_path.find_last_of('/'));
}
#endif

#endif
#endif

#endif
