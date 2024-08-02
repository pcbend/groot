#ifndef __GG_H__
#define __GG_H__

#include <vector>

#include<TObject.h>
#include<TGFrame.h>

#include <GListTree.h>

#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h>
//class TGMainFrame;
//class TRootEmbeddedCanvas;
class GH1D; 
class GH2D;
class TH2;
class TGVerticalFrame;
class TGHorizontalFrame;
class TGTextButton;

class TGListTreeItem;

//class TDirectoryFile;

class GListTree;

class GG : public TGMainFrame {

  public:
    GG(TH2 *mat);
    virtual ~GG();

    void CreateWindow();
    void CloseWindow() override;

    void ReallyDelete() override;

    void DoButtonPress();

    //TList *GetProjections() { return fProjections->GetList(); }
    TDirectory *GetProjections() { return fProjections; }
    GListTree *GetListTree() const { return fGListTree; }

    void doDraw(std::vector<TGListTreeItem*> selected, Option_t *opt="") const;

    TCanvas *GetCanvas() const  { return fEmCanvas->GetCanvas(); }
    void cd(int i=0) const { fEmCanvas->GetCanvas()->cd(i); }

  private: 
    
    //TGMainFrame *fMain;
 
    TGVerticalFrame *fVFrame;
    TGHorizontalFrame *fButtonFrame;

    TGTextButton *fButton1;
    TGTextButton *fButton2;
    TGTextButton *fButton3;
    TGTextButton *fButton4;


    TGHorizontalFrame *fHFrame;
    TRootEmbeddedCanvas *fEmCanvas;
    GListTreeCanvas    *fGListTreeCanvas;
    GListTree          *fGListTree;


  private:
    TDirectory  *fProjections;  

    //TH2         *fMatrix;
    //TH1         *fTotalX;
    //TH1         *fTotalY;
    GH2D       *fMatrix;
    GH1D        *fTotalX;
    GH1D        *fTotalY;

  ClassDefOverride(GG,0)
};



#endif
