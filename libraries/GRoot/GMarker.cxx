
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

GMarker::~GMarker() { } 

void GMarker::AddTo(TH1 *h, double x, double y,Option_t *opt) {
  if(!h) return;
  fHist = h;
  //SetX1(fHist->GetXaxis()
  SetX1(x);
  SetX2(x);
  //SetY1(fHist->GetMinimum());
  //SetY2(fHist->GetMaximum());
  fX =x;  
  Update();
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

void GMarker::Update() { 
  //std::cout << "tpad range changed called!\n" << std::endl;
 
  //if(!gPad || fX!=fX) return;
  
  //TTimer *t = new TTimer();
  //t->Connect("Timeout()","GMarker",this,"doUpdate()");
  //t->Start(1,true);

  //gPad->Modified();
  //gPad->Update();
  //double ymin = gPad->GetUymin();
  //double ymax = gPad->GetUymax();
  //std::cout << "ymin: " << ymin << std::endl;
  //std::cout << "ymax: " << ymax << std::endl;
  //SetY1(ymin) ; 
  //SetY2(ymax);
  //Paint();
  //DrawLine(fX,ymin,fX,ymax);
  return;
}

/*
void GMarker::doUpdate() {
  double ymin = gPad->GetUymin();
  double ymax = gPad->GetUymax();
  std::cout << "ymin: " << ymin << std::endl;
  std::cout << "ymax: " << ymax << std::endl;
  SetY1(ymin) ; 
  SetY2(ymax);
  gPad->Modified();
  gPad->Update();

  return;
}
*/

//  1d
//  h->GetXaxis()->First() h->GetXaxis()->Last() h->GetMaximum() h->GetMinimum() 
//
//
//
