#include <iostream>

#include <Gint.h>
#include <Gtypes.h>
#include <argParser.h>

Gint *Gint::fGint = 0;

//Gint::Gint(int argc, char **argv) : TRint("gint",&argc,argv,0,0,true,false) {
Gint::Gint(int argc, char **argv) : TRint("gint",0,0,0,0,true,false) {
  LoadOptions(argc,argv);

  SetPrompt("gint [%d] ");

}

Gint *Gint::Get(int argc,char **argv) {
  if(!fGint)
    fGint = new Gint(argc,argv);
  return fGint;
}

Gint::~Gint() { }


void Gint::LoadOptions(int argc, char **argv) {
  //check the grutrc file for set preset optrions....

  printf("in load optrions: %i\n",argc);

  argParser parser;

  std::vector<std::string> input_files;
  bool doHelp,doGui,doVersion;

  parser.default_option(&input_files)
    .description("Input file(s)");
  parser.option("h help ?",&doHelp)
    .description("Show this help Message")
    .default_value(false);
  parser.option("g gui",&doGui)
    .description("Start the GUI")
    .default_value(false);
  parser.option("v version",&doVersion)
    .description("Show version")
    .default_value(false);
 
  // Do the parsing...
  try{
    parser.parse(argc, argv);
  } catch (ParseError& e){
    std::cerr << "ERROR: " << e.what() << "\n"
              << parser << std::endl;
    //fShouldExit = true;
  }

 

  // Print help if requested.
  if(doHelp){
    //Version();
    std::cout << parser << std::endl;
    //fShouldExit = true;
  }
  if(doVersion) {
    //Version();
    printf("version not available.\n");
    //fShouldExit = true;
  }
  if(doGui) {
    printf("starting gui...\n"); 
  }


 for(auto& file : input_files){
    FileAutoDetect(file);
  }
}

kFileType Gint::DetermineFileType(const std::string& filename) const {
  size_t dot = filename.find_last_of('.');
  std::string ext = filename.substr(dot+1);

  if((ext=="gz") || (ext=="bz2") || (ext=="zip")) {
    std::string remaining = filename.substr(0,dot);
    ext = remaining.substr(remaining.find_last_of('.')+1);
  }
  
  if(ext == "cal") {
    return kFileType::CALIBRATION;
  } else if(ext == "root") {
    return kFileType::ROOTFILE;
  } else if((ext=="c") || (ext=="C") 
            || (ext=="c+") || (ext=="C+") 
            || (ext=="c++") || (ext=="C++")) {
    return kFileType::MACRO;
  } else if(ext == "cuts") {
    return kFileType::CUTS;
  } else {
    return kFileType::UNKNOWN;
  }
};

bool Gint::FileAutoDetect(const std::string& filename) {
  switch(DetermineFileType(filename)){
    case kFileType::CALIBRATION:
      break;
    case kFileType::ROOTFILE:
      break;
    case kFileType::MACRO:
      break;
    case kFileType::CUTS:
    default:
      printf("\tDiscarding unknown file: %s\n",filename.c_str());
      return false;    
    break;
  };
  return true;
}

