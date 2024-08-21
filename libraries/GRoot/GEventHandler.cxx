
#include "GEventHandler.h"

#include <TH1.h>
#include <TH2.h>

GH1DEventHandler::GH1DEventHandler(TH1* hist) : GEventHandler(static_cast<TObject*>(hist)) { } 

GH1DEventHandler::~GH1DEventHandler() { }

void GH1DEventHandler::EventProcessed(Event_t *event) { }
void GH1DEventHandler::HandleInput(EEventType event, Int_t px, Int_t py) { }

/********/
/********/
/********/
/********/

GH2DEventHandler::GH2DEventHandler(TH2* hist) : GEventHandler(static_cast<TObject*>(hist)) { } 

GH2DEventHandler::~GH2DEventHandler() { }

void GH2DEventHandler::EventProcessed(Event_t *event) { }
void GH2DEventHandler::HandleInput(EEventType event, Int_t px, Int_t py) { }
