
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
#include<TPad.h>
#include<TString.h>

#include<algorithm>
#include<array>

#include<GCanvas.h>
#include<GGaus.h>
#include<GPeak.h>
#include<GMarker.h>

#include<GH1D.h>
#include<GH2D.h>

namespace {
TVirtualPad* gRequestedCurrentPad = nullptr;

TVirtualPad* ResolveInteractionPad(TVirtualPad* pad) {
  if(!pad)
    return nullptr;

  TString name(pad->GetName());
  if(name.BeginsWith("__gh1d_residual_") && name.EndsWith("_residual")) {
    TString histPadName = name;
    histPadName.Resize(histPadName.Length() - TString("_residual").Length());
    histPadName.Append("_hist");

    auto* tpad = dynamic_cast<TPad*>(pad);
    TVirtualPad* parent = tpad ? tpad->GetMother() : nullptr;
    if(!parent || !parent->GetListOfPrimitives())
      return pad;

    TObject* obj = parent->GetListOfPrimitives()->FindObject(histPadName);
    if(obj && obj->InheritsFrom(TVirtualPad::Class()))
      return static_cast<TVirtualPad*>(obj);

    return pad;
  }

  if(pad->GetListOfPrimitives()) {
    TIter iter(pad->GetListOfPrimitives());
    while(TObject* obj = iter.Next()) {
      if(!obj->InheritsFrom(TVirtualPad::Class()))
        continue;
      TString childName(obj->GetName());
      if(childName.BeginsWith("__gh1d_residual_") && childName.EndsWith("_hist"))
        return static_cast<TVirtualPad*>(obj);
    }
  }

  return pad;
}

GInteractionInfo BuildInteractionInfo(TVirtualPad* commandPad,TVirtualPad* eventPad)  {
  GInteractionInfo info;
  info.pad = commandPad;
  if(!info.pad) return info;
  if(!eventPad) eventPad = commandPad;
  info.selected = eventPad->GetSelected();
  info.target   = GrabPlottable();
  if(info.selected)
    info.selectedName = info.selected->GetName();
  if(info.target)
    info.targetName = info.target->GetName();
  info.event    = eventPad->GetEvent();
  info.px       = eventPad->GetEventX();
  info.py       = eventPad->GetEventY();
  info.x        = eventPad->PadtoX(eventPad->AbsPixeltoX(info.px));
  info.y        = eventPad->PadtoY(eventPad->AbsPixeltoY(info.py));
  return info;
}
}


GGaus *GausFit(TH1 *hist,double xlow, double xhigh,Option_t *opt) {
  if(!hist)
    return 0;
  if(xlow>xhigh)
    std::swap(xlow,xhigh);

  const std::string requestedOptions = opt ? opt : "";
  GGaus *mypeak= new GGaus(xlow,xhigh);
  std::string options = requestedOptions;
  options.append("Q+");
  mypeak->Fit(hist,options.c_str());
  //mypeak->Background()->Draw("SAME");
  //TF1 *bg = new TF1(*mypeak->Background());
  //hist->GetListOfFunctions()->Add(bg);

  double chi2 = GetChi2(hist,mypeak);
  if(requestedOptions.find("no-print") == std::string::npos)
    printf("Cal chi2 = %.03f\n",chi2);

  if(auto* ghist = dynamic_cast<GH1D*>(hist))
    ghist->SetResidualFit(mypeak,xlow,xhigh);

  return mypeak;
}

GDoubleGaus *DoubleGausFit(TH1 *hist,double x1, double x2,double x3, double x4,Option_t *opt) {
  if(!hist)
    return 0;

  std::array<double,4> values = {x1,x2,x3,x4};
  std::sort(values.begin(),values.end());

  const double xlow  = values[0];
  const double cent1 = values[1];
  const double cent2 = values[2];
  const double xhigh = values[3];

  const std::string requestedOptions = opt ? opt : "";
  GDoubleGaus *mypeak= new GDoubleGaus(cent1,cent2,xlow,xhigh);
  std::string options = requestedOptions;
  options.append("Q+");
  mypeak->Fit(hist,cent1,cent2,options.c_str());
  //mypeak->Background()->Draw("SAME");
  //TF1 *bg = new TF1(*mypeak->Background());
  //hist->GetListOfFunctions()->Add(bg);

  double chi2 = GetChi2(hist,mypeak);
  if(requestedOptions.find("no-print") == std::string::npos)
    printf("Cal chi2 = %.03f\n",chi2);

  if(auto* ghist = dynamic_cast<GH1D*>(hist))
    ghist->SetResidualFit(mypeak,xlow,xhigh);

  return mypeak;
}

GPeak *PhotoPeakFit(TH1 *hist,double xlow, double xhigh,Option_t *opt) {
  if(!hist)
    return 0;
  if(xlow>xhigh)
    std::swap(xlow,xhigh);

  const std::string requestedOptions = opt ? opt : "";
  GPeak *mypeak= new GPeak((xlow+xhigh)/2.0,xlow,xhigh);
  std::string options = requestedOptions;
  options.append("Q+");
  mypeak->Fit(hist,options.c_str());
  //mypeak->Background()->Draw("SAME");
  //TF1 *bg = new TF1(*mypeak->Background());
  //hist->GetListOfFunctions()->Add(bg);

  double chi2 = GetChi2(hist,mypeak);
  if(requestedOptions.find("no-print") == std::string::npos)
    printf("Cal chi2 = %.03f\n",chi2);

  if(auto* ghist = dynamic_cast<GH1D*>(hist))
    ghist->SetResidualFit(mypeak,xlow,xhigh);

  return mypeak;
}


TH1 *GrabHist(int i)  {
  //return the histogram from the current canvas, pad i.
  TH1 *hist = 0;
  if(!gPad)
    return hist;

  // GrabHist intentionally follows gPad. Commands that create a new canvas
  // from a TExec should call RequestCurrentPad(); GCanvas applies that request
  // after ROOT's HandleInput restores its saved pad. This keeps the prompt's
  // current pad correct without adding hidden histogram state here.
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
      TList* hists = static_cast<THStack*>(obj)->GetHists();
      if(!hists)
        continue;
      TIter stackIter(hists);
      while(TObject* stackObj = stackIter.Next()) {
        if(!stackObj->InheritsFrom(TH1::Class()))
          continue;
        if(j==i) {
          hist = static_cast<TH1*>(stackObj);
          break;
        }
        j++;
      }
      if(hist)
        break;
    }
  }
  return hist;
}

void RequestCurrentPad(TVirtualPad* pad) {
  gRequestedCurrentPad = pad;
}

TVirtualPad* TakeRequestedCurrentPad() {
  TVirtualPad* pad = gRequestedCurrentPad;
  gRequestedCurrentPad = nullptr;
  return pad;
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


TH1D  *ResidualHist(TH1* hist, TF1* fit) { 
  if(!hist) return nullptr;
  if(!fit)  fit = GrabFit();
  if(!fit)  return nullptr;

  double xlow, xhigh;
  fit->GetRange(xlow, xhigh);

  auto* res = new TH1D(
    Form("%s_residuals", fit->GetName()),
    "Residuals;X;Data-Fit",
    hist->GetNbinsX(),
    hist->GetXaxis()->GetXmin(),
    hist->GetXaxis()->GetXmax()
  );

  res->SetDirectory(nullptr);

  for(int i = 1; i <= hist->GetNbinsX(); ++i) {
    const double x = hist->GetBinCenter(i);

    if(x < xlow || x > xhigh)
      continue;

    const double data = hist->GetBinContent(i);
    const double model = fit->Eval(x);
    const double err = hist->GetBinError(i);

    double r = data - model;
    //if(normalized && err > 0)
    //  r /= err;
    res->SetBinContent(i, r);
  }
  return res;
}

void   DrawResiduals(TH1* hist, TF1* fit,bool normalized) { }


GInteractionInfo BuildInteractionInfo()  {
  return BuildInteractionInfo(gPad,gPad);
}

namespace {
  GInteractionInfo gLastInteractionInfo;
}

const GInteractionInfo& GetLastInteractionInfo() {
  return gLastInteractionInfo;
}


// the below is meant to be added to a pad to make the 
// object interactable. This is currently being handled 
// automatically in the Creation of a GCanvas - it is 
// also passed to subpads using the GCanvas::Divide Method.
// need to be void to prevent useless printing.  
void GRootInteract() {

  TVirtualPad* eventPad = gPad;
  if(eventPad && eventPad->GetSelectedPad())
    eventPad = eventPad->GetSelectedPad();

  TVirtualPad* commandPad = ResolveInteractionPad(eventPad);
  if(commandPad && commandPad != gPad)
    commandPad->cd();

  GInteractionInfo info = BuildInteractionInfo(commandPad,eventPad);
  gLastInteractionInfo = info;

  DispatchInteraction(info);

  if(info.modified && info.pad) {
    info.pad->Modified();
    info.pad->Update();
  }
  gLastInteractionInfo = info;

  return;
}

bool DispatchInteraction(GInteractionInfo &info) {
  bool temp = false;
  if(auto* h = dynamic_cast<TH1*>(info.target))
    temp = GRootInteractHist(h,info);
  else if(auto* gr = dynamic_cast<TGraph*>(info.target))
    temp = GRootInteractGraph(gr,info);
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
  if(info.selected && info.selected->InheritsFrom(TCutG::Class()))
    return false; // let ROOT/TCutG handle dragging/editing existing cuts

  switch(info.event) {
    case kButton1Down:       
      //if(GCanvas::GetCurrentEvent().fState & kKeyControlMask) {
      //if(GCanvas::GetCurrentEvent().fState && currentHist) {
      {
        const bool ctrlPressed = GCanvas::GetCurrentEvent().fState & kKeyControlMask;

        GMarker *marker = new GMarker();
        // Once a 2D cut has been started, further clicks add vertices rather
        // than replacing the temporary range selection.
        GMarkerType type = GMarkerType::kPrimary;
        if(ctrlPressed) {
          type = currentHist->GetDimension() == 1 ? GMarkerType::kFit : GMarkerType::kCut;
        } else if(currentHist->GetDimension() == 2 &&
                  !GMarker::Get(currentHist,GMarkerType::kCut).empty()) {
          type = GMarkerType::kCut;
        }
        marker->SetType(type);
        marker->AddTo(currentHist,info.x,info.y,false);
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
       TH1 *current = GrabHist();
       if(current) {
          GCanvas *c = new GCanvas();  //move to makedefcanvas?
          TH1 *h = current->DrawCopy();
          if(h) {
            h->SetDirectory(0);
            // TH1::DrawCopy clones the histogram's list of functions.  In
            // groot that list can contain interactive objects with their own
            // ownership rules (markers, fit helpers, residual helpers, ...).
            // The pop-out canvas is only a clean view of the histogram data,
            // so do not let the copied histogram own/delete those copied
            // helpers when the canvas is closed.
            if(h->GetListOfFunctions())
              h->GetListOfFunctions()->Clear("nodelete");
          }
          c->Modified();
          c->Update();
       }
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
      if(currentHist->GetDimension()==1) {
        auto* projection = dynamic_cast<GH1D*>(currentHist);
        auto* parent = projection ? dynamic_cast<GH2D*>(projection->GetParent()) : nullptr;
        if(!parent) {
          printf("Background ranges are only available on projections from a 2D histogram.\n");
          break;
        }
        if(markers.size()!=2) {
          printf("Select exactly two primary markers before setting a background range.\n");
          break;
        }
        // A background selection is one explicit interval. Replacing it keeps
        // the subsequent projection operation unambiguous.
        for(auto* marker : GMarker::GetBG(currentHist))
          marker->Remove();
        for(auto* marker : markers)
          marker->SetType(GMarkerType::kBackground);
        printf("Background range set: %.3f to %.3f\n",
               markers.at(0)->X(),markers.at(1)->X());
        info.modified = true;
        //gPad->Modified();
      } else if(currentHist->GetDimension()==2 && !markers.empty()) {
        // Start cut-vertex mode. Subsequent clicks add kCut vertices until
        // the cut is created with 'g' or cleared with 'm'.
        for(auto* marker : markers)
          marker->SetType(GMarkerType::kCut);
        info.modified = true;
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
    case kKey_f:
      if(currentHist->GetDimension()==1 && markers.size()>1) {
        PhotoPeakFit(currentHist,markers.at(0)->X(),markers.at(1)->X());
        info.modified = true;
      }
      break;
    case kKey_g:
      if(currentHist->GetDimension()==1) {
        auto fitMarkers = GMarker::Get(currentHist,GMarkerType::kFit);
        bool didFit = false;
        if(fitMarkers.size()==4) {
          DoubleGausFit(currentHist,
                        fitMarkers.at(0)->X(),
                        fitMarkers.at(1)->X(),
                        fitMarkers.at(2)->X(),
                        fitMarkers.at(3)->X());
          didFit = true;
        } else if(markers.size()>1) {
          GausFit(currentHist,markers.at(0)->X(),markers.at(1)->X());
          didFit = true;
          //gPad->Modified();
        }
        if(didFit) {
          GMarker::RemoveAll(currentHist);
          info.modified = true;
        }
      } else if(currentHist->GetDimension()==2) {
        static int gGateCounter = 0;
        TCutG *cut = GMarker::MakeTCutG(currentHist,GMarkerType::kCut);
        if(!cut)
          break;
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
        if(currentHist->InheritsFrom(GH1D::Class())) {
          GH1D *gcurrentHist = dynamic_cast<GH1D*>(currentHist);
          gcurrentHist->ClearResidual();
        }
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
            if(!bgmarkers.empty() && bgmarkers.size()!=2) {
              printf("Background selection is invalid: expected one range (two markers), found %zu.\n",
                     bgmarkers.size());
              break;
            }
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
            if(proj)
              proj->Draw();
          }
        }
      }
      break;
    case kKey_r:
      if(currentHist->InheritsFrom(GH1D::Class())) {
        dynamic_cast<GH1D*>(currentHist)->ToggleResiduals();
        // ToggleResiduals redraws and can replace/delete the pad that handled
        // this key event. Do not let the generic post-dispatch update touch
        // info.pad after the layout changes.
        info.modified = false;
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
        GCanvas *canvas = new GCanvas;
        px->Draw();
        RequestCurrentPad(canvas);
      }
      break;
   case kKey_X:
      if(currentHist->InheritsFrom(TH2::Class())) {
        TH2* temp = static_cast<TH2*>(currentHist);
        temp->SetShowProjectionX(1);   
      }
      break; 
    case kKey_y:
      if(currentHist->InheritsFrom(GH2D::Class())) {
        double ylow = currentHist->GetYaxis()->GetBinLowEdge(currentHist->GetYaxis()->GetFirst());
        double yup  = currentHist->GetYaxis()->GetBinUpEdge(currentHist->GetYaxis()->GetLast());
        currentHist->GetYaxis()->UnZoom();
        GH1D *py = dynamic_cast<GH2D*>(currentHist)->ProjectionY();
        py->SetBit(GH1D::kProjectionX,0);
        GCanvas *canvas = new GCanvas;
        py->Draw();
        RequestCurrentPad(canvas);
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

  if(info.modified && currentHist && currentHist->InheritsFrom(GH1D::Class())) {
    auto* ghist = dynamic_cast<GH1D*>(currentHist);
    if(ghist && ghist->ShowResiduals()) {
      ghist->UpdateResidualDisplay(info.pad);
      info.modified = false;
    }
  }

  return true;
}
