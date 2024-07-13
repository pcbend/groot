#ifndef __GG_H__
#define __GG_H__

#include<TObject.h>

class TGMainFrame;
class TRootEmbeddedCanvas;
class TH1; 
class TH2;
class TGVerticalFrame;
class TGHorizontalFrame;
class TGTextButton;

class fListTree;

class GG {

  public:
    GG(TH2 *mat);
    virtual ~GG();

    void CreateWindow();
    void CloseWindow();

    void DoButtonPress();

  private: 
    
    TGMainFrame *fMain;
 
    TGVerticalFrame *fVFrame;
    TGHorizontalFrame *fButtonFrame;

    TGTextButton *fButton1;
    TGTextButton *fButton2;
    TGTextButton *fButton3;
    TGTextButton *fButton4;

    TRootEmbeddedCanvas *fEmCanvas;

  private:

    TH2         *fMatrix;
    TH1         *fTotalX;
    TH1         *fTotalY;

    GListTree *fListTree;



  ClassDef(GG,0)
};



#endif
