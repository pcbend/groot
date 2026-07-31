

#include <GCanvas.h>

#include <string>
#include <regex>
#include <cmath>

#include <globals.h>
#include <GCommands.h>
#include <GMarker.h>
#include <GROI.h>
#include <GH1D.h>
#include <GH2D.h>


#include <KeySymbols.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TClass.h>
#include <TCutG.h>
#include <TFrame.h>
#include <TPad.h>
#include <TAxis.h>

#include <TContextMenu.h>
#include <TColor.h>
#include <TEnv.h>
#include <TGFrame.h>
#include <TRootCanvas.h>
#include <TGMenu.h>
#include <TGStatusBar.h>
#include <TVirtualX.h>

#include<TROOT.h>
#include <cstring>


int GCanvas::fCanvasNumber = 0;

Event_t GCanvas::fCurrentEvent;

namespace {

void ApplyRequestedCurrentPad(TVirtualPad* requestedPad) {
  if(!requestedPad)
    return;

  requestedPad->cd();
  gROOT->SetSelectedPad(requestedPad);

  TCanvas* canvas = requestedPad->GetCanvas();
  auto* selectedPad = dynamic_cast<TPad*>(requestedPad);
  if(canvas && selectedPad)
    canvas->SetSelectedPad(selectedPad);
}

Pixel_t GetPixelFromEnv(const char *key, const char *fallback) {
  const char *value = gEnv->GetValue(key, fallback);
  if(!value || !*value) {
    value = fallback;
  }

  if(!strcmp(value, "black")) {
    return TGFrame::GetBlackPixel();
  }
  if(!strcmp(value, "white")) {
    return TGFrame::GetWhitePixel();
  }

  Color_t color = value[0] == '#' ? TColor::GetColor(value)
                                  : TColor::GetColorByName(value);
  if(color < 0) {
    color = fallback && fallback[0] == '#'
          ? TColor::GetColor(fallback)
          : TColor::GetColorByName(fallback);
  }

  return color >= 0 ? TColor::Number2Pixel(color) : TGFrame::GetBlackPixel();
}

void ApplyRootCanvasTheme(TCanvas *canvas) {
  if(!canvas) {
    return;
  }

  auto *rootCanvas = dynamic_cast<TRootCanvas*>(canvas->GetCanvasImp());
  if(!rootCanvas) {
    return;
  }

  const Pixel_t background = GetPixelFromEnv("Gui.BackgroundColor", "#e0e0e0");
  const Pixel_t foreground = GetPixelFromEnv("Gui.ForegroundColor", "black");

  if(auto *menuBar = rootCanvas->GetMenuBar()) {
    menuBar->SetBackgroundColor(background);
    TIter next(menuBar->GetTitles());
    while(auto *obj = next()) {
      auto *title = dynamic_cast<TGMenuTitle*>(obj);
      if(!title) {
        continue;
      }
      title->SetBackgroundColor(background);
      title->SetTextColor(foreground);
      gClient->NeedRedraw(title);
    }
    gClient->NeedRedraw(menuBar);
  }

  if(auto *statusBar = rootCanvas->GetStatusBar()) {
    statusBar->SetBackgroundColor(background);
    statusBar->SetForegroundColor(foreground);
  }
}

void DisconnectCanvasEvents(GCanvas *canvas) {
  if(!canvas) {
    return;
  }

  TQObject::Disconnect("TGClient", "ProcessedEvent(Event_t *,Window_t)", canvas,
                       "EventProcessed(Event_t*)");
  TQObject::Disconnect("TGClient", "ProcessedEvent(Event_t*)", canvas,
                       "EventProcessed(Event_t*)");

  auto *imp = canvas->GetCanvasImp();
  if(imp && imp->IsA() == TRootCanvas::Class()) {
    static_cast<TRootCanvas*>(imp)->Disconnect("ProcessedEvent(Event_t*)",
                                               canvas,
                                               "EventProcessed(Event_t*)");
  }
}

}

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

GCanvas::~GCanvas() {
  DisconnectCanvasEvents(this);
}
  


void GCanvas::Close(Option_t *opt) {
  DisconnectCanvasEvents(this);
  TCanvas::Close(opt);
}

void GCanvas::Init(const char* name, const char* title) {
  std::string temp   = Form("canvas_%i",fCanvasNumber++); 
  std::string sname  = name;  
  std::string stitle = title;

  fLockPads = false;
  fManagedCursorActive = false;
  fManagedCursor = kPointer;

  if(!sname.length())  this->SetName(temp.c_str());
  if(!stitle.length()) this->SetTitle(temp.c_str());

  this->AddExec("groot_interact","GRootInteract()");
  ApplyRootCanvasTheme(this);


  gClient->Connect("ProcessedEvent(Event_t *,Window_t)","GCanvas",this,"EventProcessed(Event_t*)");
  //GetCanvasImp()->Connect("ProcessedEvent(Event_t *,Window_t)","GCanvas",this,"EventProcessed(Event_t*)");
  //if(GetCanvasImp()->IsA() == TRootCanvas::Class())
  //  static_cast<TRootCanvas*>(GetCanvasImp())->Connect("ProcessedEvent(Event_t *,Window_t)","GCanvas",this,
  //                                                     "EventProcessed(Event_t*)");
}


TCanvas *GCanvas::MakeDefCanvas() {

  TCanvas *c =  new GCanvas();
  ::Info("GCanvas::MakeDefCanvas"," created default GCanvas with name %s", c->GetName());
  return c;
}

/*
  //  Event Processing Order:
    0 GetSelected() 
      - Check that the class is one I want to deal with, otherwise pass back to base root?


    1 Event Processed  || this goes to all canvases(?) 
      - Finds Arrow Keys, otherwise does nothing.
        - HandleArrowPress
    
    2 HandleInput
      // look for histograms - check that GetSelected() is Inherits from TH1 or TFrame, then check dimensions.
      - switch yard to
        - HandleKeyPress
        - HandleMouseButton1

        -- this needs to reworked.  The Canvas(Pad) should just pass the info to the object(s)...
           we run into an issue for traditional root objects,
           perhaps we can fix this with a TExec?


*/

void GCanvas::EventProcessed(Event_t *event) {
//ProcessedEvent(int event, int x, int y, TObject *selected) {
  
 /* 
  printf("\n\n\n");
  printf("-------------\n");
  printf("Event Processed\n");
  printf("\tevent  = %i\n",event->fCode);
  printf("\tevent  = 0x%08x\n",event->fCode);
  printf("\tstate  = 0x%08x\n",event->fState);  // contains modifier information...
  printf("\ttype   = 0x%08x\n",event->fType);   // 0 down, 1 up
  printf("\twindow = %lu\n",event->fWindow);
  printf("\tx      = 0x%08x\n",event->fX);
  printf("\ty      = 0x%08x\n",event->fY);
  printf("\n");
  */

  if(!event) {
    return;
  }

  auto *imp = GetCanvasImp();
  if(!imp) {
    return;
  }

  fCurrentEvent = *event;

  unsigned int keysym;
  char str[2];
  gVirtualX->LookupString(event, str, sizeof(str), keysym);

  //printf("keysym = %i\t gPad = %p GrabHist() = %p GrabHist()->GetName() = %s\n",
  //  keysym,gPad,GrabHist(),GrabHist() ? GrabHist()->GetName() : "");

  /*
  if(keysym==kKey_Space) {
    printf("space bar pressed!\n");
    printf("\twindow = %lu\n",event->fWindow);
    printf("this:  %p\n",this);
    printf("gPad:  %p\n",gPad);
  }
  */

  if(!event->fWindow) return;
  if(!this->GetCanvasID()) return;
  if(!gVirtualX->GetWindowID(this->GetCanvasID())) return;

  if(static_cast<unsigned long>(event->fWindow) != 
     static_cast<unsigned long>(gVirtualX->GetWindowID(this->GetCanvasID()))) 
    return;
  //switch(event->fCode) {


  if(event->fType!=0) return; // only look for key down...

  switch(keysym) {
    case kKey_Left:
    case kKey_Up:
    case kKey_Right:
    case kKey_Down:
      //printf("arrow key pressed.\n");
      //printf("keysym = 0x%08x\n",keysym);
      //TH1 *currentHist = GrabHist();
      //if(currentHist && currentHist->GetDimension()==1) 
      HandleArrowPress(kArrowKeyPress,keysym,keysym,event->fState);
      //else if(currentHist && currentHist->GetDimension()==2)
      //  HandleArrowPress_2d(kArrowKeyPress,keysym,keysym,event->fState);
      //HandleArrowPress_1d(kArrowKeyPress,keysym,keysym,event->fState);
      break;
    defualt:
      break;
  }
  return;
}

void GCanvas::HandleInput(EEventType event, int px, int py) {
  bool handled = false;

  if(event==kKeyPress && px==kKey_Space) {
    if(GetSelectedPad()) {
      if(GetSelectedPad() != gPad) {
        GetSelectedPad()->cd();
        UpdateAllPads();
      }
    }
  }



  if(!GetSelected()) {
    TCanvas::HandleInput(event,px,py);
    ApplyRequestedCurrentPad(TakeRequestedCurrentPad());
    UpdateCursorForSelected(event);
    return;
  }

  
  //printf("GetSelected()->IsA()->GetName() = %s\n",GetSelected()->IsA()->GetName());
  //if(GetSelected()->IsA()->Class() == TFrame::Class())
    //printf("GetSelected's Pad is %p\n",GetSelectedPad());





  if( GetSelected()->InheritsFrom(TH1::Class()) ||
      GetSelected()->InheritsFrom(TFrame::Class()) ) {
    //return TCanvas::HandleInput(event,px,py);

  TH1 *currentHist = GrabHist();
  if(!currentHist) {
    TCanvas::HandleInput(event,px,py);
    ApplyRequestedCurrentPad(TakeRequestedCurrentPad());
    UpdateCursorForSelected(event);
    return;
  }

  //check the event,
  switch(event) {
    case kKeyPress:
      //if(currentHist->GetDimension()==1) 
      //  handled = HandleKeyPress_1d(event,px,py);
      //else if(currentHist->GetDimension()==2)
      //  handled = HandleKeyPress_2d(event,px,py);
      break;
    case kButton1Shift:
    case kButton1Down:
    case kButton1Up:
    case kButton1Double:
      //printf("clicked on %s\n",GetSelected()->IsA()->GetName());
      //if(!GetSelected()) break;
      //if(GetSelected()->InheritsFrom(TH1::Class()) ||
      //   GetSelected()->InheritsFrom(TFrame::Class()) )
      //if(currentHist->GetDimension()==1) 
      //  handled = HandleMouseButton1(event,px,py);
      //else if(currentHist->GetDimension()==2)
      //  handled = HandleMouseButton1_2d(event,px,py);
      break;
    case kButton1Motion:
    case kButton1ShiftMotion: 
      //handled = true;
      break;
    default:
      break;
  }
  //printf(RED "handled = %i" RESET_COLOR  "\n",handled);
  }
  if(!handled) 
    TCanvas::HandleInput(event,px,py);
  else 
    UpdateAllPads();

  ApplyRequestedCurrentPad(TakeRequestedCurrentPad());
  UpdateCursorForSelected(event);
}

ECursor GCanvas::CursorForSelected(TObject *selected) const {
  if(!selected)
    return kPointer;

  // Keep all groot-specific cursor policy here, after ROOT has picked the
  // object under the mouse.  This avoids making individual objects manage the
  // global canvas cursor, and gives us one expandable place to add future
  // interactive objects.
  if(selected->InheritsFrom(GMarker::Class()))
    return kHand;

  if(selected->InheritsFrom(TCutG::Class()))
    return kHand;

  return kPointer;
}

void GCanvas::UpdateCursorForSelected(EEventType event) {
  // ROOT already sets a number of useful cursors while picking and executing
  // events.  Only override the cursor for the interactions groot explicitly
  // owns, and only reset the cursor if the previous cursor was ours.
  if(event != kMouseMotion && event != kMouseEnter && event != kMouseLeave)
    return;

  const ECursor cursor = CursorForSelected(GetSelected());
  if(cursor != kPointer) {
    if(fManagedCursorActive && fManagedCursor == cursor)
      return;
    SetCursor(cursor);
    fManagedCursor = cursor;
    fManagedCursorActive = true;
    return;
  }

  if(fManagedCursorActive) {
    SetCursor(kPointer);
    fManagedCursor = kPointer;
    fManagedCursorActive = false;
  }
}



/*
bool GCanvas::HandleArrowPress_2d(EEventType event, int px, int py, int mask) { 
  bool handled = false;
  TH1 *currentHist = 0;
  switch(event) {
    default:
      break;
  }
  return handled;
}
*/

bool GCanvas::HandleArrowPress(EEventType event, int px, int py,int mask) {
  bool handled  = false;
  bool doUpdate = false;
  TH1   *currentHist = GrabHist();
  TList *gList = 0;
  //if(!GetSelected()) return handled; 

  //if(GetSelected()->IsA() == TFrame::Class()) {
  //  printf("Selected: %s\n",GetSelected()->IsA()->GetName());
  //} else if(1==0) {
  //
  // } else {

  //}

  //printf("Selected: %s\n",GetSelected()->IsA()->GetName());
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
        if((mask&kKeyShiftMask) || fLockPads )  {
          TIter iter(gList);
          while(TH1 *hist = dynamic_cast<TH1*>(iter())) 
            hist->GetXaxis()->SetRangeUser(gPad->GetUxmin()-halfWindow,gPad->GetUxmax()-halfWindow);
        } else {
          currentHist->GetXaxis()->SetRangeUser(gPad->GetUxmin()-halfWindow,gPad->GetUxmax()-halfWindow);
        }
        //gPad->Modified();
        doUpdate = true;
      } 
      break;
    case kKey_Up:
      if(currentHist && currentHist->InheritsFrom(GH1D::Class())) {
        GH1D *gcurrentHist = dynamic_cast<GH1D*>(currentHist);
        if(gcurrentHist->GetParent()) {
          TH2* p = gcurrentHist->GetParent();
          if(p && p->InheritsFrom(GH2D::Class())) {
            GH1D* hnext = dynamic_cast<GH2D*>(p)->Next(gcurrentHist);
            if(hnext) {
              hnext->Draw();
              //gPad->Modified();
              doUpdate = true;
            }
          }
        }
      }
      break;
    case kKey_Right:
      if(currentHist && currentHist->GetDimension()==1) {
        double halfWindow = fabs(0.5*(gPad->GetUxmax() - gPad->GetUxmin()));
        if((gPad->GetUxmax() + halfWindow)>currentHist->GetXaxis()->GetXmax()) 
          halfWindow = fabs(currentHist->GetXaxis()->GetXmax() - gPad->GetUxmax());
        if((mask&kKeyShiftMask) || fLockPads ) {
          TIter iter(gList);
          while(TH1 *hist = dynamic_cast<TH1*>(iter())) 
            hist->GetXaxis()->SetRangeUser(gPad->GetUxmin()+halfWindow,gPad->GetUxmax()+halfWindow);
        } else {
          currentHist->GetXaxis()->SetRangeUser(gPad->GetUxmin()+halfWindow,gPad->GetUxmax()+halfWindow);
        }
        //gPad->Modified();
        doUpdate = true;
      } 
      break;
    case kKey_Down:
      if(currentHist && currentHist->InheritsFrom(GH1D::Class())) {
        GH1D *gcurrentHist = dynamic_cast<GH1D*>(currentHist);
        if(gcurrentHist->GetParent()) {
          TH2* p = gcurrentHist->GetParent();
          if(p && p->InheritsFrom(GH2D::Class())) {
            GH1D* hprev = dynamic_cast<GH2D*>(p)->Previous(gcurrentHist);
            if(hprev) {
              hprev->Draw();
              //gPad->Modified();
              doUpdate = true;
            }
          }
        }
      }
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



TVirtualPad* GCanvas::GetSelectedPad() const {
  TVirtualPad *sPad = TCanvas::GetSelectedPad();
  //printf("gPad = 0x%p\n",gPad);
  //printf("sPad = 0x%p\n",sPad);
  return sPad;
}


void GCanvas::UpdateAllPads() {
  //TIter iter(this->GetListOfPrimitives());
  //while(TObject *obj = iter()) printf("\t\t%s\n",obj->GetName());  
  /*
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
  */
  //this->Modified();
  TIter iter(gROOT->GetListOfCanvases());
  while(TCanvas *c = (TCanvas*)iter.Next()) {
    c->Modified();
    c->Update();
  }
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


void GCanvas::Divide(int nx,int ny,float xmargin,float ymargin,int color) {
  //printf("GCanvas Divide called.\n");
  TCanvas::Divide(nx,ny,xmargin,ymargin,color);

  TIter iter(this->GetListOfPrimitives());
  while(TObject *obj = iter.Next())
    if(obj->InheritsFrom(TPad::Class()))
      static_cast<TPad*>(obj)->AddExec("groot_interact","GRootInteract()");

}
