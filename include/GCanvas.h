#ifndef __GCANVAS_H__
#define __GCANVAS_H__

#include <TCanvas.h>

class GCanvas : public TCanvas {
  public:
    GCanvas(bool build=true);
    GCanvas(const char* name, const char* title, int wtopx, int wtopy, int ww, int wh);
    GCanvas(const char* name, const char* title, int ww, int wh);
    GCanvas(const char* name, const char* title="",int form=1);
    GCanvas(const char* name, int ww, int wh, int winid);
    virtual ~GCanvas(); 

    void HandleInput(EEventType event, int px, int py) override; 
    bool HandleMouseButton1(EEventType event, int px, int py);
    bool HandleKeyPress(EEventType event, int px, int py);

  //private:
    void Init(const char* name="",const char* title="");

  private:
    EEventType fEvent;
    int        fEventX;
    int        fEventy;

    static int fCanvasNumber;


  ClassDefOverride(GCanvas,0)
};

#endif
