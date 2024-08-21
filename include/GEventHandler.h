#ifndef __GEVENTHANDLER_H__
#define __GEVENTHANDLER_H__


/*  
  //  Event Processing Order:
    1 Event Processed 
      - Finds Arrow Keys, otherwise does nothing.                                                                       
        - HandleArrowPress
    2 HandleInput
      // look for histograms - check that GetSelected() is Inherits from TH1 or TFrame, then check dimensions.
      - switch yard to
        - HandleKeyPress
        - HandleMouseButton1
    
*/  

#include <TObject.h>
#include <GuiTypes.h>  //Event_t
#include <Buttons.h>    //EEventType

class TH1;
class TH2;

class GEventHandler : public TObject {


  public:
    GEventHandler(TObject *obj) : fObject(obj) { }
    virtual ~GEventHandler() { } 

    virtual void EventProcessed(Event_t *event) = 0; 
    virtual void HandleInput(EEventType event, Int_t px, Int_t py) = 0;

  private: 
    TObject * fObject;

  ClassDef(GEventHandler, 0)

};


class GH1DEventHandler : public GEventHandler {

  public:
    GH1DEventHandler(TH1 *h);
    virtual ~GH1DEventHandler();

    virtual void EventProcessed(Event_t *event);
    virtual void HandleInput(EEventType event, Int_t px, Int_t py);

  ClassDef(GH1DEventHandler, 0)

};


class GH2DEventHandler : public GEventHandler {

  public:
    GH2DEventHandler(TH2 *h);
    virtual ~GH2DEventHandler();

    virtual void EventProcessed(Event_t *event);
    virtual void HandleInput(EEventType event, Int_t px, Int_t py);

  ClassDef(GH2DEventHandler, 0)

};






#endif
