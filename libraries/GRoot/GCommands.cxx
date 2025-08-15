
#include <GCommands.h>

#include<TROOT.h>
#include<TVirtualPad.h>
#include<TList.h>
#include<TH1.h>
#include<TGraph.h>
#include<TF1.h>
#include<THStack.h>
#include<KeySymbols.h>

#include<GCanvas.h>
#include<GGaus.h>
#include<GPeak.h>
#include<GMarker.h>

#include<GH1D.h>
#include<GH2D.h>


GGaus *GausFit(TH1 *hist,double xlow, double xhigh,Option_t *opt) {
  if(!hist)
    return 0;
  if(xlow>xhigh)
    std::swap(xlow,xhigh);

  GGaus *mypeak= new GGaus(xlow,xhigh);
  std::string options = opt;
  options.append("Q+");
  mypeak->Fit(hist,options.c_str());
  //mypeak->Background()->Draw("SAME");
  //TF1 *bg = new TF1(*mypeak->Background());
  //hist->GetListOfFunctions()->Add(bg);

  double chi2 = GetChi2(hist,mypeak);
  printf("Cal chi2 = %.03f\n",chi2);

  return mypeak;
}

GPeak *PhotoPeakFit(TH1 *hist,double xlow, double xhigh,Option_t *opt) {
  if(!hist)
    return 0;
  if(xlow>xhigh)
    std::swap(xlow,xhigh);

  GPeak *mypeak= new GPeak((xlow+xhigh)/2.0,xlow,xhigh);
  std::string options = opt;
  options.append("Q+");
  mypeak->Fit(hist,options.c_str());
  //mypeak->Background()->Draw("SAME");
  //TF1 *bg = new TF1(*mypeak->Background());
  //hist->GetListOfFunctions()->Add(bg);

  double chi2 = GetChi2(hist,mypeak);
  printf("Cal chi2 = %.03f\n",chi2);

  return mypeak;
}


TH1 *GrabHist(int i)  {
  //return the histogram from the current canvas, pad i.
  TH1 *hist = 0;
  if(!gPad)
    return hist;
  TIter iter(gPad->GetListOfPrimitives());
  int j=0;
  while(TObject *obj = iter.Next()) {
    if(obj->InheritsFrom(TH1::Class())) {
      if(j==i) {
        hist = (TH1*)obj;
        break;
      }
      j++;
    } else if(obj->InheritsFrom(THStack::Class())) {
      hist = ((THStack*)obj)->GetHistogram();
      break;
    }
  }
  return hist;
}




TList *GrabHists(TVirtualPad *p) {
  //return all histograms on a canvas or pad. (default is the gPad);
  TList *histList = new TList;
  if(!p) p = gPad;
  if(!p) return histList;

  TVirtualPad *current = gPad;
  //TCanvas *c = p->GetCanvas();

  TIter nextp(p->GetListOfPrimitives());
  while(TObject *obj = nextp()) {
    //printf("obj->GetName() = %s\n",obj->GetName());
    if(obj->InheritsFrom(TVirtualPad::Class())) {
      TList *temp = GrabHists((TVirtualPad*)obj);
      TIter nextp2(temp);
      while(TObject *obj2 = nextp2()) histList->Add(obj2);
    }else if(obj->InheritsFrom(TH1::Class())) {
      histList->Add(obj);
    }
  }  
  //printf("found %i histograms...\n",histList->GetEntries());
  return histList;
}

TF1 *GrabFit(int i)  {
  //return the histogram from the current canvas, pad i.
  TH1 *hist = 0;
  TF1 *fit = 0;
  if(!gPad)
    return fit;
  TIter iter(gPad->GetListOfPrimitives());
  int j=0;
  while(TObject *obj = iter.Next()) {
    if(obj->InheritsFrom(TH1::Class())) {
      hist = (TH1*)obj;
      TIter iter2(hist->GetListOfFunctions());
      while(TObject *obj2 = iter2.Next()){
        if(obj2->InheritsFrom(TF1::Class())){
          if(j==i) {
            fit=(TF1*)obj2;
            return fit;
          }
          j++;
        }
      }
    }
  }
  return fit;
}



double GetChi2(TObject *obj,TF1 *f=0) {
  if(obj->InheritsFrom(TGraph::Class())) {
    /*
       TGraph *gr = (TGraph*)obj;
       if(f==0) 
       if(gr->GetListOfFunctions()->GetEntries()) 
       f = (TF1*)gr->GetListOfFunctions()->Last();
       if(f==0)
       return sqrt(-1);

       double low,high;
       f->GetRange(low,high);
       double *x = gr->GetX();
       double *y = gr->GetY();
       double chi2 = 0;
       for(int i=0;i<11;i++){            //help 
       double yf = fx->Eval(x[i]);
       chi2 += pow((yf-y[i]),2)/y[i];
       }

       printf("chi2 = %f\n",chi2/10); // 10 is number of dat
     */
    //FIXME
    return sqrt(-1);

  } else if(obj->InheritsFrom(TH1::Class())) {
    TH1 *hist = (TH1*)obj;
    if(hist->GetDimension()>1)
      return sqrt(-1);

    if(f==0) 
      if(hist->GetListOfFunctions()->GetEntries()) 
        f = (TF1*)hist->GetListOfFunctions()->Last();
    if(f==0)
      return sqrt(-1);

    double low,high;
    f->GetRange(low,high);
    int binLow = hist->FindBin(low);
    int binHigh = hist->FindBin(high);

    //double *x = gr->GetX();
    //double *y = gr->GetY();
    double chi2 = 0;
    int pzero = 0;
    for(int i=binLow;i<=binHigh;i++){            //help 
      double obs = hist->GetBinContent(i);
      if(obs==0){
        pzero++;
        continue;
      }
      double cal = f->Eval(hist->GetBinCenter(i));
      chi2 += pow((obs-cal),2)/obs;
    }
    int NDF = binHigh-binLow+1-pzero;
    //printf("chi2 = %f\n",chi2/(NDF-1)); // 10 is number of dat

    return chi2/(NDF-1);
  }
  return sqrt(-1);
}



// the below is meant to be added to a pad to make the 
// object interactable. This is currently being handled 
// automatically in the Creation of a GCanvas - it is 
// also passed to subpads using the GCanvas::Divide Method.
// need to be void to prevent useless printing.  
void Interact() {

  //printf("gPad:           %p\n",gPad);
  //printf("GetSelectedPad: %p\n",gPad->GetSelectedPad());
  //printf("---------\n\n");
  if(gPad && gPad->GetSelectedPad())
    if(gPad != gPad->GetSelectedPad()) 
      return;
  //at this point, we should have the pad under the canvas.

  //gPad->cd();

  Int_t event = gPad->GetEvent(); // Get the type of event (e.g., kButton1Down, kMouseMotion)
  Int_t px = gPad->GetEventX();   // Get the X-coordinate of the event in pixels
  Int_t py = gPad->GetEventY();   // Get 

  //printf("gPad:             %p\n",gPad);
  //printf("gPad GetNumber:   %i\n",gPad->GetNumber());
  //printf("gPad GetSelected: %p\n",gPad->GetSelected());
  //printf("GrabHist():       %p\n",GrabHist());

  //printf("event:          %i\n",event);
  //printf("px:             %i\n",px);
  //printf("py:             %i\n",py);


  switch(event) {
    case kNoEvent:           
      break;
    case kButton1Down:       
    case kButton2Down:       
    case kButton3Down:       
      InteractMouseButton(event,px,py);
      break;
    case kKeyDown:           
    case kWheelUp:           
    case kWheelDown:         
      break;
    case kButton1Shift:      
      InteractMouseButton(event,px,py);
      break;
    case kButton1ShiftMotion:
    case kButton1Up:         
    case kButton2Up:         
    case kButton3Up:         
    case kKeyUp:             
      break;
    case kButton1Motion:     
    case kButton2Motion:     
    case kButton3Motion:     
      break;
    case kKeyPress:          
      InteractKeyPress(event,px,py);
      break;
    case kArrowKeyPress:     
    case kArrowKeyRelease:   
      break;
    case kButton1Locate:     
    case kButton2Locate:     
    case kButton3Locate:     
      break;
    case kESC:               
      break;
    case kMouseMotion:       
    case kMouseEnter:        
    case kMouseLeave:        
      break;
    case kButton1Double:     
    case kButton2Double:     
    case kButton3Double:     
      break;
    default:
      break;
  }

  gPad->Update();

  return;
}

bool InteractMouseButton(int event, int px, int py) {
  TH1 *currentHist = GrabHist();
  double x  = gPad->PadtoX(gPad->AbsPixeltoX(px));
  double y  = gPad->PadtoY(gPad->AbsPixeltoY(py));

  //printf("event:  %i\n",event);
  //printf("px:     %i\n",px);
  //printf("py:     %i\n",py);

  //printf("x: %.2f\n",x);
  //printf("y: %.2f\n",y);
  //printf("------------\n");
  switch(event) {
    case kButton1Down:       
      if(GCanvas::GetCurrentEvent().fState & kKeyControlMask) {
        GMarker *marker = new GMarker();
        marker->AddTo(currentHist,x,y);
        gPad->Modified();
      } else {
        //printf("button1\n");
      }
      break;
    case kButton2Down:       
      //printf("button2\n");
      break;
    case kButton3Down:       
      //printf("button3\n");
      break;
    case kButton1Shift:
      //printf("shiftbutton1\n");
      break;
  }

  return true;
}

bool InteractKeyPress(int event, int px, int py) {
  //printf("key:  %i\t%i\t%i\n",event,px,py);
  TH1 *currentHist = GrabHist();
  std::vector<GMarker*> markers = GMarker::GetAll(currentHist);
  switch(py) {
    case kKey_b:
      if(currentHist->InheritsFrom(GH1D::Class())) {
        dynamic_cast<GH1D*>(currentHist)->ShowBackground();
        gPad->Modified();
      }
      break;
    case kKey_B:
      if(currentHist->InheritsFrom(GH1D::Class())) {
        dynamic_cast<GH1D*>(currentHist)->ToggleBackground();
        gPad->Modified();
      }
      break;
    case kKey_e:
      if(markers.size()>1) {
        double xlow  = markers.at(0)->X();
        double xhigh = markers.at(1)->X();
        if(xlow>xhigh) std::swap(xlow,xhigh);
        currentHist->GetXaxis()->SetRangeUser(xlow,xhigh);
        if(currentHist->GetDimension()==2) {
          double ylow  = markers.at(0)->Y();
          double yhigh = markers.at(1)->Y();
          if(ylow>yhigh) std::swap(ylow,yhigh);
          currentHist->GetYaxis()->SetRangeUser(ylow,yhigh);
        }
        GMarker::RemoveAll(currentHist);
        gPad->Modified();
      }
      break;
    case kKey_g:
      if(currentHist->GetDimension()==1 && markers.size()>1) {
        GausFit(currentHist,markers.at(0)->X(),markers.at(1)->X());
        gPad->Modified();
        //GMarker::RemoveAll(currentHist);
      }
      break;
    case kKey_m:
      GMarker::RemoveAll(currentHist);
      gPad->Modified();
      break;
    case kKey_n:
      if(currentHist) {
        TListIter iter(currentHist->GetListOfFunctions());
        std::vector<TF1*> funcs;
        while(TObject *obj=iter.Next()) {
          if(obj->InheritsFrom(TF1::Class()))
            funcs.push_back(((TF1*)obj));
        }
        for(auto i=funcs.begin();i!=funcs.end();i++)
          currentHist->GetListOfFunctions()->Remove(*i);
        GMarker::RemoveAll(currentHist);
        currentHist->Sumw2(false);
        if(currentHist->InheritsFrom(GH1D::Class())) {
          GH1D *gcurrentHist = dynamic_cast<GH1D*>(currentHist);
          gcurrentHist->RemovePeaks();
        }
        gPad->Modified();
      }
      break;
    case kKey_o:
      currentHist->GetXaxis()->UnZoom();
      if(currentHist->GetDimension()==2) 
        currentHist->GetYaxis()->UnZoom();
      gPad->Modified();
      break;
    case kKey_p: 
      if(currentHist->InheritsFrom(GH1D::Class())) {
        if(markers.size()>1) {
          double xlow  = markers.at(0)->X();
          double xhigh = markers.at(1)->X();
          if(xlow>xhigh) std::swap(xlow,xhigh);
          GH2D *parent = dynamic_cast<GH2D*>(dynamic_cast<GH1D*>(currentHist)->GetParent());
          if(parent) {
            GH1D * p =parent->ProjectionY(xlow,xhigh);
            new GCanvas;
            p->Draw();
          }
        }
      }
      break;
    case kKey_w:
      if(currentHist->InheritsFrom(GH1D::Class())) {
        double xlow = currentHist->GetXaxis()->GetBinLowEdge(currentHist->GetXaxis()->GetFirst());
        double xup  = currentHist->GetXaxis()->GetBinUpEdge(currentHist->GetXaxis()->GetLast());
        currentHist->Rebin(2);
        currentHist->GetXaxis()->SetRangeUser(xlow,xup);
      }
      gPad->Modified();
      break;   
    case kKey_q:
      if(currentHist->InheritsFrom(GH1D::Class())) {
        GH1D *gcurrentHist = dynamic_cast<GH1D*>(currentHist);
        double xlow = gcurrentHist->GetXaxis()->GetBinLowEdge(gcurrentHist->GetXaxis()->GetFirst());
        double xup  = gcurrentHist->GetXaxis()->GetBinUpEdge(gcurrentHist->GetXaxis()->GetLast());
        gcurrentHist->Unbin(2);
        gcurrentHist->GetXaxis()->SetRangeUser(xlow,xup);
      }        
      gPad->Modified();
      break;   
    case kKey_s:
      if(currentHist->InheritsFrom(GH1D::Class())) {
        GH1D *gcurrentHist = dynamic_cast<GH1D*>(currentHist);
        gcurrentHist->ShowPeaks();
        gPad->Modified();
      }
      break;
    case kKey_x:
      if(currentHist->InheritsFrom(GH2D::Class())) {
        double xlow = currentHist->GetXaxis()->GetBinLowEdge(currentHist->GetXaxis()->GetFirst());
        double xup  = currentHist->GetXaxis()->GetBinUpEdge(currentHist->GetXaxis()->GetLast());
        currentHist->GetXaxis()->UnZoom();
        GH1D *px = dynamic_cast<GH2D*>(currentHist)->ProjectionX();
        new GCanvas;
        px->Draw();
      }
      break;
    case kKey_l:
    case kKey_z:
      if(currentHist->GetDimension()==2) { 
        if(gPad->GetLogz()) {
          currentHist->GetZaxis()->UnZoom();
          gPad->SetLogz(0);
        } else {
          if(currentHist->GetMinimum()<0) 
            currentHist->GetZaxis()->SetRangeUser(0,currentHist->GetMaximum());
          gPad->SetLogz(1);
        }
      } else {
        //printf("GetUymax:  %.2f\n",gPad->GetUxmax());
        if(gPad->GetLogy()) {
          gPad->SetLogy(0);
          //currentHist->GetYaxis()->SetRangeUser(0,gPad->GetUymax());
          currentHist->GetYaxis()->UnZoom();
        } else {
          if(gPad->GetUymin()<0) 
            currentHist->GetYaxis()->SetRangeUser(0,gPad->GetUymax());
          gPad->SetLogy(1);
        }
      }
      gPad->Modified();
    default:
      break;
  }

  return true;
}



