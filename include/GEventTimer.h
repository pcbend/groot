#ifndef __GEVENTTIMER_H__
#define __GEVENTTIMER_H__

#include <cstdio>

#include <TTimer.h>

#include <GGlobals.h>
#include <Histomatic.h>

class TList;

TList *GetSignals(const char *classname="TCanvas");

class GEventTimer : public TTimer { 
  public:
    GEventTimer() : TTimer(100) {     }

    bool Notify() override; 

  private:

  ClassDefOverride(GEventTimer,0)
};


#endif
