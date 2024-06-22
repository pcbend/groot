
#include "GROI.h"
#include "GMarker.h"
#include "TGraph.h"
#include "TH1.h"

GROI::GROI() : TNamed(), fMarker1(0), fMarker2(0), fFill(0), fParent(0) {  }

GROI::GROI(const char* name, const char* title) : TNamed(name, title), 
  fMarker1(0), fMarker2(0),fFill(0),fParent(0) {  }


GROI::GROI(GMarker *m1, GMarker *m2, const char* name, const char* title) : TNamed(name, title), 
  fFill(0),fParent(0) { 
  fMarker1 = new GMarker(*m1);
  fMarker2 = new GMarker(*m2);
  //fMarker1->SetDrawOption("tohist");
  //fMarker2->SetDrawOption("tohist");
  fMarker1->SetLineColor(kBlue);
  fMarker2->SetLineColor(kBlue);

}

GROI::~GROI() { 
  if (fMarker1) delete fMarker1;
  if (fMarker2) delete fMarker2;
  if (fFill)    delete fFill;
}


void GROI::Draw(Option_t* opt) {
  if(fParent) {
    this->Paint("same");
  }
  else {
    this->Paint();
  }
}

void GROI::CreateFill() {
  //int left  = fParent->GetXaxis()->FinBin(fMarker1->GetX());
  //int right = fParent->GetXaxis()->FinBin(fMarker2->GetX());
  std::vector<double> x; 
  std::vector<double> y; 
  x.push_back(fMarker1->X());
  y.push_back(0);
  double step    = (fMarker2->X()-fMarker1->X())/200;
  double current = fMarker1->X();
  while(current <= fMarker2->X()) {
    x.push_back(current);
    y.push_back(fParent->GetBinContent(fParent->GetXaxis()->FindBin(current)));
    current += step;
  }
  x.push_back(fMarker2->X());
  y.push_back(0);
  x.push_back(fMarker1->X());
  y.push_back(0);

  if(fFill) {
    delete fFill;
  }
  fFill = new TGraph(x.size(),x.data(),y.data());

  fFill->SetFillStyle(3002); // or any other fill style
  fFill->SetFillColor(2);    // or any other color
  //gr->Draw("F");          // draw graph with fill area option
}


void GROI::Paint(Option_t* opt) {
  opt = "tohist";
  printf("Painting ROI %s with opt %s\n", GetName(),opt);
  if (fMarker1 && fMarker2) {
    fMarker1->Paint(opt);
    fMarker2->Paint(opt);
  }
  if(fParent && fParent->GetDimension() == 1) {
    if(!fFill) {
      this->CreateFill();
    }
    fFill->Paint("F");
  }
}

int GROI::DistancetoPrimitive(Int_t px, Int_t py) {
  if (fMarker1 && fMarker2) {
    int d1 = fMarker1->DistancetoPrimitive(px, py);
    int d2 = fMarker2->DistancetoPrimitive(px, py);
    return (d1 < d2) ? d1 : d2;
  }
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
