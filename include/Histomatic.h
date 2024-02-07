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
#include "TGStatusBar.h"
#include "TGComboBox.h"

#include "TFile.h"
#include "TKey.h"
#include "TList.h"

#include<exception>
#include<iostream>
#include<map>

class Histomatic;
class GEventTimer;
class TH1;
class TCanvas;

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

	  void ClearActive();

  	void InsertObject(TObject *obj,TGListTreeItem *parent=0);
  	const TGPicture *GetIcon(TClass *cls);
  
 	 	void Clicked(TGListTreeItem *item, int btn, unsigned int mask, int x, int y) override; 
 	 	void DoubleClicked(TGListTreeItem *item, int btn, int x, int y) override; 

  	std::string GetPath(TGListTreeItem *item) const;
 		std::string GetFullPath(TGListTreeItem *item) const;
  	std::string GetFileName(TGListTreeItem *item) const;
 	 	TObject*    GetObject(TGListTreeItem *item) const;
  	TKey*       GetKey(TGListTreeItem *item) const;

  	bool HandleButton(Event_t *event) override;

  	bool IsDrawable(const TGListTreeItem *item) const;


    std::vector<TGListTreeItem*> GetSelected() { return fSelected; }

  private:
    TGCanvas   *fCanvas;
    //TGListTree *fListTree;
    TGListTreeItem *fLastSelected;
    int fLastY; 
    int fLastX; 

    std::vector<TGListTreeItem*> fSelected;
  public:  
		std::map<std::string, TObject*> fObjReadMap;
	private:
		Histomatic *fHistomatic;
  
  ClassDefOverride(GListTree,0)
};




class Histomatic { //: public TGMainFrame {
  //RQ_OBJECT("Histomatic")
  //Histomatic(const Histomatic&) = delete;
  //Histomatic& operator=(const Histomatic&) = delete;

  public:
    Histomatic();
    virtual ~Histomatic();    

    //TGListTree *GetListTree() { return fGListTree; }
    GListTree *GetListTree() { return fGListTree; }

    void buttonAction();

    void SetStatusText(std::string text, int col) { fStatusBar->SetText(text.c_str(),col); }

  //private:
    void doUpdate();
    //void doDraw(TObject*,Option_t *opt="");
    //void doDraw(TList*,Option_t *opt="");
    void doDraw();
    void doDraw(std::vector<TGListTreeItem*> selected,Option_t *opt="");
    void drawHists(std::vector<TH1*> hists, TCanvas *g=0); 

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
    enum EDrawOption {
      eDrawNew,
      eDrawSame,
      eDrawStacked
    };
    TGCheckButton     *fDrawNormalized;
    TGCheckButton     *fDrawColz;


    //TGCanvas          *fListTreeCanvas;
    //TGListTree        *fListTree;
    GListTreeCanvas    *fGListTreeCanvas;
    GListTree          *fGListTree;
    
    TGStatusBar        *fStatusBar;

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
    //TList *GetAllActive();

    TGStatusBar *GetStatusBar() { return fStatusBar; } 

  private:
    TGMainFrame *fMainWindow;
    //TGVerticalFrame   *fVf;

    bool fDrawNew;

    TList *fTrash;
    //std::map<std::string, TObject*> fObjReadMap;
    GEventTimer *fEventTimer;


  //ClassDefOverride(Histomatic,0)
  ClassDef(Histomatic,0)
};


#endif



