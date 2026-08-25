#include <GRootFile.h>
#include <GTxt3.h>
#include <GSpe.h>
#include <GMat.h>
#include <GTwd.h>
#include <GNsm.h>

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <TFile.h>
#include <TH1D.h>
#include <TObjString.h>
#include <TParameter.h>

namespace {

std::string Lower(std::string value) {
  std::transform(value.begin(), value.end(), value.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return value;
}

std::string Extension(std::string filename) {
  filename = Lower(filename);
  const auto dot = filename.find_last_of('.');
  if(dot == std::string::npos)
    return {};

  std::string ext = filename.substr(dot + 1);
  if(ext == "gz" || ext == "bz2" || ext == "zip") {
    const std::string remaining = filename.substr(0, dot);
    const auto innerDot = remaining.find_last_of('.');
    if(innerDot == std::string::npos)
      return ext;
    ext = remaining.substr(innerDot + 1);
  }
  return ext;
}

bool HasExtension(const std::string& filename, const char* ext) {
  return Extension(filename) == ext;
}

TObject* NotImplemented(const char* readerName, const std::string& filename) {
  std::cout << "\t" << readerName << " recognized " << filename
            << ", but this reader is not implemented yet." << std::endl;
  return nullptr;
}

} // namespace

bool GRootFile::CanOpen(const std::string& filename) const {
  return HasExtension(filename, "root");
}

TObject* GRootFile::Open(const std::string& filename, Option_t* opt) const {
  auto* file = new TFile(filename.c_str(), opt ? opt : "");
  if(!file->IsOpen()) {
    delete file;
    std::cout << "Could not open ROOT file: " << filename << std::endl;
    return nullptr;
  }
  std::cout << "\topened ROOT file: " << file->GetName() << std::endl;
  return file;
}

bool GTxt3::CanOpen(const std::string& filename) const {
  return HasExtension(filename, "txt3");
}

TObject* GTxt3::Open(const std::string& filename, Option_t*) const {
  std::ifstream input(filename);
  if(!input.is_open()) {
    std::cout << "Could not open txt3 file: " << filename << std::endl;
    return nullptr;
  }

  std::vector<double> channels;
  std::vector<double> counts;
  double c0 = 0.0;
  double c1 = 1.0;
  double c2 = 0.0;
  std::string unit = "keV";

  std::string line;
  while(std::getline(input, line)) {
    if(line.empty())
      continue;

    if(line.find("C0") != std::string::npos) {
      sscanf(line.c_str(), "C0 = %lf; C1 = %lf; C2 = %lf;", &c0, &c1, &c2);

      const size_t unitPos = line.find("unit =");
      if(unitPos != std::string::npos) {
        unit = line.substr(unitPos + 6);
        while(!unit.empty() && unit[0] == ' ')
          unit.erase(0,1);
      }
      continue;
    }

    if(line.find("RealTime") != std::string::npos ||
       line.find("LiveTime") != std::string::npos)
      continue;

    std::stringstream stream(line);
    double channel = 0.0;
    double count = 0.0;
    double energy = 0.0;
    if(stream >> channel >> count >> energy) {
      channels.push_back(channel);
      counts.push_back(count);
    }
  }

  if(counts.empty()) {
    std::cout << "No spectrum data found in txt3 file: " << filename << std::endl;
    return nullptr;
  }

  const int nBins = static_cast<int>(counts.size());
  std::vector<double> binEdges;
  binEdges.reserve(nBins + 1);
  for(int i = 0; i <= nBins; ++i) {
    const double channelEdge = channels.front() - 0.5 + i;
    binEdges.push_back(c0 + c1 * channelEdge + c2 * channelEdge * channelEdge);
  }

  auto* hist = new TH1D("TXT3 -_- Spectrum", filename.c_str(), nBins, binEdges.data());
  hist->SetDirectory(nullptr);
  hist->GetListOfFunctions()->Add(new TParameter<double>("C0", c0));
  hist->GetListOfFunctions()->Add(new TParameter<double>("C1", c1));
  hist->GetListOfFunctions()->Add(new TParameter<double>("C2", c2));
  hist->GetListOfFunctions()->Add(new TObjString(("unit=" + unit).c_str()));
  hist->GetXaxis()->SetTitle(unit.c_str());
  hist->GetYaxis()->SetTitle("Counts");

  for(int i = 0; i < nBins; ++i)
    hist->SetBinContent(i + 1, counts[i]);

  std::cout << "\topened txt3 spectrum: " << filename << std::endl;
  return hist;
}

bool GSpe::CanOpen(const std::string& filename) const {
  return HasExtension(filename, "spe");
}

TObject* GSpe::Open(const std::string& filename, Option_t*) const {
  return NotImplemented(Name(), filename);
}

bool GMat::CanOpen(const std::string& filename) const {
  return HasExtension(filename, "mat");
}

TObject* GMat::Open(const std::string& filename, Option_t*) const {
  return NotImplemented(Name(), filename);
}

bool GTwd::CanOpen(const std::string& filename) const {
  return HasExtension(filename, "twd") || HasExtension(filename, "sqr");
}

TObject* GTwd::Open(const std::string& filename, Option_t*) const {
  return NotImplemented(Name(), filename);
}

bool GNsm::CanOpen(const std::string& filename) const {
  return HasExtension(filename, "nsm");
}

TObject* GNsm::Open(const std::string& filename, Option_t*) const {
  return NotImplemented(Name(), filename);
}
