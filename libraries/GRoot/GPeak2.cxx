
#include <GPeak2.h>
#include <TGraph.h>
#include <TVirtualFitter.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>
#include <TH1.h>


#include "globals.h"
#include "GFunctions.h"
#include "GCanvas.h"

ClassImp(GPeak2)

GPeak2::GPeak2()
      : TF1("temp",GFunctions::PhotoPeakBG,0,16000,7) {
  //Clear("");
  //SetName("temp");
  InitNames();
  //SetParent(0);
}

GPeak2::GPeak2(double xlow,double xhigh,Option_t *opt)
      : TF1("temp",GFunctions::PhotoPeakBG,xlow,xhigh,7) { ///,
  Clear("");
  if(xlow>xhigh)
    std::swap(xlow,xhigh);

  TF1::SetRange(xlow,xhigh);

  //fBGFit.SetNpx(1000);
  //fBGFit.SetLineStyle(2);
  //fBGFit.SetLineColor(kBlack);

  SetName(Form("PFIT_%d_to_%d",(int)(xlow),(int)(xhigh)));
  InitNames();


  //SetParent(0);
}


/*
GPeak2::GPeak2(Double_t cent,Double_t xlow,Double_t xhigh,TF1 *bg,Option_t *opt)
      : TF1("photopeakbg",GFunctions::PhotoPeakBG,xlow,xhigh,7) {
  Clear("");
  if(cent>xhigh || cent<xlow) {
    //out of range...
    if(xlow>cent)
      std::swap(xlow,cent);
    if(xlow>xhigh)
      std::swap(xlow,xhigh);
    if(cent>xhigh)
      std::swap(cent,xhigh);
  }
  TF1::SetRange(xlow,xhigh);
  SetName(Form("Chan%d_%d_to_%d",(Int_t)(cent),(Int_t)(xlow),(Int_t)(xhigh)));
  InitNames();
  TF1::SetParameter("centroid",cent);

  //if(bg) {
  //  fBGFit.Clear();
  //  fBGFit.Copy(*bg);
  //} else {
  //  fBGFit = TF1("BGFit",GFunctions::StepBG,xlow,xhigh,10);
  //}

  //fBGFit.SetNpx(1000);
  //fBGFit.SetLineStyle(2);
  //fBGFit.SetLineColor(kBlack);
  
  SetParent(0);
  //DetachBackground();
  //SetDirectory(0);
  //fBGFit.SetDirectory(0);
}
*/
/*
GPeak2::GPeak2()
      : TF1("photopeakbg",GFunctions::PhotoPeakBG,0,1000,10){
        //fBGFit("background",GFunctions::StepBG,0,1000,10) {

  Clear();
  InitNames();
  //fBGFit.SetNpx(1000);
  //fBGFit.SetLineStyle(2);
  //fBGFit.SetLineColor(kBlack);
  
  SetParent(0);
  //SetDirectory(0);
  //DetachBackground();
  //fBGFit.SetDirectory(0);
}
*/
/*
GPeak2::GPeak2(const GPeak2 &peak)
  : TF1(peak) {
  
  SetParent(0);
  //SetDirectory(0);
  //DetachBackground();
  //fBGFit.SetDirectory(0);
  peak.Copy(*this);
}
*/

GPeak2::~GPeak2() {
  //gROOT->RecursiveRemove(&fBGFit);
  //gROOT->RecursiveRemove(this);
  //if(background)
  //  delete background;
}

//void GPeak2::Fcn(Int_t &npar,Double_t *gin,Double_T &f,Double_t *par,Int_t iflag) {
  //chisquared calculator
  //
//  int i=0;
//  double chisq = 0;
//  double delta = 0;
//  for(i=0;i<nbins;i++) {
//    delta = (data[i] - GFunctions::PhotoPeakBG((x+i),par))/error[i];
//    chisq += delta*delta;
//  }
//  f=chisq;
//}

void GPeak2::InitNames(){
  TF1::SetParName(0,"Height");
  TF1::SetParName(1,"centroid");
  TF1::SetParName(2,"sigma");
  TF1::SetParName(3,"R");
  TF1::SetParName(4,"beta");
  TF1::SetParName(5,"step");
  TF1::SetParName(6,"bg_offset");
  //TF1::SetParName(7,"bg_slope");
}

//void GPeak2::Copy(TObject &obj) const {
  //printf("0x%08x\n",&obj);
  //fflush(stdout);
  //printf("%s\n",obj.GetName());
  //fflush(stdout);

//  TF1::Copy(obj);
//  ((GPeak2&)obj).init_flag = init_flag;
//  ((GPeak2&)obj).fArea     = fArea;
//  ((GPeak2&)obj).fDArea    = fDArea;
//  ((GPeak2&)obj).fSum      = fSum;
//  ((GPeak2&)obj).fDSum     = fDSum;
//  ((GPeak2&)obj).fChi2     = fChi2;
//  ((GPeak2&)obj).fNdf      = fNdf;

  //fBGFit.Copy((((GPeak2&)obj).fBGFit));
  //((GPeak2&)obj).DetachBackground();
//}

bool GPeak2::InitParams(TH1 *fithist){
  if(!fithist){
    printf("No histogram is associated yet, no initial guesses made\n");
    return false;
  }
  //printf("%s called.\n",__PRETTY_FUNCTION__); fflush(stdout);
  //Makes initial guesses at parameters for the fit. Uses the histogram to
  double   xlow,xhigh;
  GetRange(xlow,xhigh);

  int binlow  = fithist->GetXaxis()->FindBin(xlow);
  int binhigh = fithist->GetXaxis()->FindBin(xhigh);

  double yhigh = fithist->GetBinContent(binlow);
  double ylow  = fithist->GetBinContent(binhigh);

  for(int x=1;x<5;x++) {
    yhigh += fithist->GetBinContent(binlow-x);
    ylow  += fithist->GetBinContent(binhigh+x);
  }
  yhigh = yhigh / 5.0;
  ylow  = ylow  / 5.0;

  if(ylow>yhigh)
    std::swap(ylow,yhigh);

  double largestx=0.0;
  double largesty=0.0;
  int i = binlow;
  for(;i<=binhigh;i++) {
    if(fithist->GetBinContent(i) > largesty) {
      largesty = fithist->GetBinContent(i);
      largestx = fithist->GetXaxis()->GetBinCenter(i);
    }
  }

  // - par[0]: height of peak
  // - par[1]: cent of peak
  // - par[2]: sigma
  // - par[3]: R:    relative height of skewed gaus to gaus
  // - par[4]: beta: "skewedness" of the skewed gaussin
  // - par[5]: step: size of stepfunction step.
  // - par[6]: base bg height.

  //limits.
  TF1::SetParLimits(0,0,largesty*2);                    // - par[0]: height of peak
  TF1::SetParLimits(1,xlow,xhigh);                      // - par[1]: cent of peak
  TF1::SetParLimits(2,0.1,xhigh-xlow);                  // - par[2]: sigma
  TF1::SetParLimits(3,0.0,40);                          // - par[3]: R:    relative height of skewed gaus to gaus
  TF1::SetParLimits(4,0.01,5);                          // - par[4]: beta: "skewedness" of the skewed gaussin           
  double step = (yhigh-ylow)/largesty*50;               
  TF1::SetParLimits(5,0.0,step+step);                   // - par[5]: step: size of stepfunction step.
  double offset = ylow;
  TF1::SetParLimits(6,offset-0.5*offset,offset+offset); // - par[6]: base bg height.

  //Make initial guesses
  TF1::SetParameter(0,largesty);             // - par[0]: height of peak
  TF1::SetParameter(1,largestx);             // - par[1]: cent of peak
  TF1::SetParameter(2,(largestx*.01)/2.35);  // - par[2]: sigma
  TF1::SetParameter(3,5.);                   // - par[3]: R:    relative height of skewed gaus to gaus
  TF1::SetParameter(4,1.);                   // - par[4]: beta: "skewedness" of the skewed gaussin
  TF1::SetParameter(5,step);                 // - par[5]: step: size of stepfunction step.
  TF1::SetParameter(6,offset);               // - par[6]: base bg height.
  //TF1::SetParameter(7,slope);

  TF1::SetParError(0,0.10 * largesty);
  TF1::SetParError(1,0.25);
  TF1::SetParError(2,0.10 *((largestx*.01)/2.35));
  TF1::SetParError(3,5);
  TF1::SetParError(4,0.5);
  TF1::SetParError(5,0.10 * step);
  TF1::SetParError(6,0.10 * offset);

  //TF1::Print();
  TVirtualFitter::SetMaxIterations(100000);

  //SetInitialized();
  return true;
}

/*
TFitResultPtr GPeak2::Fit(TH1 *fithist,Option_t *opt) {
  if(!fithist)
    return false;
  TString options = opt;
  if(!IsInitialized())
    InitParams(fithist);
  TVirtualFitter::SetMaxIterations(100000);

  bool verbose = !options.Contains("Q");
  bool noprint =  options.Contains("no-print");
  if(noprint) {
    options.ReplaceAll("no-print","");
  }

  if(fithist->GetSumw2()->fN!=fithist->GetNbinsX()+2)
    fithist->Sumw2();

  TFitResultPtr fitres = fithist->Fit(this,Form("%sLRSME",options.Data()));

  printf("chi^2/NDF = %.02f\n",this->GetChisquare()/(double)this->GetNDF());

  if(!fitres.Get()->IsValid()) {
    printf(RED  "fit has failed, trying refit... " RESET_COLOR);
    fithist->GetListOfFunctions()->Last()->Delete();
    fitres = fithist->Fit(this,Form("%sLRSME",options.Data())); //,Form("%sRSM",options.Data()))
    if( fitres.Get()->IsValid() ) {
      printf(DGREEN " refit passed!" RESET_COLOR "\n");
    } else {
      printf(DRED " refit also failed :( " RESET_COLOR "\n");
    }
  }

  Double_t xlow,xhigh;
  TF1::GetRange(xlow,xhigh);

  double bgpars[5];
  bgpars[0] = TF1::GetParameters()[0];
  bgpars[1] = TF1::GetParameters()[1];
  bgpars[2] = TF1::GetParameters()[2];
  bgpars[3] = TF1::GetParameters()[5];
  bgpars[4] = TF1::GetParameters()[6];

  //fithist->GetListOfFunctions()->Print();
  TF1 fBGFit("background",GFunctions::StepBG,xlow,xhigh,6); 
  fBGFit.SetName(Form("BG_%d_to_%d",(int)(xlow),(int)(xhigh)));
  fBGFit.SetNpx(1000);
  fBGFit.SetLineStyle(2);
  fBGFit.SetLineColor(kBlack);
  //fBGFit.SetParameters(bgpars);
  fBGFit.SetParameters(bgpars);

  fArea = this->Integral(xlow,xhigh) / fithist->GetBinWidth(1);
  double bgArea = fBGFit.Integral(xlow,xhigh) / fithist->GetBinWidth(1);
  fArea -= bgArea;

  if(xlow>xhigh)
    std::swap(xlow,xhigh);
  fSum = fithist->Integral(fithist->GetXaxis()->FindBin(xlow),
                           fithist->GetXaxis()->FindBin(xhigh));// * fithist->GetBinWidth(1);
  printf("sum between markers: %02f\n",fSum);
  fDSum = TMath::Sqrt(fSum);
  fSum -= bgArea;
  printf("sum after subtraction: %02f\n",fSum);


  if(!verbose) {
    printf("hist: %s\n",fithist->GetName());
    //Print();
    //printf("BG Area:         %.02f\n",bgArea);
    ////printf("GetChisquared(): %.4f\n", TF1::GetChisquare());
    //printf("GetNDF():        %i\n",   TF1::GetNDF());
    //printf("GetProb():       %.4f\n", TF1::GetProb());
    //TF1::Print();
  }

  //printf("fithist->GetListOfFunctions()->FindObject(this) = 0x%08x\n",fithist->GetListOfFunctions()->FindObject(GetName()));
  //fflush(stdout);
  Copy(*fithist->GetListOfFunctions()->FindObject(GetName()));
  fithist->GetListOfFunctions()->Add(fBGFit.Clone()); //use to be a clone.
  //fithist->GetListOfFunctions()->Add(&fBGFit); //use to be a clone.

  SetParent(0); //fithist);
  //DetachBackground();


  //delete tmppeak;
  return fitres;
}
*/

/*
Bool_t GPeak2::FitExclude(TH1 *fithist,double xlow,double xhigh,Option_t *opt) {
  //ok, we are going to assume we have a funny shaped peak here,
  //freeze all parameters except offset and step and see if we can get the 
  //bg right so we can at least try to use the sum...  - this is bad, i am going to create and 
  //return a new one.....
  TF1 ff("photopeakbg_exclude",GFunctions::PhotoPeakBGExcludeRegion,xlow,xhigh,9);
  ff.SetParName(0,"Height");
  ff.SetParName(1,"centroid");
  ff.SetParName(2,"sigma");
  ff.SetParName(3,"R");
  ff.SetParName(4,"beta");
  ff.SetParName(5,"step");
  ff.SetParName(6,"bg_offset");
  ff.SetParName(7,"exclude_low");
  ff.SetParName(8,"exclude_high");
  
  ff.FixParameter(0,TF1::GetParameter(0));         //fithist->GetBinContent(bin));
  ff.FixParameter(1,TF1::GetParameter(1)); 
  ff.FixParameter(2,TF1::GetParameter(2)); 
  ff.FixParameter(3,TF1::GetParameter(3)); 
  ff.FixParameter(4,TF1::GetParameter(4)); 
  ff.SetParameter(5,TF1::GetParameter(5)); 
  ff.SetParameter(6,TF1::GetParameter(6)); 
  ff.FixParameter(7,xlow+10); 
  ff.FixParameter(8,xhigh-10); 

  fithist->Fit(&ff,"QRN+");
  
  TF1::SetParameter(5,ff.GetParameter(5));
  TF1::SetParameter(6,ff.GetParameter(6));
  
  double bgpars[5];
  bgpars[0] = TF1::GetParameters()[0];
  bgpars[1] = TF1::GetParameters()[1];
  bgpars[2] = TF1::GetParameters()[2];
  bgpars[3] = TF1::GetParameters()[5];
  bgpars[4] = TF1::GetParameters()[6];
  //bgpars[5] = TF1::GetParameters()[7];
  
  
  TF1 fBGFit("background",GFunctions::StepBG,xlow,xhigh,6);
  fBGFit.SetNpx(1000);
  fBGFit.SetLineStyle(2);
  fBGFit.SetLineColor(kBlack);
  fBGFit.SetParameters(bgpars);

  fArea = this->Integral(xlow,xhigh) /  fithist->GetBinWidth(1);
  double bgArea = fBGFit.Integral(xlow,xhigh) / fithist->GetBinWidth(1);
  fArea -= bgArea;


  if(xlow>xhigh)
    std::swap(xlow,xhigh);
  fSum = fithist->Integral(fithist->GetXaxis()->FindBin(xlow),
                           fithist->GetXaxis()->FindBin(xhigh)); // * fithist->GetBinWidth(1);
  printf("sum between markers: %02f\n",fSum);
  fDSum = TMath::Sqrt(fSum);
  fSum -= bgArea;
  printf("sum after subtraction: %02f\n",fSum);


  //if(!verbose) {
    printf("exclude on hist: %s\n",fithist->GetName());
    Print();
    //printf("BG Area:         %.02f\n",bgArea);
    //printf("GetChisquared(): %.4f\n", TF1::GetChisquare());
    //printf("GetNDF():        %i\n",   TF1::GetNDF());
    //printf("GetProb():       %.4f\n", TF1::GetProb());
    //TF1::Print();
  //}
  
  fithist->GetListOfFunctions()->Clear();
  fithist->GetListOfFunctions()->Add(this);


  //Copy(*fithist->GetListOfFunctions()->FindObject(GetName()));
  //fithist->GetListOfFunctions()->Remove(fBGFit.GetName()); 
  fithist->GetListOfFunctions()->Add(fBGFit.Clone()); //use to be a clone.
  //DetachBackground();

  
  return true;


}
*/
/*
void GPeak2::Clear(Option_t *opt){
  TString options = opt;
  //Clear the GPeak2 including functions and histogram
  if(options.Contains("all"))
    TF1::Clear();
  init_flag = false;
  fArea  = 0.0;
  fDArea = 0.0;
  fSum   = 0.0;
  fDSum  = 0.0;
  fChi2  = 0.0;
  fNdf   = 0.0;
}
*/
/*
void GPeak2::Print(Option_t *opt) const {
  TString options = opt;
  printf(GREEN );
  printf("Name: %s \n", this->GetName());
  printf("Centroid:  %1f +/- %1f \n", this->GetParameter("centroid"),this->GetParError(GetParNumber("centroid")));
  printf("Area:      %1f +/- %1f \n", fArea, fDArea);
  printf("Sum:       %1f +/- %1f \n", fSum, fDSum);
  printf("FWHM:      %1f +/- %1f \n",this->GetFWHM(),this->GetFWHMErr());
  printf("Reso:      %1f%%  \n",this->GetFWHM()/this->GetParameter("centroid")*100.);
  printf("Chi^2/NDF: %1f\n",fChi2/fNdf);
  if(options.Contains("all")){
    TF1::Print(opt);
  }
  printf(RESET_COLOR);
  printf("\n");
}
*/
/*
void GPeak2::DrawResiduals(TH1 *hist) const{
  if(hist){
    return;
  }
  if(fChi2<0.000000001){
    printf("No fit performed\n");
    return;
  }
  Double_t xlow,xhigh;
  GetRange(xlow,xhigh);
  Int_t nbins = hist->GetXaxis()->GetNbins();
  Double_t *res = new Double_t[nbins];
  Double_t *bin = new Double_t[nbins];
  Int_t points = 0;
  for(int i =1;i<=nbins;i++) {
    if(hist->GetBinCenter(i) <= xlow || hist->GetBinCenter(i) >= xhigh)
      continue;
    res[points] = (hist->GetBinContent(i) - this->Eval(hist->GetBinCenter(i)))+ this->GetParameter("Height")/2;
    bin[points] = hist->GetBinCenter(i);
    points++;
  }
  new GCanvas();
  TGraph *residuals = new TGraph(points,bin,res);
  residuals->Draw("*AC");
  delete[] res;
  delete[] bin;
}
*/

//void GPeak2::DetachBackground() {
//  fBGFit.SetParent(0);
//  fBGFit.SetBit(TObject::kCanDelete,false);
//}
