#ifndef __HISTOMATIC_H__
#define __HISTOMATIC_H__


#include "TGClient.h"
#include "TGFrame.h"
#include "TGTab.h"
#include "TGMenu.h"
#include "TGLayout.h"
#include "TGSplitter.h"
#include "TGStatusBar.h"
#include "TGButton.h"
#include "TGListTree.h"

#include "TFile.h"
#include "TKey.h"
#include "TList.h"

#include<exception>
#include<iostream>

class Histomatic;

extern Histomatic *gHistomatic;

class GListTreeCanvas : public TGCanvas {
  public:
    GListTreeCanvas(const TGWindow *p=nullptr, UInt_t w=10, UInt_t h=10, UInt_t options=kSunkenFrame|kDoubleBorder, Pixel_t back=GetDefaultFrameBackground());
    ~GListTreeCanvas();

    bool HandleButton(Event_t *event);

  private:

  ClassDef(GListTreeCanvas,0)
};


class GListTree : public TGListTree {
  public: 
    //GListTree(TGWindow *parent=0,UInt_t w=1,UInt_t h=1, UInt_t options=kSunkenFrame|kDoubleBorder,Pixel_t back=GetDefaultFrameBackground());
    GListTree(TGCanvas *parent=0,Histomatic *hist=0);
    ~GListTree();

  TList *GetAllActive(TGListTreeItem *item=0);  
  void ClearActive();

  void InsertObject(TObject *obj,TGListTreeItem *parent=0);
  const TGPicture *GetIcon(TClass *cls);
  void OnDoubleClicked(TGListTreeItem *item, Int_t btn); 

  std::string GetFullPath(TGListTreeItem *item) const;
  TObject*    GetObject(TGListTreeItem *item) const;

  bool HandleButton(Event_t *event);

  private:
    TGCanvas   *fCanvas;
    //TGListTree *fListTree;
    TGListTreeItem *fLastSelected;
    TList      *fActive;
  
    Histomatic *fHistomatic;
  
  ClassDef(GListTree,0)
};

class Histomatic { //: public TGMainFrame {
  //RQ_OBJECT("Histomatic")
  //Histomatic(const Histomatic&) = delete;
  //Histomatic& operator=(const Histomatic&) = delete;

  public:
    Histomatic();
    virtual ~Histomatic();    

    TGListTree *GetListTree() { return fGListTree; }

    void buttonAction();

  //private:
    void doUpdate();
    void doDraw(TObject*,Option_t *opt="");

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

    //TGCanvas          *fListTreeCanvas;
    //TGListTree        *fListTree;
    GListTreeCanvas    *fGListTreeCanvas;
    GListTree          *fGListTree;
    
  public:
    enum EHistMessages {
      kThing1,
      kThing2,
      kSendHelp,
      kCloseWindow
    };

    void CreateWindow();
    void CloseWindow();

    void AddRootFile(TFile *file) { fGListTree->InsertObject(file); }
    TList *GetAllActive();

  private:
    TGMainFrame *fMainWindow;
    //TGVerticalFrame   *fVf;

    bool fDrawNew;

  //ClassDefOverride(Histomatic,0)
  ClassDef(Histomatic,0)
};


#endif



