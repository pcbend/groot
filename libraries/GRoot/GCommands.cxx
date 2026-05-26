
#include <GCommands.h>

#include<TROOT.h>
#include<TVirtualPad.h>
#include<TList.h>
#include<TH1.h>
#include<TGraph.h>
#include<TF1.h>
#include<THStack.h>
#include<KeySymbols.h>
#include<TFile.h>
#include <TCutG.h>

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

TObject *GrabPlottable(int i) { 
  TObject *object = 0;
  if(!gPad)
    return object;
  TIter iter(gPad->GetListOfPrimitives());
  int j=0;
  while(TObject *obj = iter.Next()) {
    if(obj->InheritsFrom(TH1::Class())) {
      if(j==i) {
        object = obj;
        break;
      }
      j++;
    } else if(obj->InheritsFrom(TGraph::Class())) {
      if(j==i) {
        object = obj;
        break;
      }
      j++;
    }
  }
  return object;
}



void ls(int n) {
  if(gROOT->GetListOfFiles()->GetEntries()>n) {
    ((TFile*)(gROOT->GetListOfFiles()->At(n)))->ls();
  }
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

void SaveAllCuts(TH1 *hist,const char* fname,Option_t *opt) { 
  if(!hist || !hist->GetListOfFunctions()) return;
  std::vector<TObject*> cuts;
  TIter iter(hist->GetListOfFunctions());
  while(TObject *obj = iter.Next()) {
    if(obj->InheritsFrom(TCutG::Class())) 
      cuts.push_back(obj);
  }
  if(cuts.size()) {
    TDirectory *current = gDirectory;
    printf("opening file: %s with option %s\n",fname,opt);
    TFile *f = TFile::Open(fname,opt);
    for(size_t i=0;i<cuts.size();i++) {
      printf("\twriting %s\n",cuts.at(i)->GetName());
      cuts.at(i)->Write();
    }
    current->cd();
  }
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


GInteractionInfo BuildInteractionInfo()  {
  GInteractionInfo info;
  info.pad = gPad;
  if(!info.pad) return info;
  info.selected = info.pad->GetSelected();
  info.target   = GrabPlottable();
  info.event    = info.pad->GetEvent();
  info.px       = info.pad->GetEventX();
  info.py       = info.pad->GetEventY();
  info.x        = info.pad->PadtoX(info.pad->AbsPixeltoX(info.px));
  info.y        = info.pad->PadtoY(info.pad->AbsPixeltoY(info.py));
  return info;
}




// the below is meant to be added to a pad to make the 
// object interactable. This is currently being handled 
// automatically in the Creation of a GCanvas - it is 
// also passed to subpads using the GCanvas::Divide Method.
// need to be void to prevent useless printing.  
void GRootInteract() {

  GInteractionInfo info = BuildInteractionInfo();
  
  if(info.pad && gPad && (gPad != info.pad->GetSelectedPad())) 
    return;

  DispatchInteraction(info);
  return;
}

bool DispatchInteraction(GInteractionInfo &info) {
  bool temp = false;
  if(auto* h = dynamic_cast<TH1*>(info.target))
    temp = GRootInteractHist(h,info);
  else if(auto* gr = dynamic_cast<TGraph*>(info.target))
    temp = GRootInteractGraph(gr,info);
  if(info.modified && info.pad) {
    info.pad->Modified();
    info.pad->Update();
  }
  return temp;
}


bool GRootInteractGraph(TGraph *current,GInteractionInfo &info) {return true;}
bool GRootInteractGraphMouseButton(TGraph* current,GInteractionInfo &info) {return true;}
bool GRootInteractGraphKeyPress(TGraph* current,GInteractionInfo &info) {return true;}

bool GRootInteractHist(TH1 *current,GInteractionInfo &info) {
  switch(info.event) {
    case kNoEvent:           
      break;
    case kButton1Down:       
    case kButton2Down:       
    case kButton3Down:       
      GRootInteractHistMouseButton(current,info);
      break;
    case kKeyDown:           
    case kWheelUp:           
    case kWheelDown:         
      break;
    case kButton1Shift:      
      GRootInteractHistMouseButton(current,info);
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
      GRootInteractHistKeyPress(current,info);
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
  return true;
}

bool GRootInteractHistMouseButton(TH1* currentHist,GInteractionInfo &info) {
  if(!currentHist) return false;


  if(info.selected && info.selected->InheritsFrom(GMarker::Class()))
    return false; //should allow GMarker::ExecuteEvent deal with this...

  switch(info.event) {
    case kButton1Down:       
      //if(GCanvas::GetCurrentEvent().fState & kKeyControlMask) {
      //if(GCanvas::GetCurrentEvent().fState && currentHist) {
      {
        const bool ctrlPressed = GCanvas::GetCurrentEvent().fState & kKeyControlMask;

        GMarker *marker = new GMarker();
        marker->SetType(GMarkerType::kPrimary);
        marker->AddTo(currentHist,info.x,info.y,ctrlPressed);
        //gPad->Modified();
        info.modified = true;
      //} else {
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

bool GRootInteractHistKeyPress(TH1 *currentHist,GInteractionInfo &info) {
  //printf("key:  %i\t%i\t%i\n",event,px,py);
  std::vector<GMarker*> markers = GMarker::Get(currentHist,GMarkerType::kPrimary);
  switch(info.py) {
    case kKey_b:
      if(currentHist && currentHist->InheritsFrom(GH1D::Class())) {
        dynamic_cast<GH1D*>(currentHist)->ShowBackground();
        //gPad->Modified();
        info.modified = true;
      }
      break;
    case kKey_B:
      if(currentHist && currentHist->InheritsFrom(GH1D::Class())) {
        dynamic_cast<GH1D*>(currentHist)->ToggleBackground();
        info.modified = true;
        //gPad->Modified();
      }
      break;
    case kKey_c:
      if(markers.size()>1 && currentHist->GetDimension()==1) {
        markers.at(0)->SetType(GMarkerType::kBackground);
        markers.at(1)->SetType(GMarkerType::kBackground);
        info.modified = true;
        //gPad->Modified();
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
        //gPad->Modified();
        info.modified = true;
      }
      break;
    case kKey_g:
      if(currentHist->GetDimension()==1 && markers.size()>1) {
        GausFit(currentHist,markers.at(0)->X(),markers.at(1)->X());
        //gPad->Modified();
        info.modified = true;
        //GMarker::RemoveAll(currentHist);
      } else if(currentHist->GetDimension()==2 && markers.size()>1) {
        static int gGateCounter = 0;
        TCutG *cut = GMarker::MakeTCutG(currentHist);    
        cut->SetName(Form("cut%i",gGateCounter++));

        currentHist->GetListOfFunctions()->Add(cut);
        GMarker::RemoveAll(currentHist);

        info.modified = true;

      }
      break;
    case kKey_m:
      GMarker::RemoveAll(currentHist);
      //gPad->Modified();
      info.modified = true;
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
        info.modified = true;
        //gPad->Modified();
      }
      break;
    case kKey_o:
      currentHist->GetXaxis()->UnZoom();
      if(currentHist->GetDimension()==2) 
        currentHist->GetYaxis()->UnZoom();
      //gPad->Modified();
      info.modified = true;
      break;
    case kKey_p: 
      if(currentHist->InheritsFrom(GH1D::Class())) {
        GH1D *ghist = dynamic_cast<GH1D*>(currentHist);
        std::vector<GMarker*> bgmarkers = GMarker::Get(currentHist,GMarkerType::kBackground);
        if(markers.size()==2) {
          double xlow  = markers.at(0)->X();
          double xhigh = markers.at(1)->X();
          if(xlow>xhigh) std::swap(xlow,xhigh);
          GH2D *parent = dynamic_cast<GH2D*>(ghist->GetParent());
          if(parent) {
            GH1D *proj =0;
            //printf("markers.size() = %i\n",int(markers.size()));
            //printf("bgmarkers.size() = %i\n",int(bgmarkers.size()));
            if(bgmarkers.size()==2) {
              double bgxlow  = bgmarkers.at(0)->X();
              double bgxhigh = bgmarkers.at(1)->X();
              if(bgxlow>bgxhigh) std::swap(bgxlow,bgxhigh);
              if(currentHist->TestBits(GH1D::kProjectionX))
                proj = parent->ProjectionY(xlow,xhigh,bgxlow,bgxhigh);
              else
                proj = parent->ProjectionX(xlow,xhigh,bgxlow,bgxhigh);
            } else {
              if(currentHist->TestBits(GH1D::kProjectionX))
                proj = parent->ProjectionY(xlow,xhigh);
              else
                proj = parent->ProjectionX(xlow,xhigh);
            }
            //new GCanvas;
            proj->Draw();
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
      //gPad->Modified();
      info.modified = true;
      break;   
    case kKey_q:
      if(currentHist->InheritsFrom(GH1D::Class())) {
        GH1D *gcurrentHist = dynamic_cast<GH1D*>(currentHist);
        double xlow = gcurrentHist->GetXaxis()->GetBinLowEdge(gcurrentHist->GetXaxis()->GetFirst());
        double xup  = gcurrentHist->GetXaxis()->GetBinUpEdge(gcurrentHist->GetXaxis()->GetLast());
        gcurrentHist->Unbin(2);
        gcurrentHist->GetXaxis()->SetRangeUser(xlow,xup);
      }        
      //gPad->Modified();
      info.modified = true;
      break;   
    case kKey_s:
      if(currentHist->InheritsFrom(GH1D::Class())) {
        GH1D *gcurrentHist = dynamic_cast<GH1D*>(currentHist);
        gcurrentHist->ShowPeaks();
        //gPad->Modified();
        info.modified = true;
      }
      break;
    case kKey_x:
      if(currentHist->InheritsFrom(GH2D::Class())) {
        double xlow = currentHist->GetXaxis()->GetBinLowEdge(currentHist->GetXaxis()->GetFirst());
        double xup  = currentHist->GetXaxis()->GetBinUpEdge(currentHist->GetXaxis()->GetLast());
        currentHist->GetXaxis()->UnZoom();
        GH1D *px = dynamic_cast<GH2D*>(currentHist)->ProjectionX();
        px->SetBit(GH1D::kProjectionX,1);
        new GCanvas;
        px->Draw();
      }
      break;
    case kKey_y:
      if(currentHist->InheritsFrom(GH2D::Class())) {
        double ylow = currentHist->GetYaxis()->GetBinLowEdge(currentHist->GetYaxis()->GetFirst());
        double yup  = currentHist->GetYaxis()->GetBinUpEdge(currentHist->GetYaxis()->GetLast());
        currentHist->GetYaxis()->UnZoom();
        GH1D *py = dynamic_cast<GH2D*>(currentHist)->ProjectionY();
        py->SetBit(GH1D::kProjectionX,0);
        new GCanvas;
        py->Draw();
      }
      break;
    case kKey_l:
    case kKey_z:
      if(currentHist->GetDimension()==2) { 
        if(info.pad->GetLogz()) {
          currentHist->GetZaxis()->UnZoom();
          info.pad->SetLogz(0);
        } else {
          if(currentHist->GetMinimum()<0) 
            currentHist->GetZaxis()->SetRangeUser(0,currentHist->GetMaximum());
          info.pad->SetLogz(1);
        }
      } else {
        //printf("GetUymax:  %.2f\n",gPad->GetUxmax());
        if(info.pad->GetLogy()) {
          info.pad->SetLogy(0);
          //currentHist->GetYaxis()->SetRangeUser(0,gPad->GetUymax());
          currentHist->GetYaxis()->UnZoom();
        } else {
          if(info.pad->GetUymin()<0) 
            currentHist->GetYaxis()->SetRangeUser(0,gPad->GetUymax());
          info.pad->SetLogy(1);
        }
      }
      info.modified = true;
      //gPad->Modified();
    default:
      break;
  }

  return true;
}



