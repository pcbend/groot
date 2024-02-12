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
    bool HandleArrowPress(EEventType event, int px, int py,int mask);
 
    //void ProcessedEvent(int event, int x, int y, TObject *selected) override;
    void EventProcessed(Event_t *event);

    void UpdateAllPads();

    void SetLockPads(bool flag=true) { fLockPads = flag; }
    bool GetLockPads() const { return fLockPads; }



  //private:
    void Init(const char* name="",const char* title="");
    TVirtualPad* GetSelectedPad() const override;

    //void doSelected(TVirtualPad *pad, TObject *obj, Int_t event);

  private:
    EEventType fEvent;
    int        fEventX;
    int        fEventy;

    bool fLockPads;

    static int fCanvasNumber;


  ClassDefOverride(GCanvas,0)
};

#endif
