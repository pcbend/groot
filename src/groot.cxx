
#include<cstdio>

#include<Gint.h>
#include<utils.h>

#include<TEnv.h>

void loadEnv() {
  // Set the GSYS variable based on the executable path.
  // If GSYS has already been defined, don't overwrite.
  setenv("GSYS", (programPath()+"/..").c_str(), 0);

  printf("GSYS has been set to: %s\n",getenv("GSYS"));

  // Load $GSYS/.gintrc
  std::string grut_path = Form("%s/.gintrc",getenv("GSYS"));
  gEnv->ReadFile(grut_path.c_str(),kEnvChange);

  // Load $HOME/.gintrc
  grut_path = Form("%s/.gintrc",getenv("HOME"));
  if(fileExists(grut_path.c_str())){
    gEnv->ReadFile(grut_path.c_str(),kEnvChange);
  }
  
  // Load $PWD/.gintrc
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
