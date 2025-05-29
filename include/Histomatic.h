#ifndef __HISTOMATIC_H__
#define __HISTOMATIC_H__

//#include "TQObject.h"
//#include "RQ_OBJECT.h"
#include "TGClient.h"
#include "TGFrame.h"
#include "TGTab.h"
#include "TGMenu.h"
#include "TGLayout.h"
#include "TGSplitter.h"
#include "TGStatusBar.h"
#include "TGButton.h"
#include "TGStatusBar.h"
#include "TGComboBox.h"

#include "TFile.h"
#include "TList.h"

#include <GListTree.h>

#include<exception>
#include<iostream>
#include<map>

//class Histomatic;
class GEventTimer;
class TH1;
class TCanvas;
class TPad;



class Histomatic : public TGMainFrame { //: public TQObject { //: public TGMainFrame {
  //RQ_OBJECT("Histomatic");
  //Histomatic(const Histomatic&) = delete;
  //Histomatic& operator=(const Histomatic&) = delete;

  private:
    static Histomatic* fInstance;
    Histomatic();
  public:
    static Histomatic* Get();
    virtual ~Histomatic();    

    //TGListTree *GetListTree() { return fGListTree; }
    GListTree *GetListTree() const { return fGListTree; }

    void buttonAction();
    void SetStatusText(std::string text, int col) { fStatusBar->SetText(text.c_str(),col); }

  //private:
    void doUpdate() const;
    //void doDraw(TObject*,Option_t *opt="");
    //void doDraw(TList*,Option_t *opt="");
    void doDraw();
    void doDraw(std::vector<TGListTreeItem*> selected,Option_t *opt="") const;
    void drawHists(std::vector<TH1*> hists, TCanvas *g=0) const; 

    void closeAllCanvases();

    void doLockPads(TPad *pad=0);


    //bool HandleKey(Event_t *event)       override { printf("HandleKey\n"); return false; }
    //bool HandleSelection(Event_t *event) override { printf("HandleSelection\n"); return false; }
    //bool HandleSelectionRequest(Event_t *event) override { printf("HandleSelectioniRequest\n"); return false; }
    //bool HandleEvent(Event_t *event) override { printf("Event\n"); return false; }



  protected:
    TGLayoutHints *fLH0, *fLH1, *fLH2;
    
    TGVerticalFrame *fVf;
    TGVerticalFrame *fButtonContainer;
    TGHorizontalFrame *fButtonRow1;
    TGHorizontalFrame *fButtonRow2;

    //TGMenuBar *fMenuBar;
    //TGHorizontalFrame *fTopMenuFrame;               
    //TGHorizontalFrame *fPreMenuFrame; 

    //TGPopupMenu       *fMenuFile;
    //TGPopupMenu       *fMenuHelp;     

    TGTextButton      *fButton1; 
    TGTextButton      *fButton2; 
    TGTextButton      *fButton3; 
    TGTextButton      *fButton4; 
    
    TGTextButton      *fButton5; 
    TGTextButton      *fButton6; 
    TGTextButton      *fButton7; 
    TGTextButton      *fButton8; 

    TGHorizontalFrame *fDrawOptionContainer;
    TGButtonGroup     *fDrawOptionGroup;

    TGComboBox        *fDrawComboBox;
    TGCheckButton     *fDrawNormalized;
    TGCheckButton     *fDrawColz;
    TGCheckButton     *fLockPads;


    //TGCanvas          *fListTreeCanvas;
    //TGListTree        *fListTree;
    GListTreeCanvas    *fGListTreeCanvas;
    GListTree          *fGListTree;
    
    TGStatusBar        *fStatusBar;

  public:
    enum EDrawOption {
      eDrawNew,
      eDrawSame,
      eDrawStacked
    };
    enum EHistMessages {
      kThing1,
      kThing2,
      kSendHelp,
      kCloseWindow
    };


    int GetDrawOption() { return (EDrawOption)fDrawComboBox->GetSelected(); }


    void CreateWindow();
    void CloseWindow() override;

    void Show(int width=350,int height=780);

    void AddRootFile(TFile *file) { fGListTree->InsertObject(file); }
    //TList *GetAllActive();

    TGStatusBar *GetStatusBar() { return fStatusBar; } 

  private:
    //TGMainFrame *fMainWindow;
    //TGVerticalFrame   *fVf;

    bool fDrawNew;

    TList *fTrash;
    //std::map<std::string, TObject*> fObjReadMap;
    GEventTimer *fEventTimer;

  ClassDefOverride(Histomatic,0)
  //ClassDef(Histomatic,0)
};


#endif



