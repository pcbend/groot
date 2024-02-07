
#include<cstdio>

#include<Gint.h>
#include<myClass.h>
#include<utils.h>

#include<TEnv.h>
//#include<TStyle.h>
//#include<TROOT.h>

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

  //loadStyle();
  loadEnv();
  Gint::Get(argc,argv)->Run(true);
 
  return 0;
}
