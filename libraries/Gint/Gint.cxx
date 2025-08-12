#include <iostream>

#include <TFile.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TEnv.h>

#include <Gint.h>
#include <Gtypes.h>
#include <argParser.h>
#include <Histomatic.h>

#include <globals.h>
#include <GGlobals.h>

Gint *Gint::fGint = 0;

//Gint::Gint(int argc, char **argv) : TRint("gint",&argc,argv,0,0,true,false) {
Gint::Gint(int argc, char **argv) : TRint("gint",0,0,0,0,true,false), 
  fRootFilesOpened(0), fTabLock(false), fMainThreadId(std::this_thread::get_id())  {

  LoadOptions(argc,argv);
  LoadStyle();
  SetPrompt("groot [%d] ");

}

Gint *Gint::Get(int argc,char **argv) {
  if(!fGint)
    fGint = new Gint(argc,argv);
  return fGint;
}

Gint::~Gint() { }

void Gint::Terminate(int status) {
  printf("\nbye,bye\n\n");
  SetPrompt("");
  TRint::Terminate(status);

}

void Gint::LoadStyle() {
  // Load the ROOT style file
  //gStyle->SetPalette(kVisibleSpectrum);
  gStyle->SetPalette(gEnv->GetValue("Gint.Style",kVisibleSpectrum));
  gStyle->SetHistLineWidth(2);
  gStyle->SetHistFillStyle(0);
 
  gStyle->SetFrameBorderMode(1);
  //gStyle->SetFrameFillColor(1);
  gStyle->SetPadBorderMode(1);
  gStyle->SetPadBorderSize(5);
  //gStyle->SetPadColor(2);

//canvas.SetBorderSize(6);
//canvas.SetHighLightColor(kBlue); //or whatever color


  //Hists.Stats: "nemri"
  gStyle->SetOptStat(gEnv->GetValue("Hists.Stats","nemri"));

  //gEnv->Print();


  gROOT->ForceStyle();
}


void Gint::LoadOptions(int argc, char **argv) {
  //check the grutrc file for set preset optrions....
  

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
    Histomatic::Get(); 
    //gHistomatic = new Histomatic;
  }

  
  for(auto& file : input_files){
    switch(DetermineFileType(file)){
      case kFileType::CALIBRATION:
        break;
      case kFileType::ROOTFILE:
        {
          TFile *rfile = OpenRootFile(file);
          if(rfile && doGui && gHistomatic) 
            gHistomatic->AddRootFile(rfile);
        }
        break;
      case kFileType::MACRO:
        break;
      case kFileType::CUTS:
        break;
      default:
        printf("\tDiscarding unknown file: %s\n",file.c_str());
      break;
    };
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

/*
bool Gint::FileAutoDetect(const std::string& filename) {
  switch(DetermineFileType(filename)){
    case kFileType::CALIBRATION:
      break;
    case kFileType::ROOTFILE:
      OpenRootFile(filename);
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
*/

TFile *Gint::OpenRootFile(const std::string& filename, Option_t* opt) {
  TString sopt(opt);
  sopt.ToLower();

  TFile *file = NULL;
  if(sopt.Contains("recreate") || sopt.Contains("new")) {
    file = new TFile(filename.c_str(),"recreate");
    if(!file->IsOpen()) file = NULL;
    if(file) {
      const char* command = Form("TFile* _file%i = (TFile*)%luL",
                                 fRootFilesOpened,
                                 (unsigned long)file);
      TRint::ProcessLine(command);
      fRootFilesOpened++;
    } else {
      std::cout << "Could not create " << filename << std::endl;
    }
  } else {
    //file = TFile::Open(filename.c_str(),opt);
    file = new TFile(filename.c_str(),opt);
    if(!file->IsOpen()) file = NULL;
    if(file) {
      const char* command = Form("TFile* _file%i = (TFile*)%luL",
                                 fRootFilesOpened,
                                 (unsigned long)file);
      TRint::ProcessLine(command);
      std::cout << "\tfile " << BLUE << file->GetName() << RESET_COLOR
                <<  " opened as " << BLUE <<  "_file" << fRootFilesOpened << RESET_COLOR <<  std::endl;

      fRootFilesOpened++;
    } else {
      std::cout << "Could not create " << filename << std::endl;
    }
  }

  return file;
}

int Gint::TabCompletionHook(char* buf, int* pLoc, std::ostream& out) {
  fTabLock = true;
  int result = TRint::TabCompletionHook(buf,pLoc,out);
  fTabLock = false;
  return result;
}


long Gint::ProcessLine(const char* line, bool sync, int* error) {
  long retval = 0;
  if(fTabLock) {
    return TRint::ProcessLine(line,sync,error);
  }
  TString sline(line);
  if(!sline.Length()) {
    return 0;
  }
  sline.ReplaceAll("TCanvas","GCanvas");

  if(std::this_thread::get_id() != fMainThreadId){
    printf("Not the main thread...  ");
    fflush(stdout);
  }           
  
  if(!sline.CompareTo("clear")) {
    retval = TRint::ProcessLine(".! clear");
  } else {
    retval = TRint::ProcessLine(sline.Data(),sync,error);
  }

  if(retval < 0) {
    //std::cerr << "Error processing line: " << line << std::endl;
  }
  return retval;
}



