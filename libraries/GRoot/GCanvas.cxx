

#include <GCanvas.h>

#include <string>

#include <globals.h>
#include <GCommands.h>
#include <GMarker.h>
#include <GH1D.h>
#include <GH2D.h>

#include <KeySymbols.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TClass.h>
#include <TFrame.h>
#include <TAxis.h>

#include <TContextMenu.h>
#include <TRootCanvas.h>
#include <TVirtualX.h>


int GCanvas::fCanvasNumber = 0;


GCanvas::GCanvas(bool build) : 
  TCanvas(build) { Init(); }

GCanvas::GCanvas(const char* name, const char* title, int wtopx, int wtopy, int ww, int wh) :
  TCanvas(name,title,wtopx,wtopy,ww,wh) { Init(name,title); }

GCanvas::GCanvas(const char* name, const char* title, int ww, int wh) : 
  TCanvas(name,title,ww,wh) { Init(name,title); }

GCanvas::GCanvas(const char* name, const char* title,int form) :
  TCanvas(name,title,form) { Init(name,title); }

GCanvas::GCanvas(const char* name, int ww, int wh, int winid) :
  TCanvas(name,ww,wh,winid) { Init(name); }

GCanvas::~GCanvas() { }  

void GCanvas::Init(const char* name, const char* title) {
  std::string temp   = Form("canvas_%i",fCanvasNumber++); 
  std::string sname  = name;  
  std::string stitle = title;

  if(!sname.length())  this->SetName(temp.c_str());
  if(!stitle.length()) this->SetTitle(temp.c_str());

  gClient->Connect("ProcessedEvent(Event_t *,Window_t)","GCanvas",this,"EventProcessed(Event_t*)");

}

void GCanvas::EventProcessed(Event_t *event) {
//ProcessedEvent(int event, int x, int y, TObject *selected) {
  /*
  printf("-------------\n");
  printf("Event Processed\n");
  printf("\tevent  = %i\n",event->fCode);
  printf("\tevent  = 0x%08x\n",event->fCode);
  printf("\tstate  = 0x%08x\n",event->fState);  // contains modifier information...
  printf("\ttype   = 0x%08x\n",event->fType);   // 0 down, 1 up
  printf("\twindow = %lu\n",event->fWindow);
  printf("\tx      = 0x%08x\n",event->fX);
  printf("\ty      = 0x%08x\n",event->fY);
  */

  if(static_cast<unsigned long>(event->fWindow) != 
     static_cast<unsigned long>(gVirtualX->GetWindowID(this->GetCanvasID()))) 
    return;
  //switch(event->fCode) {

  if(event->fType!=0) return; // only look for key down...

  unsigned int keysym;
  char str[2];
  gVirtualX->LookupString(event, str, sizeof(str), keysym);


  //bool GCanvas::HandleArrowPress(EEventType event, int px, int py) {
  //switch(event->fCode) {
  switch(keysym) {
    case kKey_Left:
    case kKey_Up:
    case kKey_Right:
    case kKey_Down:
      //printf("arrow key pressed.\n");
      //printf("keysym = 0x%08x\n",keysym);
      HandleArrowPress(kArrowKeyPress,keysym,keysym,event->fState);
      break;
    defualt:
      break;
  }
  
  //printf("\tobject = %s\n",selected ? selected->GetName() : "");
  
  
  return;
}



void GCanvas::HandleInput(EEventType event, int px, int py) {
  bool handled = false;


  //check the event,
  switch(event) {
    case kKeyPress:
      handled = HandleKeyPress(event,px,py);
      break;
    //case kArrowKeyPress:
    //case kArrowKeyRelease:
    //  handled = HandleArrowPress(event,px,py);
    //  break;
    case kButton1Shift:
    case kButton1Down:
    case kButton1Up:
    case kButton1Double:
      //printf("clicked on %s\n",GetSelected()->IsA()->GetName());
      if(GetSelected()->InheritsFrom(TH1::Class()) ||
         GetSelected()->InheritsFrom(TFrame::Class()) )
        handled = HandleMouseButton1(event,px,py);
      break;
    case kButton1Motion:
    case kButton1ShiftMotion: 
      //handled = true;
      break;
    default:
      break;
  }
  //printf(RED "handled = %i" RESET_COLOR  "\n",handled);
  if(!handled) 
    TCanvas::HandleInput(event,px,py);
}


bool GCanvas::HandleMouseButton1(EEventType event, int px, int py) { 
  bool handled = false;
  TH1 *currentHist = 0;
  switch(event) {
    case kButton1Down:
      currentHist = GrabHist();
      //TVirtualPad *sPad = TCanvas::GetSelectedPad();
      if(gPad != TCanvas::GetSelectedPad()) {
        //switch the gPad to the clicked pad... 
        TCanvas::HandleInput(kButton2Down,px,py);
      } else if(currentHist && gPad) {
        GMarker *m = new GMarker();
        //printf(RED "adding marker\n" RESET_COLOR "\n"); fflush(stdout);
        double xx = gPad->AbsPixeltoX(px);
        double x  = gPad->PadtoX(xx);
        //int  binx = h->GetXaxis()->FindBin(x);
        m->AddTo(currentHist,x);
        handled = true;
        
        gPad->Modified();
        gPad->Update();
      }
      break;
    case kButton1Shift:
      currentHist = GrabHist();
      if(gPad != TCanvas::GetSelectedPad()) {
        TCanvas::HandleInput(kButton2Down,px,py);
      } else if(currentHist && gPad) {
        //unzoom hist?
        TVirtualPad *current = gPad;
        GCanvas *c = new GCanvas;
        c->cd();
        GMarker::RemoveAll(currentHist);      
        currentHist->DrawCopy();
        gPad->Modified();
        gPad->Update();

      }
      break;
      break;
    case kButton1Up:
      break;
    case kButton1Double:
      break;
    default:
      break; 
  }

  return handled;
}

bool GCanvas::HandleArrowPress(EEventType event, int px, int py,int mask) {
  bool handled  = false;
  bool doUpdate = false;
  TH1   *currentHist = 0;
  TList *gList = 0;
  //printf("HandleArrowPress()\tevent = %i\tpx = %i\tpy = %i\n",event,px,py); fflush(stdout);
  currentHist = GrabHist();
  gList = GrabHists(gPad->GetCanvas());
  switch(px) {
    //currentHist->GetXaxis()->GetXmin() // minimum x
    //currentHist->GetXaxis()->GetXmax() // maximum x
    //gPad->GetUxmin() // current min
    //gPAd->GetUxmax() // current max
    case kKey_Left:
      if(currentHist && currentHist->GetDimension()==1) {
        double halfWindow = fabs(0.5*(gPad->GetUxmax() - gPad->GetUxmin()));
        if((gPad->GetUxmin() - halfWindow)<currentHist->GetXaxis()->GetXmin()) 
          halfWindow = fabs(gPad->GetUxmin()-currentHist->GetXaxis()->GetXmin());
        if(mask&kKeyShiftMask) {
          TIter iter(gList);
          while(TH1 *hist = dynamic_cast<TH1*>(iter())) 
            hist->GetXaxis()->SetRangeUser(gPad->GetUxmin()-halfWindow,gPad->GetUxmax()-halfWindow);
        } else {
          currentHist->GetXaxis()->SetRangeUser(gPad->GetUxmin()-halfWindow,gPad->GetUxmax()-halfWindow);
        }
      } 
      handled  = true;
      doUpdate = true;
      break;
    case kKey_Up:
      break;
    case kKey_Right:
      if(currentHist && currentHist->GetDimension()==1) {
        double halfWindow = fabs(0.5*(gPad->GetUxmax() - gPad->GetUxmin()));
        if((gPad->GetUxmax() + halfWindow)>currentHist->GetXaxis()->GetXmax()) 
          halfWindow = fabs(currentHist->GetXaxis()->GetXmax() - gPad->GetUxmax());
        if(mask&kKeyShiftMask) {
          TIter iter(gList);
          while(TH1 *hist = dynamic_cast<TH1*>(iter())) 
            hist->GetXaxis()->SetRangeUser(gPad->GetUxmin()+halfWindow,gPad->GetUxmax()+halfWindow);
        } else {
          currentHist->GetXaxis()->SetRangeUser(gPad->GetUxmin()+halfWindow,gPad->GetUxmax()+halfWindow);
        }
      } 
      handled  = true;
      doUpdate = true;
      break;
    case kKey_Down:
      //printf("ARROW!!\n"); fflush(stdout);
      break;
    default:
      break;
  }
  if(doUpdate) {
    UpdateAllPads();
    //gPad->Modified();
    //gPad->Update();
  }
  return handled;
}


bool GCanvas::HandleKeyPress(EEventType event, int px, int py) {
  bool handled  = false;
  bool doUpdate = false;
  TH1*   currentHist = 0;
  TList* gList = 0;
  //printf("HandleKey()\tevent = %i\tpx = %i\tpy = %i\n",event,px,py); fflush(stdout);
  //printf("key: %i  %i  %i\n",event,px,py);
  switch(py) {
    case kKey_Space:
      printf("--space--\n");
      //printf("this->GetName() = %s\n",this->GetName());
      TCanvas::HandleInput(kButton2Down,px,py);
      handled = true;
      break;
    case kKey_b:
      currentHist = GrabHist();
      if(currentHist->InheritsFrom(GH1D::Class())) {
        //remember the current range...
        double xlow = currentHist->GetXaxis()->GetBinLowEdge(currentHist->GetXaxis()->GetFirst());
        double xup  = currentHist->GetXaxis()->GetBinUpEdge(currentHist->GetXaxis()->GetLast());
        dynamic_cast<GH1D*>(currentHist)->Background();
        //currentHist->GetXaxis()->SetRangeUser(xlow,xup); // and reset the range.
        handled  = true;
        doUpdate = true;
      }
      break;
    case kKey_B:
      // for some reason if I try to remove the background from multiple 
      // histograms drawn in different Pads on the same Canvas, the inital 
      // one always has it's range set to the full range.  It simply won't 
      // remember it's original range.  All test I can currently think of
      // say it should. (?) :/
      gList = GrabHists(gPad->GetCanvas());
      if(gList) {
        TIter iter(gList);
        while(TObject *obj = iter.Next()) {
          if(obj->InheritsFrom(GH1D::Class())) {
            GH1D *gcurrentHist = dynamic_cast<GH1D*>(obj);
            //remember the current range...
            double xlow = gcurrentHist->GetXaxis()->GetBinLowEdge(gcurrentHist->GetXaxis()->GetFirst());
            double xup  = gcurrentHist->GetXaxis()->GetBinUpEdge(gcurrentHist->GetXaxis()->GetLast());
            //printf("%s\t%.1f\t%.1f\n",gcurrentHist->GetName(),xlow,xup);
            gcurrentHist->Background();
            //printf("%s\t%.1f\t%.1f\n",gcurrentHist->GetName(),xlow,xup);
            //gcurrentHist->GetXaxis()->SetRangeUser(xlow,xup); // and rest the range.
          }
        }
        doUpdate = true;
        handled  = true;
      }  
      break;
    case kKey_e:
      currentHist = GrabHist();
      if(currentHist) {
        std::vector<GMarker*> markers = GMarker::GetAll(currentHist);
        if(markers.size()>1) {
          double xlow  = markers.at(0)->X();
          double xhigh = markers.at(1)->X();
          if(xlow>xhigh) std::swap(xlow,xhigh);
          currentHist->GetXaxis()->SetRangeUser(xlow,xhigh);
          GMarker::RemoveAll(currentHist);
          doUpdate=true;
          handled=true;
        }
      }
      break;
    case kKey_E:
      currentHist = GrabHist();
      if(currentHist) {
        std::vector<GMarker*> markers;
        //if(currentHist) 
        markers = GMarker::GetAll(currentHist);
        double xlow,xhigh;
        if(markers.size()>1) {
          xlow  = markers.at(0)->X();
          xhigh = markers.at(1)->X();
          if(xlow>xhigh) std::swap(xlow,xhigh);
          GMarker::RemoveAll(currentHist);
        } else {
          GetContextMenu()->Action(currentHist->GetXaxis(),currentHist->GetXaxis()->Class()->GetMethodAny("SetRangeUser"));
          xlow = currentHist->GetXaxis()->GetBinCenter(currentHist->GetXaxis()->GetFirst());
          xhigh = currentHist->GetXaxis()->GetBinCenter(currentHist->GetXaxis()->GetLast());
        }  
        gList = GrabHists(gPad->GetCanvas()); 
        TIter iter(gList);
        while(TH1 *hist = ((TH1*)iter.Next())) {
          if(hist->GetDimension()==1) {
            hist->GetXaxis()->SetRangeUser(xlow,xhigh);
            //pad->Modified();
            //pad->Update();
          }
        }
        //gPad->GetCanvas()->Modified(); 
        //gPad->GetCanvas()->Update(); 
      }
      doUpdate = true;
      handled  = true;
      break;
    case kKey_f:
      currentHist=GrabHist();
      if(currentHist) {
        std::vector<GMarker*> markers = GMarker::GetAll(currentHist);
        if(markers.size()>1) {
          if(PhotoPeakFit(currentHist,markers.at(0)->X(),markers.at(1)->X())) {
            doUpdate=true;
            handled=true;
            //GMarker::RemoveAll(currentHist);
          }
        }
      }
      break;
    case kKey_g:
      currentHist=GrabHist();
      if(currentHist) {
        std::vector<GMarker*> markers = GMarker::GetAll(currentHist);
        if(markers.size()>1) {
          if(GausFit(currentHist,markers.at(0)->X(),markers.at(1)->X())) {
            doUpdate=true;
            handled=true;
            //GMarker::RemoveAll(currentHist);
          }
        }
      }
      break;
    case kKey_i:
      currentHist=GrabHist();
      if(currentHist) {
        std::vector<GMarker*> markers = GMarker::GetAll(currentHist);
        if(markers.size()>1) {
          double low  = markers.at(0)->X();
          double high = markers.at(1)->X();
          if(low>high) std::swap(low,high);    
          double sum = currentHist->Integral(currentHist->FindBin(low),
                                       currentHist->FindBin(high));
          printf( BLUE "\n\tSum [%i : %i] = %.01f   (bins)" RESET_COLOR  "\n",currentHist->FindBin(low),currentHist->FindBin(high),sum);
          printf( BLUE "\tSum [%.01f : %.01f] = %.01f" RESET_COLOR  "\n",low,high,sum);
        }
      }
      break;
    case kKey_n:
      currentHist=GrabHist();
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

      }
      doUpdate = true;
      handled  = true;
      break;
    case kKey_N:
     currentHist = GrabHist();
     if(currentHist && currentHist->InheritsFrom(GH1D::Class())) {
      static_cast<GH1D*>(currentHist)->Normalize();
      doUpdate = true;
      handled  = true;
     }
     break;
    case kKey_o:
      //printf("\tkey: %i  %i  %i\n",event,px,py);
      currentHist = GrabHist();
      if(currentHist && currentHist->GetDimension()==1) {    
        currentHist->GetXaxis()->UnZoom();
      } else if(currentHist && currentHist->GetDimension()==2) {  
        ((TH2*)currentHist)->GetXaxis()->UnZoom();
        ((TH2*)currentHist)->GetYaxis()->UnZoom();
      }
      doUpdate = true;
      handled  = true;
      break;
    case kKey_O:
      gList = GrabHists(gPad->GetCanvas());    
      if(gList) {
        TIter iter(gList);
        while(TObject *obj = iter.Next()) {
          if(obj->InheritsFrom(TH1::Class())) {
            currentHist = dynamic_cast<TH1*>(obj);
            currentHist->GetXaxis()->UnZoom();
            if(currentHist->GetDimension()==2)
              currentHist->GetYaxis()->UnZoom();
          }
        }
        TIter citer(gPad->GetCanvas()->GetListOfPrimitives());
        while(TObject* obj = citer.Next()) { 
          if(obj->InheritsFrom(TVirtualPad::Class())) {
            ((TVirtualPad*)obj)->Modified();
            ((TVirtualPad*)obj)->Update();
          }
        }
        handled = true;
      }  
      break;
    case kKey_p:
      currentHist = GrabHist();
      if(currentHist && currentHist->InheritsFrom(GH1D::Class())) {
        GH1D *gcurrentHist = (GH1D*)currentHist;
        if(gcurrentHist->GetParent() && gcurrentHist->GetParent()->InheritsFrom(GH2D::Class())) {
         std::vector<GMarker*> markers = GMarker::GetAll(gcurrentHist);
          if(markers.size()>1) {
            double xlow  = markers.at(0)->X();
            double xhigh = markers.at(1)->X();
            if(xlow>xhigh) std::swap(xlow,xhigh);
            GH1D *proj=0;
				 		if(gcurrentHist->TestBit(GH1D::kProjectionX))
              proj = dynamic_cast<GH2D*>(gcurrentHist->GetParent())->ProjectionY(xlow,xhigh);
            else
              proj = dynamic_cast<GH2D*>(gcurrentHist->GetParent())->ProjectionX(xlow,xhigh);
            GMarker::RemoveAll(gcurrentHist);
            GCanvas *c = new GCanvas;
            proj->Draw();            
          }
        }
      }
      break;
    case kKey_w:
      currentHist = GrabHist();
      if(currentHist && currentHist->InheritsFrom(GH1D::Class())) {
        double xlow = currentHist->GetXaxis()->GetBinLowEdge(currentHist->GetXaxis()->GetFirst());
        double xup  = currentHist->GetXaxis()->GetBinUpEdge(currentHist->GetXaxis()->GetLast());
        currentHist->Rebin(2);
        currentHist->GetXaxis()->SetRangeUser(xlow,xup);
        doUpdate = true;
        handled  = true;
      }
      break;
    case kKey_q:
      currentHist = GrabHist();
      if(currentHist && currentHist->InheritsFrom(GH1D::Class())) {
        GH1D *gcurrentHist = dynamic_cast<GH1D*>(currentHist);
        double xlow = gcurrentHist->GetXaxis()->GetBinLowEdge(gcurrentHist->GetXaxis()->GetFirst());
        double xup  = gcurrentHist->GetXaxis()->GetBinUpEdge(gcurrentHist->GetXaxis()->GetLast());
        gcurrentHist->Unbin(2);
        gcurrentHist->GetXaxis()->SetRangeUser(xlow,xup);
        doUpdate = true;
        handled  = true;
      }
      break;
    case kKey_W:
      gList = GrabHists(gPad->GetCanvas());    
      if(gList) {
        TIter iter(gList);
        while(TObject *obj = iter.Next()) {
          if(obj->InheritsFrom(GH1D::Class())) {
            GH1D *gcurrentHist = dynamic_cast<GH1D*>(obj);
            double xlow = gcurrentHist->GetXaxis()->GetBinLowEdge(gcurrentHist->GetXaxis()->GetFirst());
            double xup  = gcurrentHist->GetXaxis()->GetBinUpEdge(gcurrentHist->GetXaxis()->GetLast());
            gcurrentHist->Rebin(2);
            gcurrentHist->GetXaxis()->SetRangeUser(xlow,xup);
          }
        }
        doUpdate = true;
        handled  = true;
      }  
      break;
    case kKey_Q:
      gList = GrabHists(gPad->GetCanvas());    
      if(gList) {
        TIter iter(gList);
        while(TObject *obj = iter.Next()) {
          if(obj->InheritsFrom(GH1D::Class())) {
            GH1D *gcurrentHist = dynamic_cast<GH1D*>(obj);
            double xlow = gcurrentHist->GetXaxis()->GetBinLowEdge(gcurrentHist->GetXaxis()->GetFirst());
            double xup  = gcurrentHist->GetXaxis()->GetBinUpEdge(gcurrentHist->GetXaxis()->GetLast());
            gcurrentHist->Unbin(2);
            gcurrentHist->GetXaxis()->SetRangeUser(xlow,xup);
          }
        }
        doUpdate = true;
        handled = true;
      }  
      break;
    case kKey_l:
    case kKey_z:
      currentHist = GrabHist();
      if(currentHist && currentHist->GetDimension()==1) {
        if(GetLogy()){
          // Show full y range, not restricted to positive values.
          currentHist->GetYaxis()->UnZoom();
          SetLogy(0);
        } else {
          // Only show plot from 0 up when in log scale.
          if(gPad->GetUymin()<0) {
            currentHist->GetYaxis()->SetRangeUser(0,gPad->GetUymax());
          }
          SetLogy(1);
        }
      } else if(currentHist && currentHist->GetDimension()==2) {
        if(GetLogz()){
          // Show full y range, not restricted to positive values.
          currentHist->GetZaxis()->UnZoom();
          SetLogz(0);
        } else {
          // Only show plot from 0 up when in log scale.
          if(currentHist->GetMinimum()<0) {
            currentHist->GetZaxis()->SetRangeUser(0,currentHist->GetMaximum());
          }
          SetLogz(1);
        }
      }
      doUpdate = true;
      handled  = true;
      break;
    case kKey_x:
      currentHist = GrabHist();
      if(currentHist && currentHist->GetDimension()==2) {
        if(currentHist->InheritsFrom(GH2D::Class())) {
          GCanvas * c = new GCanvas;
          // if not, make it...
          dynamic_cast<GH2D*>(currentHist)->ProjectionX()->Draw();
        }
      }
      break;
    case kKey_y:
      currentHist = GrabHist();
      if(currentHist && currentHist->GetDimension()==2) {
        if(currentHist->InheritsFrom(GH2D::Class())) {
          GCanvas * c = new GCanvas;
          // if not, make it...
          dynamic_cast<GH2D*>(currentHist)->ProjectionY()->Draw();
        }
      }
      break;
    default:
      break;


  }
  if(gPad && doUpdate) {
    UpdateAllPads();//gPad);
    //gPad->Modified();
    //gPad->Update();
  }
  return handled;
}


TVirtualPad* GCanvas::GetSelectedPad() const {
  TVirtualPad *sPad = TCanvas::GetSelectedPad();
  //printf("gPad = 0x%p\n",gPad);
  //printf("sPad = 0x%p\n",sPad);
  return sPad;
}


void GCanvas::UpdateAllPads() {
  //TIter iter(this->GetListOfPrimitives());
  //while(TObject *obj = iter()) printf("\t\t%s\n",obj->GetName());  
  TIter iter1(this->GetListOfPrimitives());
  while(TObject *obj1 = iter1()) {
    if(!obj1->InheritsFrom(TVirtualPad::Class())) continue;
    TVirtualPad *pad1 = dynamic_cast<TVirtualPad*>(obj1);
    TIter iter2(pad1->GetListOfPrimitives());
    while(TObject *obj2 = iter2()) {
      if(!obj2->InheritsFrom(TVirtualPad::Class())) continue;
      TVirtualPad *pad2 = dynamic_cast<TVirtualPad*>(obj2);
      pad2->Modified();
      pad2->Update();
    }
    pad1->Modified();
    pad1->Update();
  }
  this->Modified();
  this->Update();
}





/*
void GCanvas::doSelected(TVirtualPad *pad, TObject *obj, Int_t event) {
  printf("--------------\n");
  printf("gPad    = 0x%p\n",gPad);
  printf("pad     = 0x%p\n",pad);
  printf("obj     = 0x%p\n",obj);
  if(obj) 
  printf("name    = %s\n",obj->GetName());
  if(obj->InheritsFrom(TAxis::Class())) 
  printf("parent  = %s\n",((TAxis*)obj)->GetParent()->GetName());
  printf("event   = %i\n",event);
  printf("\n\n");
}
*/
