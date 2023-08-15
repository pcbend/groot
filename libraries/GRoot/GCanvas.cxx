

#include <GCanvas.h>

#include <string>

#include <globals.h>
#include <GCommands.h>
#include <GMarker.h>

#include <KeySymbols.h>
#include <TH1.h>
#include <TH2.h>
#include <TClass.h>
#include <TFrame.h>

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

}

void GCanvas::HandleInput(EEventType event, int px, int py) {
  bool handled = false;

  //check the event,
  switch(event) {
    case kKeyPress:
      handled = HandleKeyPress(event,px,py);
      break;
    case kArrowKeyPress:
    case kArrowKeyRelease:
      //bool = HandleArrow();
      break;
    case kButton1Down:
    case kButton1Shift:
    case kButton1Up:
    case kButton1Double:
      printf("clicked on %s\n",GetSelected()->IsA()->GetName());
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
  TH1 *gHist = 0;
  switch(event) {
    case kButton1Down:
      gHist = GrabHist();
      if(gHist && gPad) {
        GMarker *m = new GMarker();
        printf(RED "adding marker\n" RESET_COLOR "\n"); fflush(stdout);
        double xx = gPad->AbsPixeltoX(px);
        double x  = gPad->PadtoX(xx);
        //int  binx = h->GetXaxis()->FindBin(x);
        m->AddTo(gHist,x);
        m->Update();
        handled = true;
        
        gPad->Modified();
        gPad->Update();
      }
      break;
    case kButton1Shift:
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

bool GCanvas::HandleKeyPress(EEventType event, int px, int py) {
  //printf("key: %i  %i  %i\n",event,px,py);
  bool handled = false;
  TH1 *gHist = 0;
  switch(py) {
    case kKey_o:
      //printf("\tkey: %i  %i  %i\n",event,px,py);
      gHist = GrabHist();
      if(gHist && gHist->GetDimension()==1) {    
        gHist->GetXaxis()->UnZoom();
      } else if(gHist && gHist->GetDimension()==2) {  
        ((TH2*)gHist)->GetXaxis()->UnZoom();
        ((TH2*)gHist)->GetYaxis()->UnZoom();
      }
      if(gPad) { 
        gPad->Modified();
        gPad->Update();
      }
      handled = true;
    default:
      break;
  }
  return handled;
}



