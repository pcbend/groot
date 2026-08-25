#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

#include <TAxis.h>
#include <TH1.h>
#include <TObject.h>
#include <TParameter.h>

#include <GFile.h>
#include <GFileReader.h>
#include <Gtypes.h>

namespace {

void require(bool condition, const char* message) {
  if(!condition) {
    std::cerr << "FAILED: " << message << '\n';
    std::exit(1);
  }
}

void requireNear(double actual, double expected, double tolerance, const char* message) {
  if(std::abs(actual - expected) > tolerance) {
    std::cerr << "FAILED: " << message << " expected " << expected
              << " got " << actual << '\n';
    std::exit(1);
  }
}

void requireReader(const std::string& filename, kFileType type, const char* name) {
  const GFileReader* reader = GFile::FindReader(filename);
  require(reader != nullptr, "reader should exist");
  require(reader->Type() == type, "reader type should match");
  require(std::string(reader->Name()) == name, "reader name should match");
}

} // namespace

int main() {
  requireReader("spectrum.root", kFileType::ROOTFILE, "GRootFile");
  requireReader("spectrum.txt3", kFileType::TXT3, "GTxt3");
  requireReader("spectrum.SPE", kFileType::SPE, "GSpe");
  requireReader("matrix.mat.gz", kFileType::MAT, "GMat");
  requireReader("matrix.twd", kFileType::TWD, "GTwd");
  requireReader("matrix.sqr.bz2", kFileType::TWD, "GTwd");
  requireReader("offline.nsm", kFileType::NSM, "GNsm");
  require(GFile::FindReader("unknown.xyz") == nullptr, "unknown extension should not route");

  const std::string txt3Path = std::string(GROOT_TEST_DATA_DIR) + "/tiny.txt3";
  std::unique_ptr<TObject> object(GFile::Open(txt3Path));
  require(object != nullptr, "txt3 fixture should open");
  auto* hist = dynamic_cast<TH1*>(object.get());
  require(hist != nullptr, "txt3 fixture should produce a histogram");
  require(hist->GetNbinsX() == 4, "txt3 bin count");
  requireNear(hist->GetBinContent(1), 10.0, 1e-12, "txt3 bin 1 content");
  requireNear(hist->GetBinContent(4), 40.0, 1e-12, "txt3 bin 4 content");
  requireNear(hist->GetXaxis()->GetBinLowEdge(1), -0.5, 1e-12, "txt3 first edge");
  requireNear(hist->GetXaxis()->GetBinUpEdge(4), 7.5, 1e-12, "txt3 last edge");
  require(std::string(hist->GetXaxis()->GetTitle()) == "keV", "txt3 x unit");

  auto* c0 = dynamic_cast<TParameter<double>*>(hist->GetListOfFunctions()->FindObject("C0"));
  auto* c1 = dynamic_cast<TParameter<double>*>(hist->GetListOfFunctions()->FindObject("C1"));
  require(c0 && c1, "txt3 calibration metadata");
  requireNear(c0->GetVal(), 0.5, 1e-12, "txt3 C0");
  requireNear(c1->GetVal(), 2.0, 1e-12, "txt3 C1");
  return 0;
}
