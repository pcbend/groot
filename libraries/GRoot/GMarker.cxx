
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

  //we want to limit how many marks are on the histogram.  
  //lets start with two....
  int fMaxMarkers=2;
  int markerCount = 0;
  TIter iter(fHist->GetListOfFunctions(),kIterBackward);
  while(TObject *obj=iter.Next()) {
    if(obj->InheritsFrom(GMarker::Class())) {
      markerCount++;
      if(markerCount>=fMaxMarkers)
        ((GMarker*)obj)->Remove();
    }
  }
    

  
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

void GMarker::RemoveAll(TH1 *h) {
  //remove all markers from h
  TIter iter(h->GetListOfFunctions(),kIterBackward);
  while(TObject *obj = iter.Next())
    if(obj->InheritsFrom(GMarker::Class()))
      ((GMarker*)obj)->Remove();

}

std::vector<GMarker*> GMarker::GetAll(TH1 *h) {
  //return all markers in h
  std::vector<GMarker*> toReturn;
  TIter iter(h->GetListOfFunctions());
  while(TObject *obj = iter.Next())
    if(obj->InheritsFrom(GMarker::Class()))
      toReturn.push_back(((GMarker*)obj));

  return toReturn;
}

