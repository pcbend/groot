
#include <GF1.h>
#include "globals.h"

#include <cmath>

#include <TH1.h>

ClassImp(GF1)

GF1::GF1()
  : TF1() {
  Clear();
}

GF1::GF1(const char* name, const char* formula,
         Double_t xmin, Double_t xmax)
  : TF1(name, formula, xmin, xmax, TF1::EAddToList::kNo) {
  Clear();
}

GF1::GF1(const char* name, Double_t (*fcn)(Double_t*, Double_t*),
         Double_t xmin, Double_t xmax, Int_t npar)
  : TF1(name, fcn, xmin, xmax, npar) {
  Clear();
}

GF1::GF1(const GF1& other)
  : TF1(other) {
  other.Copy(*this);
}

GF1::~GF1() {}

void GF1::Copy(TObject& obj) const {
  TF1::Copy(obj);

  auto& out  = static_cast<GF1&>(obj);
  out.fArea  = fArea;
  out.fDArea = fDArea;
  out.fSum   = fSum;
  out.fDSum  = fDSum;
  out.fChi2  = fChi2;
  out.fNdf   = fNdf;
  out.fInitialized = fInitialized;
}

void GF1::Clear(Option_t* opt) {
  TString options = opt;

  if(options.Contains("all"))
    TF1::Clear(opt);
  fInitialized = false;
  ClearResults();
}

void GF1::ClearResults() { 
  fArea  = 0.0;
  fDArea = 0.0;
  fSum   = 0.0;
  fDSum  = 0.0;
  fChi2  = 0.0;
  fNdf   = 0.0;
}

void GF1::Print(Option_t* opt) const {
  printf(GREEN);
  printf("Name:      %s\n", GetName());
  printf("Area:      %f +/- %f\n", fArea, fDArea);
  printf("Sum:       %f +/- %f\n", fSum, fDSum);
  printf("Chi^2/NDF: %f\n", fNdf != 0.0 ? fChi2 / fNdf : 0.0);

  TString options = opt;
  if(options.Contains("all"))
    TF1::Print(opt);

  printf(RESET_COLOR);
  printf("\n");
}


void GF1::CalStatistics(TH1 *hist, TF1 *background, TFitResultPtr *results) {
  if(!hist) return;

  double xlow,xhigh;
  GetRange(xlow,xhigh);
  if(xlow>xhigh) std::swap(xlow,xhigh);

  const double binwidth = hist->GetBinWidth(1);

  double totalArea = Integral(xlow,xhigh) / binwidth; 
  double bgArea    = 0.0;

  if(background) 
    bgArea = background->Integral(xlow,xhigh) / binwidth;

  SetArea(totalArea - bgArea);

  const int binLow  = hist->GetXaxis()->FindBin(xlow);
  const int binHigh = hist->GetXaxis()->FindBin(xhigh);

  const double rawSum = hist->Integral(binLow,binHigh);
  SetSum(rawSum - bgArea);

  SetSumErr(GetSum()>0 ? TMath::Sqrt(GetSum()) : 0.0);

  SetChi2(GetChisquare());
  SetNdf(GetNDF());
}




