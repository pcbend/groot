
#include <string>

#include <globals.h>
#include <GCanvas.h>

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
    case kArrowKeyPress:
    case kArrowKeyRelease:
    case kButton1Motion:
    case kButton1ShiftMotion: 
      handled = true;
      break;
    default:
      break;
  }

  //printf(RED "handled = %i" RESET_COLOR  "\n",handled);


//  if(!handled) 
//    TCanvas::HandleInput(event,px,py);

}





