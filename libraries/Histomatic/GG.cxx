
#include <GG.h>

#include <TCanvas.h>
#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h>
#include <TGButton.h>

#include <Histomatic.h>
#include <GCanvas.h>
#include <GH2D.h>

GG::GG(TH2 *mat) : fMain(0), fMatrix(0) {   
  //fMatrix = new GH2D(*mat);
  fMatrix = new GH2D(mat);
  fTotalX = fMatrix->ProjectionX();
  fTotalY = fMatrix->ProjectionY();

} 

GG::~GG() { 
}

void GG::CreateWindow() {
  fMain = new TGMainFrame(gClient->GetRoot(), 100, 100);
  fMain->SetWindowName("GG");

  fVFrame = new TGVerticalFrame(fMain, 100, 100);

  fButtonFrame = new TGHorizontalFrame(fVFrame, 100, 100);
  fButton1 = new TGTextButton(fButtonFrame, "&Button 1");
  fButton1->Connect("Clicked()", "GG", this, "DoButtonPress()");
  fButton2 = new TGTextButton(fButtonFrame, "&Button 2");
  fButton2->Connect("Clicked()", "GG", this, "DoButtonPress()");
  fButton3 = new TGTextButton(fButtonFrame, "&Button 3");
  fButton3->Connect("Clicked()", "GG", this, "DoButtonPress()");
  fButton4 = new TGTextButton(fButtonFrame, "&Button 4");
  fButton4->Connect("Clicked()", "GG", this, "DoButtonPress()");
  fButtonFrame->AddFrame(fButton1, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,0,0,0,0));
  fButtonFrame->AddFrame(fButton2, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,0,0,0,0));
  fButtonFrame->AddFrame(fButton3, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,0,0,0,0));
  fButtonFrame->AddFrame(fButton4, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,0,0,0,0));

  fVFrame->AddFrame(fButtonFrame, new TGLayoutHints(kLHintsExpandX, 0, 0, 0, 0));

  fEmCanvas = new TRootEmbeddedCanvas("fEmCanvas", fVFrame, 100, 100);
  int wid = fEmCanvas->GetCanvasWindowId();
  GCanvas *gc = new GCanvas("gcanvas",10,10,wid);
  fEmCanvas->AdoptCanvas(gc);

  fVFrame->AddFrame(fEmCanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,0,0,0,0));

  fMain->AddFrame(fVFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 10, 10));

  if(fMatrix) {
    fEmCanvas->GetCanvas()->cd();
    //fMatrix->Draw("colz2");
    fTotalX->Draw();
  }
 

  fMain->MapSubwindows();
  //fMain->Resize(fMain->GetDefaultSize());
  fMain->Resize(101,101);
  fMain->MapWindow();
}

void GG::DoButtonPress() {
  printf("GG::DoButton() called\n");
}


void GG::CloseWindow() {
  //fMain->CloseWindow();
  fMain->DontCallClose();
}


