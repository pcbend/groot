
#include "GROI.h"
#include "GMarker.h"
#include "TGraph.h"
#include "TH1.h"
#include "TF1.h"
#include "TButton.h"

#include "GCommands.h"
#include "GGaus.h"

GROI::GROI() : TNamed(), xlow(0), xhigh(0), fFit(0),fFill(0), fParent(0) {  }

GROI::GROI(const char* name, const char* title) : TNamed(name, title), 
  xlow(0), xhigh(0),fFit(0),fFill(0),fParent(0) {  }


GROI::GROI(GMarker *m1, GMarker *m2, const char* name, const char* title) : 
      GROI(m1->X(),m2->X(),name,title) {  }

GROI::GROI(double x1, double x2, const char* name, const char* title) : TNamed(name, title), 
  xlow(x1),xhigh(x2),fFit(0),fFill(0),fParent(0) { 
  //fMarker1 = new GMarker(*m1);
  //fMarker2 = new GMarker(*m2);
  //fMarker1->SetDrawOption("tohist");
  //fMarker2->SetDrawOption("tohist");
  //fMarker1->SetLineColor(kBlue);
  //fMarker2->SetLineColor(kBlue);

  if(xlow>xhigh) std::swap(xlow,xhigh);

}


GROI::~GROI() { 
  //if (fMarker1) delete fMarker1;
  //if (fMarker2) delete fMarker2;
  if(fFill)     delete fFill;
  if (fFit)     delete fFit;
}

void GROI::SetParent(TH1* parent) {
  fParent = parent;
}

void GROI::Update() {
  //printf("Updating ROI %s\n", GetName());
  CreateFill();
  if(fFit) {
  }
} 

void GROI::Draw(Option_t* opt) {
  //if(fParent) {
  //  this->Paint("same");
  //}
  //else {
  //  this->Paint();
  //}
  this->Paint();
}

void GROI::CreateFill() {
  if(!fParent) return;

  int binLow  = fParent->GetXaxis()->FindBin(xlow);
  int binHigh = fParent->GetXaxis()->FindBin(xhigh);

  int bins = binHigh-binLow;

  if(fFill)
    delete fFill;
  fFill = new TH1D("temp","temp",bins,xlow,xhigh);
  fFill->SetDirectory(0);

  for(int i=1;i<=bins;i++)
    fFill->SetBinContent(i,fParent->GetBinContent((i-1)+binLow));

  fFill->SetFillStyle(1001); // or any other fill style
  //fFill->SetFillColor(kBlue);    // or any other color
  fFill->SetFillColorAlpha(kBlue, 0.35);

  fFit = static_cast<TF1*>(GausFit(fParent,xlow,xhigh));

}


void GROI::Paint(Option_t* opt) {
  opt = "tohist";
  printf("Painting ROI %s with opt %s\n", GetName(),opt);
  //if (fMarker1 && fMarker2) {
    //fMarker1->Paint(opt);
    //fMarker2->Paint(opt);
  //}
  if(fParent && fParent->GetDimension() == 1) {
    //if(!fLow) {

    //}
    //if(!fHigh) {

    //}
    //if(!fFill) {
      this->CreateFill();
    //}
    fFill->Paint("same");
  }
}

void GROI::ExecuteEvent(int event, int px, int py) {
  //if(!fMarker1 || !fMarker2) {
  //  return;
  //}
  printf("event = %i\n",event);
  //printf("marker1 dist = %i\n",fMarker1->DistancetoPrimitive(px,py));
  //printf("marker2 dist = %i\n",fMarker2->DistancetoPrimitive(px,py));
  
  if(event == EEventType::kMouseMotion) {
    //if (fMarker1->DistancetoPrimitive(px,py) < fMarker2->DistancetoPrimitive(px,py)) {
    //  fCurrentMarker = fMarker1;
    //} else {
    //  fCurrentMarker = fMarker2;
    //}
  } else {
    //if(fCurrentMarker)
    //  fCurrentMarker->ExecuteEvent(event, px, py);
      //Paint();
    //  CreateFill();
    //  if(gPad) {
    //    gPad->Modified();
    //    gPad->Update();
    //  }
  }
}

int GROI::DistancetoPrimitive(Int_t px, Int_t py) {
  //if (fMarker1 && fMarker2) {
  //  int d1 = fMarker1->DistancetoPrimitive(px, py);
  //  int d2 = fMarker2->DistancetoPrimitive(px, py);
  //  return (d1 < d2) ? d1 : d2;
  //}
/*
  Int_t puxmin = gPad->XtoAbsPixel(gPad->GetUxmin());
  Int_t puymin = gPad->YtoAbsPixel(gPad->GetUymin());
  Int_t puxmax = gPad->XtoAbsPixel(gPad->GetUxmax());
  Int_t puymax = gPad->YtoAbsPixel(gPad->GetUymax());

  printf("fFill  = %i\n",fFill->DistancetoPrimitive(px,py));
  printf("puxmin = %i\n",puxmin);
  printf("puymin = %i\n",puymin);
  printf("puxmax = %i\n",puxmax);
  printf("puymax = %i\n\n",puymax);

      Double_t x  = gPad->AbsPixeltoX(px);
      Double_t x1 = gPad->AbsPixeltoX(px+1);
      
      Double_t y  = gPad->AbsPixeltoY(py);
      Double_t y1 = gPad->AbsPixeltoY(py+1);

  Int_t bin       = fFill->GetXaxis()->FindBin(x);
  Double_t binVal = fFill->GetBinContent(bin);
  //Int_t binsup   = fXaxis->FindFixBin(gPad->PadtoX(x1));


  printf("x      = %.02f\n",x);
  printf("x1     = %.02f\n",x1);
  printf("bin    = %i\n",bin);
  printf("binVal = %.02f\n\n\n",binVal);

  //printf("y  = %.02f\n",y);
  //printf("y1 = %.02f\n\n\n",y1);

*/

  return 9999;
}

void GROI::RemoveAll(TH1* h) {
  TIter iter(h->GetListOfFunctions());
  while(TObject *obj = iter.Next()) {
    if(obj->InheritsFrom(GROI::Class())) {
      if(h == static_cast<GROI*>(obj)->GetParent()) {
        h->GetListOfFunctions()->Remove(obj);
      }
    }
  }
}
