#include <iostream>

#include <TFile.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TEnv.h>

#include <fstream>
#include <memory>
#include <sstream>
#include <vector>
#include <TH1D.h>
#include <TMemFile.h>
#include <TParameter.h>
#include <TObjString.h>

#include <GFile.h>
#include <Gint.h>
#include <Gtypes.h>
#include <GHTTPConnection.h>
#include <GOptions.h>
#include <Histomatic.h>

#include <globals.h>
#include <GGlobals.h>

Gint *Gint::fGint = 0;

namespace {

std::vector<std::unique_ptr<GHTTPConnection> > live_connections;

void StartLiveConnection(const std::string& url) {
  std::unique_ptr<GHTTPConnection> connection(new GHTTPConnection);
  connection->SetSnapshotCallback(
    [](const std::string& source, const std::string& path, TObject* object) {
      if(gHistomatic && gHistomatic->GetListTree()) {
        gHistomatic->GetListTree()->AddManagedPath(source, path, object);
      }
    });
  if(connection->Start(url)) {
    live_connections.push_back(std::move(connection));
  }
}

void StopLiveConnections() {
  for(auto& connection : live_connections) {
    if(connection) {
      connection->Stop();
    }
  }
  live_connections.clear();
}

}

//Gint::Gint(int argc, char **argv) : TRint("gint",&argc,argv,0,0,true,false) {
Gint::Gint(int argc, char **argv) : TRint("gint",0,0,0,0,true,false), 
  fCalC0(0), fCalC1(1), fCalC2(0), fCalUnit("keV"), fHasCalibration(false),
  fRootFilesOpened(0), fTabLock(false), fMainThreadId(std::this_thread::get_id())  {

  LoadOptions(argc,argv);
  //LoadStyle();
  gROOT->ForceStyle(); 
  SetPrompt("groot [%d] ");

}

Gint *Gint::Get(int argc,char **argv) {
  if(!fGint)
    fGint = new Gint(argc,argv);
  return fGint;
}

Gint::~Gint() {
  StopLiveConnections();
}

void Gint::Terminate(int status) {
  StopLiveConnections();
  printf("\nbye,bye\n\n");
  SetPrompt("");
  TRint::Terminate(status);

}

/*
void Gint::LoadStyle() {
  // Load the ROOT style file
  //gStyle->SetPalette(kVisibleSpectrum);
  gStyle->SetPalette(gEnv->GetValue("Gint.Style.Palette",kVisibleSpectrum));
  gStyle->SetNumberContours(gEnv->GetValue("Gint.Style.Contours",32));
  
  gStyle->SetHistLineWidth(2);
  gStyle->SetHistFillStyle(0);
 
  gStyle->SetFrameBorderMode(1);
  //gStyle->SetFrameFillColor(1);
  gStyle->SetCanvasBorderMode(1);
  gStyle->SetPadBorderMode(1);
  gStyle->SetPadBorderSize(5);
  //gStyle->SetPadColor(2);
  //canvas.SetBorderSize(6);
  //canvas.SetHighLightColor(kBlue); //or whatever color
  //Hists.Stats: "nemri"
  gStyle->SetOptStat(gEnv->GetValue("Hists.Stats","nemri"));
  //gEnv->Print();

  std::string theme = gEnv->GetValue("Gint.Style.Theme","light");
  if(theme.compare("dark")==0) {
    
    gEnv->SetValue("Gui.BackgroundColor", "#282828");
    gEnv->SetValue("Gui.ForegroundColor", "#E8E8E8");
    gEnv->SetValue("Gui.HighLightColor", "#3A3A3A");
    gEnv->SetValue("Gui.SelectBackgroundColor", "#505050");
    gEnv->SetValue("Gui.SelectForegroundColor", "white");
    gEnv->SetValue("Gui.DocumentBackgroundColor", "#202124");
    gEnv->SetValue("Gui.DocumentForegroundColor", "#E8E8E8");
    gEnv->SetValue("Gui.TooltipBackgroundColor", "#3A3A3A");
    gEnv->SetValue("Gui.TooltipForegroundColor", "white");


    gStyle->SetCanvasColor(923);  // kGrey +3 
    gStyle->SetPadColor(gEnv->GetValue("Gint.Style.PadColor",0));
    gStyle->SetFrameFillColor(gEnv->GetValue("Gint.Style.FrameColor",0));

    gStyle->SetTextColor(gEnv->GetValue("Gint.Style.TextColor",1));
    gStyle->SetLabelColor(gEnv->GetValue("Gint.Style.LabelColor",1),"XYZ");
    gStyle->SetTitleColor(gEnv->GetValue("Gint.Style.TitleColor",1),"XYZ");
    gStyle->SetAxisColor(gEnv->GetValue("Gint.Style.AxisColor",1),"XYZ");
  }
  

  gROOT->ForceStyle();
}
*/

void Gint::LoadOptions(int argc, char **argv) {
  //check the grutrc file for set preset optrions....
  GOptions options(argc, argv);
  if(!options.ParseSucceeded()) {
    std::cerr << options.Error() << std::endl;
    //fShouldExit = true;
  }

  // Print help if requested.
  if(options.ShowHelp()){
    //Version();
    std::cout << options.HelpText() << std::endl;
    //fShouldExit = true;
  }
  if(options.ShowVersion()) {
    //Version();
    printf("version not available.\n");
    //fShouldExit = true;
  }
  if(options.StartGui()) {
    printf("starting gui...\n"); 
    Histomatic::Get(); 
    //gHistomatic = new Histomatic;
  }

  
  for(const auto& input : options.Inputs()){
    const std::string& file = input.value;
    switch(input.type){
      case GOptions::Input::Type::LiveUrl:
        StartLiveConnection(file);
        break;
      case GOptions::Input::Type::Calibration:
	LoadCalibrationFile(file);
        break;
      case GOptions::Input::Type::File:
        {
          TObject* object = GFile::Open(file);
          auto* rfile = dynamic_cast<TFile*>(object);
          if(rfile) {
            const char* command = Form("TFile* _file%i = (TFile*)%luL",
                                       fRootFilesOpened,
                                       (unsigned long)rfile);
            TRint::ProcessLine(command);
            std::cout << "\tfile " << BLUE << rfile->GetName() << RESET_COLOR
                      <<  " opened as " << BLUE <<  "_file" << fRootFilesOpened
                      << RESET_COLOR <<  std::endl;
            fRootFilesOpened++;
          }
          if(rfile && options.StartGui() && gHistomatic)
            gHistomatic->AddRootFile(rfile);
          else if(object && options.StartGui() && gHistomatic) {
     		std::string name = file.substr(file.find_last_of("/\\") + 1);
     		name += ".root";

     		TMemFile *memfile = new TMemFile(name.c_str(), "RECREATE");
     		memfile->cd();
     		object->Write();

      		gHistomatic->AddRootFile(memfile);
          }
        }
        break;
      case GOptions::Input::Type::Macro:
        break;
      case GOptions::Input::Type::Cuts:
        break;
      case GOptions::Input::Type::Unknown:
      default:
        printf("\tDiscarding unknown file: %s\n",file.c_str());
      break;
    };
  }



}

kFileType Gint::DetermineFileType(const std::string& filename) const {
  return GOptions::DetermineFileType(filename);
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

bool Gint::LoadCalibrationFile(const std::string& filename) {
  std::ifstream input(filename);
  if(!input.is_open()) {
    std::cout << "Could not open calibration file: " << filename << std::endl;
    return false;
  }

  std::string line;
  while(std::getline(input, line)) {
    if(line.empty()) {
      continue;
    }

    if(line.find("C0") != std::string::npos) {
      sscanf(line.c_str(), "C0 = %lf", &fCalC0);
    } else if(line.find("C1") != std::string::npos) {
      sscanf(line.c_str(), "C1 = %lf", &fCalC1);
    } else if(line.find("C2") != std::string::npos) {
      sscanf(line.c_str(), "C2 = %lf", &fCalC2);
    } else if(line.find("unit") != std::string::npos) {
      size_t unitPos = line.find("=");
      if(unitPos != std::string::npos) {
        fCalUnit = line.substr(unitPos + 1);
        while(!fCalUnit.empty() && fCalUnit[0] == ' ') {
          fCalUnit.erase(0, 1);
        }
      }
    }
  }

  fHasCalibration = true;

  std::cout << "\tloaded calibration: "
            << "C0=" << fCalC0
            << ", C1=" << fCalC1
            << ", C2=" << fCalC2
            << ", unit=" << fCalUnit
            << std::endl;

  return true;
}





TH1D* Gint::OpenTxt3File(const std::string& filename) {
  std::ifstream input(filename);
  if(!input.is_open()) {
    std::cout << "Could not open txt3 file: " << filename << std::endl;
    return nullptr;
  }

  std::vector<double> channels;
  std::vector<double> counts;
  std::vector<double> energies;

  double c0 = 0;
  double c1 = 1;
  double c2 = 0;
  std::string unit = "keV";

  std::string line;
  while(std::getline(input, line)) {
    if(line.empty()) {
      continue;
    }

    if(line.find("C0") != std::string::npos) {
      sscanf(line.c_str(), "C0 = %lf; C1 = %lf; C2 = %lf;", &c0, &c1, &c2);
      
	size_t unitPos = line.find("unit =");
	if(unitPos != std::string::npos) {
	  unit = line.substr(unitPos + 6);
	  while(!unit.empty() && unit[0] == ' ') {
	  unit.erase(0,1);
	}
	}
	continue;
    }

    if(line.find("RealTime") != std::string::npos ||
       line.find("LiveTime") != std::string::npos) {
      continue;
    }

    std::stringstream ss(line);
    double channel = 0;
    double count = 0;
    double energy = 0;

    if(ss >> channel >> count >> energy) {
      channels.push_back(channel);
      counts.push_back(count);
      energies.push_back(energy);
    }
  }

  if(counts.empty()) {
    std::cout << "No spectrum data found in txt3 file: " << filename << std::endl;
    return nullptr;
  }

  //int nBins = counts.size();
  //double xMin = energies.front() - 0.5;
  //double xMax = energies.back() + 0.5;

  //TH1D* hist = new TH1D("txt3_spectrum", filename.c_str(), nBins, xMin, xMax);

  if(fHasCalibration) {
	c0 = fCalC0;
	c1 = fCalC1;
	c2 = fCalC2;
	unit = fCalUnit;
  }

  int nBins = counts.size();

  std::vector<double> binEdges;
  binEdges.reserve(nBins + 1);

  for(int i = 0; i <= nBins; ++i) { 
    double channelEdge = channels.front() - 0.5 + i;
    double energyEdge = c0 + c1 * channelEdge + c2 * channelEdge * channelEdge;
    binEdges.push_back(energyEdge);
  }

  TH1D* hist = new TH1D("TXT3 -_- Spectrum", filename.c_str(), nBins, binEdges.data());
  
  hist->GetListOfFunctions()->Add(new TParameter<double>("C0", c0));
  hist->GetListOfFunctions()->Add(new TParameter<double>("C1", c1));
  hist->GetListOfFunctions()->Add(new TParameter<double>("C2", c2));
  hist->GetListOfFunctions()->Add(new TObjString(("unit=" + unit).c_str()));

  hist->GetXaxis()->SetTitle(unit.c_str());
  hist->GetYaxis()->SetTitle("Counts");

  for(int i = 0; i < nBins; ++i) {
    hist->SetBinContent(i + 1, counts[i]);
  }

  std::cout << "\topened txt3 spectrum: " << filename << std::endl;
  return hist;
}







TFile *Gint::OpenRootFile(const std::string& filename, Option_t* opt) {
  TString sopt(opt);
  sopt.ToLower();

  TFile *file = NULL;
  if(sopt.Contains("recreate") || sopt.Contains("new")) {
    file = new TFile(filename.c_str(),"recreate");
    if(!file->IsOpen()) { delete file; file = NULL; }
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
    if(!file->IsOpen()) { delete file; file = NULL; }
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
