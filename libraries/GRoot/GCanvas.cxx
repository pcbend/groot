
#include <globals.h>

#include <GCanvas.h>

GCanvas::GCanvas(bool build) : 
  TCanvas(build) { }

GCanvas::GCanvas(const char* name, const char* title, int wtopx, int wtopy, int ww, int wh) :
  TCanvas(name,title,wtopx,wtopy,ww,wh) { }

GCanvas::GCanvas(const char* name, const char* title, int ww, int wh) : 
  TCanvas(name,title,ww,wh) { }

GCanvas::GCanvas(const char* name, const char* title,int form) :
  TCanvas(name,title,form) { }

GCanvas::GCanvas(const char* name, int ww, int wh, int winid) :
  TCanvas(name,ww,wh,winid) { }

GCanvas::~GCanvas() { }  


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

  printf(RED "handled = %i" RESET_COLOR  "\n",handled);


//  if(!handled) 
//    TCanvas::HandleInput(event,px,py);

}





