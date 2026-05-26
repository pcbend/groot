#ifndef __GCOMMANDS_H__
#define __GCOMMANDS_H__

class TVirtualPad;
class TObject;
class TList;
class TH1;
class TH1D;
class TF1;
class TGraph;
class GGaus;
class GPeak;

typedef const char Option_t;

struct GInteractionInfo {
  TVirtualPad* pad = nullptr;
  TObject* selected = nullptr;
  TObject* target = nullptr;

  int event = 0;
  int px = 0;
  int py = 0;
  double x = 0;
  double y = 0;
  double counts = 0;

  bool modified = false;
  bool IsValid() const {
    return pad && target;
  }
};


GPeak *PhotoPeakFit(TH1*,double,double,Option_t *opt="");
GGaus *GausFit(TH1*,double,double,Option_t *opt="");

TH1D  *ResidualHist(TH1* hist, TF1* fit=nullptr);
void   DrawResiduals(TH1* hist, TF1* fit=nullptr,bool normalized=true);

TH1   *GrabHist(int i=0);           //return the ith histogram from the current canvas.
TList *GrabHists(TVirtualPad *p=0); //return all histograms on a canvas or pad. (default is the gPad);
TF1   *GrabFit(int i=0);            //return the ith fit from the current canvas.

TObject *GrabPlottable(int i=0);    //return the ith plottable (th1 or tgraph) thing.

void ls(int n=0); 

void SaveAllCuts(TH1*,const char* fname="output.cuts",Option_t *opt="recreate");

double GetChi2(TObject*,TF1*);

void GRootInteract();  //void so it doesn't repeatedly print the return to the terminal.
GInteractionInfo BuildInteractionInfo();
bool DispatchInteraction(GInteractionInfo &info);
const GInteractionInfo &GetLastInteractionInfo();


bool GRootInteractHist(TH1* current,GInteractionInfo &info);
bool GRootInteractHistMouseButton(TH1* current,GInteractionInfo &info);
bool GRootInteractHistKeyPress(TH1* current,GInteractionInfo &info);
bool GRootInteractGraph(TGraph* current,GInteractionInfo &info);
bool GRootInteractGraphMouseButton(TGraph* current,GInteractionInfo &info);
bool GRootInteractGraphKeyPress(TGraph* current,GInteractionInfo &info);


#endif
