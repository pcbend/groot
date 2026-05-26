
#include<GMarker.h>

#include<iostream>

#include<TH1.h>
#include<TList.h>
#include<TVirtualPad.h>
#include<TPad.h>
//#include<TTimer.h>
#include<TEnv.h>
#include<TCutG.h>

int GMarker::GetMaxMarkers(GMarkerType type) { 
  switch(type) {
    case GMarkerType::kPrimary:
      return gEnv->GetValue("GMarker.Primary.Max",2);
    case GMarkerType::kBackground:
      return gEnv->GetValue("GMarker.Background.Max",2);
    case GMarkerType::kZoom:
      return gEnv->GetValue("GMarker.Zoom.Max",2);
    case GMarkerType::kFit:
      return gEnv->GetValue("GMarker.Fit.Max",2);
    case GMarkerType::kCut:
      return gEnv->GetValue("GMarker.Cut.Max",2);
    case GMarkerType::kProjection:
      return gEnv->GetValue("GMarker.Projection.Max",2);
    default: 
      return 2;
  };
  return 2;
}

void GMarker::SetMaxMarkers(GMarkerType type,int value) { 
  switch(type) {
    case GMarkerType::kPrimary:
      gEnv->SetValue("GMarker.Primary.Max",value);
      break;
    case GMarkerType::kBackground:
      gEnv->SetValue("GMarker.Background.Max",value);
      break;
    case GMarkerType::kZoom:
      gEnv->SetValue("GMarker.Zoom.Max",value);
      break;
    case GMarkerType::kFit:
      gEnv->SetValue("GMarker.Fit.Max",value);
      break;
    case GMarkerType::kCut:
      gEnv->SetValue("GMarker.Cut.Max",value);
      break;
    case GMarkerType::kProjection:
      gEnv->SetValue("GMarker.Projection.Max",value);
      break;
    default: 
      return;
  };
  return;
}

GMarker::GMarker() :fHist(0), fLineX(0), fLineY(0), fX(sqrt(-1)), fY(sqrt(-1)) { 
  //SetLineWidth(2);
  //SetLineColor(kRed);
  //SetName("GMarker");


}

GMarker::~GMarker() {
  //printf("gmarker deleted\n");
  if(fLineX) delete fLineX;
  if(fLineY) delete fLineY;
} 

void GMarker::AddTo(TH1 *h, double x, double y,bool ignoreMax,Option_t *opt) {
  if(!h) return;
  fHist = h;
  //if(h && h->GetDimension() == 1) {
  x = fHist->GetXaxis()->GetBinLowEdge(fHist->GetXaxis()->FindBin(x));
  SetX(x);

  if(!fLineX) fLineX = new TLine;
  fLineX->SetLineWidth(2);
  fLineX->SetLineColor(kRed);

  if(h->GetDimension() == 2) {
    y = fHist->GetYaxis()->GetBinLowEdge(fHist->GetYaxis()->FindBin(y));
    SetY(y);

    if(!fLineY) fLineY = new TLine;
    fLineY->SetLineWidth(2);
    fLineY->SetLineColor(kRed);
  }

  if(!ignoreMax) {
    const int maxMarkers = GetMaxMarkers(GMarkerType::kPrimary);
    auto markers = Get(fHist,GMarkerType::kPrimary);

    while(maxMarkers >= 0 && static_cast<int>(markers.size()) >= maxMarkers) {
      markers.front()->Remove();
      markers.erase(markers.begin());
    }
  }
  fHist->GetListOfFunctions()->Add(this);
} 

void GMarker::Remove() {
  TH1* hist = fHist;  
  fHist = nullptr;
  if(hist && hist->GetListOfFunctions()) 
    hist->GetListOfFunctions()->Remove(this);
  delete this;

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

void GMarker::RemoveAll(TH1 *h,bool removeBGMarkers) {
  //remove all markers from h
  TIter iter(h->GetListOfFunctions(),kIterBackward);
  while(TObject *obj = iter.Next()) {
    if(obj->InheritsFrom(GMarker::Class())) {
      GMarker *marker = ((GMarker*)obj);
      if(marker->GetType()==GMarkerType::kBackground && removeBGMarkers)
        marker->Remove();
      else 
        marker->Remove();
    }
  }
}


std::vector<GMarker*> GMarker::Get(TH1 *h,GMarkerType type) {
  //type:
  //  - 0: all
  //  - 1: primary
  //  - 2: bg
  //return all markers in h
  std::vector<GMarker*> toReturn;
  if(!h) return toReturn;

  TIter iter(h->GetListOfFunctions());
  while(TObject *obj = iter.Next()) {
    if(!obj->InheritsFrom(GMarker::Class())) continue;
    auto *marker = static_cast<GMarker*>(obj);
    if(type == GMarkerType::kAll || marker->GetType() == type)
      toReturn.push_back(marker);

  }
  return toReturn;
}

void GMarker::SetLineColor(Color_t color) {
  if(fLineX) fLineX->SetLineColor(color);
  if(fLineY) fLineY->SetLineColor(color);
}

void GMarker::ExecuteEvent(int event, int px, int py) { 
  if(!gPad || !fHist) return;

  switch(event) {
    case kButton1Down:
      SetBit(kCannotPick, false);
      break;

    case kButton1Motion:
    case kButton1Up: {
      double x = gPad->PadtoX(gPad->AbsPixeltoX(px));
      x = fHist->GetXaxis()->GetBinLowEdge(fHist->GetXaxis()->FindBin(x));

      SetX(x);

      gPad->Modified();
      gPad->Update();
      break;
    }

    default:
      break;
  }
 return;









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


/****************
Current issue:
- Primary markers in 2D draw as full vertical/horizontal lines.
- This becomes visually cluttered while building polygon gates.

Desired direction:
- Marker appearance should depend on GMarkerType and/or interaction mode.
- Keep current functionality for 1D range markers.
- Improve temporary visualization for 2D gate construction.

Ideas:
- Different drawing styles by marker type:
    kPrimary      -> current crosshair
    kCut          -> point/small cross
    kProjection   -> line
    kZoom         -> dashed line
    kBackground   -> alternate color/style

- Possibly add:
    GMarker::UpdateStyle()
    GMarker::PaintAsPoint()
    GMarker::PaintAsCross()
    GMarker::PaintAsLine()

- While constructing polygon cuts:
    - draw connecting polyline between markers
    - avoid full-screen crosshair clutter

- Consider:
    marker rendering strategy depending on histogram dimension
    and current interaction context.
****************/




TCutG* GMarker::MakeTCutG(TH1* h,GMarkerType type) {
  if(!h || h->GetDimension() != 2) 
    return nullptr;

  auto markers = GMarker::Get(h,type);
  std::vector<double> x;
  std::vector<double> y;


  if(markers.size() < 2) {
    return nullptr;
  } else if(markers.size()==2) {
    double x1 = markers[0]->X();
    double y1 = markers[0]->Y();
    double x2 = markers[1]->X();
    double y2 = markers[1]->Y();

    if(x1 > x2) std::swap(x1, x2);
    if(y1 > y2) std::swap(y1, y2);

    //midpoints
    const double xm = 0.5 * (x1 + x2);
    const double ym = 0.5 * (y1 + y2);
    x = {x1, xm, x2, x2, x2, xm, x1, x1, x1};
    y = {y2, y2, y2, ym, y1, y1, y1, ym, y2};
  } else { // more than 2
    const int nMarkers = static_cast<int>(markers.size());

    for(int i = 0; i < nMarkers; ++i) {
      const int next = (i + 1) % nMarkers;
      const double x1 = markers[i]->X();
      const double y1 = markers[i]->Y();
      const double x2 = markers[next]->X();
      const double y2 = markers[next]->Y();

      x.push_back(x1);
      y.push_back(y1);

      //midpoints
      x.push_back(0.5 * (x1 + x2));
      y.push_back(0.5 * (y1 + y2));
    }
    x.push_back(x.front());
    y.push_back(y.front());
  }
  TCutG *cut = new TCutG("CUT",static_cast<int>(x.size()),x.data(),y.data());
  cut->SetLineWidth(2);
  cut->SetLineColor(2);
  return cut;
}







