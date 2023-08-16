
#include<GMarker.h>

#include<iostream>

#include<TH1.h>
#include<TList.h>
#include<TVirtualPad.h>
#include<TPad.h>
//#include<TTimer.h>

GMarker::GMarker() : fX(sqrt(-1)), fY(sqrt(-1)) { 
  SetLineWidth(2);
  SetLineColor(kRed);
}

GMarker::~GMarker() {
  printf("gmarker deleted\n");
} 

void GMarker::AddTo(TH1 *h, double x, double y,Option_t *opt) {
  if(!h) return;
  fHist = h;
  SetX1(x);
  SetX2(x);
  fX =x;  
  SetVertical();
  
  fHist->GetListOfFunctions()->Add(this);

  /*   this doesn't work.  One can change the range without setting the gPad...
  if(gPad && gPad->GetPrimitive(fHist->GetName())) { // the histogram is active and drawn to the screen
    //TQObject::Connect("TPad","RangeChanged()","GMarker",this,"Update()"); // this would connect all TPad::RangeChangeds
    TQObject::Connect(gPad,"RangeChanged()","GMarker",this,"Update()");
  }
  */
  return;
} 

void GMarker::Remove() {
  if(!fHist) return;
  fHist->GetListOfFunctions()->Remove(this);
  this->Delete();
}


void GMarker::Paint(Option_t *opt) {
  //printf("\t-in gmaker paint.\n");
  //fflush(stdout);
  if(fHist && fHist->GetDimension()==1) {
    SetY1(gPad->GetUymin());
    SetY2(gPad->GetUymax());
    //std::cout<<"\t\tuymin: " << gPad->GetUymin() << std::endl;
    //std::cout<<"\t\tuymax: " << gPad->GetUymax() << std::endl;
  }
  TLine::Paint(opt);
}

