
#include<GMarker.h>

#include<iostream>

#include<TH1.h>
#include<TList.h>
#include<TVirtualPad.h>
#include<TPad.h>
//#include<TTimer.h>

GMarker::GMarker() :fHist(0), fLineX(0), fLineY(0), fX(sqrt(-1)), fY(sqrt(-1)) { 
  //SetLineWidth(2);
  //SetLineColor(kRed);
  SetName("GMarker");


}

GMarker::~GMarker() {
  //printf("gmarker deleted\n");
  if(fHist) fHist->GetListOfFunctions()->Remove(this);
  if(fLineX) delete fLineX;
  if(fLineY) delete fLineY;
} 

void GMarker::AddTo(TH1 *h, double x, double y,Option_t *opt) {
  if(!h) return;
  fHist = h;
  //if(h && h->GetDimension() == 1) {
  x = fHist->GetXaxis()->GetBinLowEdge(fHist->GetXaxis()->FindBin(x));

  if(!fLineX) fLineX = new TLine;
  fLineX->SetLineWidth(2);
  fLineX->SetLineColor(kRed);
  SetX(x);
  if(h->GetDimension() == 2) {
    y = fHist->GetYaxis()->GetBinLowEdge(fHist->GetYaxis()->FindBin(y));
    if(!fLineY) fLineY = new TLine;
    fLineY->SetLineWidth(2);
    fLineY->SetLineColor(kRed);
    SetY(y);
  }
  //printf("fLineX: %p\n",fLineX);
  //printf("fLineY: %p\n",fLineY);


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

  return;
} 

void GMarker::Remove() {
  if(!fHist) return;
  //fHist->GetListOfFunctions()->Remove(this);
  this->Delete();
}


void GMarker::Paint(Option_t *opt) {
  TString sopt(opt);
  sopt.ToLower();
  //if(sopt.Length() == 0) sopt = this->GetDrawOption();

  sopt.Append("ndc");

  if(!gPad || !fHist) return;


  double lm = gPad->GetLeftMargin();
  double rm = 1.-gPad->GetRightMargin();
  double tm = 1.-gPad->GetTopMargin();
  double bm = gPad->GetBottomMargin();
    
  double xndc  = (rm-lm)*((fX-gPad->GetUxmin())/(abs(gPad->GetUxmax()-gPad->GetUxmin())))+lm;
  double yndc  = (tm-bm)*((fY-gPad->GetUymin())/(abs(gPad->GetUymax()-gPad->GetUymin())))+bm;

/*
  printf("lm:  %.04f\n",lm);
  printf("rm:  %.04f\n",rm);
  printf("tm:  %.04f\n",tm);
  printf("bm:  %.04f\n",bm);

  printf("fX:  %.04f\n",fX);
  printf("fY:  %.04f\n",fY);
  printf("Uxmin: %.04f\n",gPad->GetUxmin());
  printf("Uxmax: %.04f\n",gPad->GetUxmax());
  printf("Uymin: %.04f\n",gPad->GetUymin());
  printf("Uymax: %.04f\n",gPad->GetUymax());



  printf("xndc:  %.04f\n",xndc);
  printf("yndc:  %.04f\n",yndc);
*/



  if(fLineX) {
    if(!fLineX->TestBit(TLine::kLineNDC))
      fLineX->SetBit(TLine::kLineNDC,true);
   
    fLineX->SetX1(xndc);
    fLineX->SetX2(xndc);
    /*
    double xuser = ((fLineX->GetX1()-lm)/(rm-lm))*(gPad->GetUxmax()-gPad->GetUxmin())+gPad->GetUxmin();
    if(fX !=xuser) {
      fX= xuser;
    }
    */
    fLineX->SetY1(bm);    
    fLineX->SetY2(tm);    
    fLineX->SetBit(TLine::kLineNDC,true);
  }
  if(fLineY) {
    if(!fLineY->TestBit(TLine::kLineNDC))
      fLineY->SetBit(TLine::kLineNDC,true);

    fLineY->SetY1(yndc);
    fLineY->SetY2(yndc);
    /*
    double yuser = ((fLineY->GetY1()-tm)/(bm-tm))*(gPad->GetUymax()-gPad->GetUymin())+gPad->GetUymin();
    if(fY !=yuser) {
      fY= yuser;
    }
    */
    fLineY->SetX1(lm);    
    fLineY->SetX2(rm);    
    fLineY->SetBit(TLine::kLineNDC,true);
  }

  if(fLineX) {  fLineX->Paint(sopt.Data()); }
  if(fLineY) {  fLineY->Paint(sopt.Data()); }

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

void GMarker::SetLineColor(Color_t color) {
  if(fLineX) fLineX->SetLineColor(color);
  if(fLineY) fLineY->SetLineColor(color);
}

void GMarker::ExecuteEvent(int event, int px, int py) { 
  //printf("GMarker::ExecuteEvent(%i,%i,%i)\n",event,px,py);
  if(!fLineX || fLineY) return;
  //printf("here\n");
  
  //if(fLineX)
  //  d1 = fLineX->DistancetoPrimitive(px,py);
  //if(fLineY)
  //  d2 = fLineY->DistancetoPrimitive(px,py);
  //printf("d1 = %i, d2 = %i\n",d1,d2);
  //if(d1<d2) { 
  if(fLineX) {
       //printf("here!"); 
    fLineX->ExecuteEvent(event,px,py);
   }
  // } else {
  //   if(fLineY)
  //     fLineY->ExecuteEvent(event,px,py);
  // } 

  return;
} 


//TODO: Currently events are not being sent to the lines...
int GMarker::DistancetoPrimitive(int px, int py) { 
  int d1 = 9999;
  int d2 = 9999;
  if(fLineX)
    d1 = fLineX->DistancetoPrimitive(px,py);
  //if(fLineY)
  //  d2 = fLineY->DistancetoPrimitive(px,py);
  //return (d1 < d2) ? d1 : d2;
  return d1;
}













