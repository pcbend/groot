
#include<cstdio>

#include<Gint.h>
#include<myClass.h>
#include<utils.h>

#include<TEnv.h>

#include <limits.h>
#ifdef __linux__
#include <unistd.h>
std::string programPath(){
  char buff[PATH_MAX+1];
  size_t len = readlink("/proc/self/exe", buff, sizeof(buff)-1);
  buff[len] = '\0';

  std::string exe_path = buff;
  return exe_path.substr(0, exe_path.find_last_of('/'));
}
#endif
#ifdef __darwin__ 
#include <mach-o/dyld.h>
std::string programPath(){
  char buff[PATH_MAX];
  uint32_t len = PATH_MAX;
  _NSGetExecutablePath(buff,&len);
  std::string exe_path = buff;
  return exe_path.substr(0, exe_path.find_last_of('/'));
}

#endif

void loadEnv() {
  // Set the GRUTSYS variable based on the executable path.
  // If GRUTSYS has already been defined, don't overwrite.
  setenv("GSYS", (programPath()+"/..").c_str(), 0);

  // Load $GRUTSYS/.grutrc
  std::string grut_path = Form("%s/.gintrc",getenv("GRUTSYS"));
  gEnv->ReadFile(grut_path.c_str(),kEnvChange);

  // Load $HOME/.grutrc
  grut_path = Form("%s/.gintrc",getenv("HOME"));
  if(fileExists(grut_path.c_str())){
    gEnv->ReadFile(grut_path.c_str(),kEnvChange);
  }
  
  grut_path = Form("%s/.gintrc",getenv("PWD"));
  if(fileExists(grut_path.c_str())){
    gEnv->ReadFile(grut_path.c_str(),kEnvChange);
  }
}
 
int main(int argc, char **argv) {
  
  //printf("hello!\n");
  myClass c;
  c.print();

  loadEnv();
 
  Gint::Get(argc,argv)->Run(true);

 
  return 0;
}
