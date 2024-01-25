#ifndef __GCOMMANDS_H__
#define __GCOMMANDS_H__

class TVirtualPad;
class TObject;
class TList;
class TH1;
class TF1;
class GGaus;
class GPeak;

typedef const char Option_t;

GPeak *PhotoPeakFit(TH1*,double,double,Option_t *opt="");
GGaus *GausFit(TH1*,double,double,Option_t *opt="");

TH1   *GrabHist(int i=0);           //return the ith histogram from the current canvas.
TList *GrabHists(TVirtualPad *p=0); //return all histograms on a canvas or pad. (default is the gPad);
TF1   *GrabFit(int i=0);            //return the ith fit from the current canvas.
 
double GetChi2(TObject*,TF1*);



#endif
