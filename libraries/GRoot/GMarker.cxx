
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
  x = fHist->GetBinLowEdge(fHist->FindBin(x));
  if(!fLineX) fLineX = new TLine;
  fLineX->SetLineWidth(2);
  fLineX->SetLineColor(kRed);
  fLineX->SetX1(x);
  fLineX->SetX2(x);
  //fX =x;
  SetX(x);
  fLineX->SetVertical();
  //} else if(h && h->GetDimension() == 2) {
  if(h->GetDimension() == 2) {
    //x = fHist->GetXaxis()->GetBinLowEdge(fHist->GetXaxis()->FindBin(x));
    y = fHist->GetYaxis()->GetBinLowEdge(fHist->GetYaxis()->FindBin(y));
    if(!fLineY) fLineY = new TLine;
    fLineY->SetLineWidth(2);
    fLineY->SetLineColor(kRed);
    fLineY->SetY1(y);
    fLineY->SetY2(y);
    SetY(y);
    fLineY->SetHorizontal();
  }
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


  if(!gPad || !fHist) return;

  if(fHist->GetDimension()==1) {
    if(!fLineX) 
      return;  // this should not be possible
    
  /*  if(!gPad->GetLogy()) {
      if(fLineX->TestBit(TLine::kLineNDC))
        fLineX->SetBit(TLine::kLineNDC,false);
      //else
      //  if(fX != fLineX->GetX1()) {
          //someone has moved the marker...
          //printf("GMarker X: %.02f\n",GetX1());
          fX = fLineX->GetX1();
      //  }
      fLineX->SetX1(fX);
      fLineX->SetX2(fX);
      fLineX->SetY1(gPad->GetUymin());
      if(sopt.Contains("tohist"))  
        fLineX->SetY2(fHist->GetBinContent(fHist->FindBin(fX)));
      else
        fLineX->SetY2(gPad->GetUymax());

      //double binval = fHist->GetBinContent(fHist->FindBin(fX));
      //int    pybin  = gPad->YtoAbsPixel(gPad->YtoPad(binval));    
      //printf("binval: %f\n",binval);
      //printf("pybin: %i\n",pybin);
      //printf("Uymin:  %f\n",gPad->GetUymin());
      //printf("Uymax:  %f\n",gPad->GetUymax());
    } else {  */
      if(!fLineX->TestBit(TLine::kLineNDC))
        fLineX->SetBit(TLine::kLineNDC,true);

      double lm = gPad->GetLeftMargin();
      double rm = 1.-gPad->GetRightMargin();
      double tm = 1.-gPad->GetTopMargin();
      double bm = gPad->GetBottomMargin();
      double xndc  = (rm-lm)*((fX-gPad->GetUxmin())/(gPad->GetUxmax()-gPad->GetUxmin()))+lm;
      fLineX->SetX1(xndc);
      fLineX->SetX2(xndc);
      double xuser = ((fLineX->GetX1()-lm)/(rm-lm))*(gPad->GetUxmax()-gPad->GetUxmin())+gPad->GetUxmin();
      if(fX !=xuser) {
        fX= xuser;
      }
      fLineX->SetY1(bm);    
      fLineX->SetY2(tm);    
      fLineX->SetBit(TLine::kLineNDC,true);
//    }  
    //std::cout<<"\t\tuymin: " << gPad->GetUymin() << std::endl;
    //std::cout<<"\t\tuymax: " << gPad->GetUymax() << std::endl;
  } else if (fHist->GetDimension()==2) {
    



  }
  
  if(fLineX) fLineX->Paint(sopt.Data());
  if(fLineY) fLineY->Paint(sopt.Data());

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
  if(!fLineX || fLineY) return;

  int d1 = 9999;
  int d2 = 9999;
  if(fLineX)
    d1 = fLineX->DistancetoPrimitive(px,py);
  if(fLineY)
    d2 = fLineY->DistancetoPrimitive(px,py);
   if(d1<d2) { 
     if(fLineX) 
       fLineX->ExecuteEvent(event,px,py);
   } else {
     if(fLineY)
       fLineY->ExecuteEvent(event,px,py);
   } 

  return;
} 


//TODO: Currently events are not being sent to the lines...
int GMarker::DistancetoPrimitive(int px, int py) { 
  int d1 = 9999;
  int d2 = 9999;
  if(fLineX)
    d1 = fLineX->DistancetoPrimitive(px,py);
  if(fLineY)
    d2 = fLineY->DistancetoPrimitive(px,py);
  return (d1 < d2) ? d1 : d2;
}














