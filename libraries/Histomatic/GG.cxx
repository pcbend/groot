
#include <GG.h>

#include <TCanvas.h>
//#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h>
#include <TGButton.h>
#include <TSystem.h>
#include <TVirtualX.h>

#include <Histomatic.h>
#include <GCanvas.h>
#include <GH2D.h>



GG::GG(TH2 *mat) : TGMainFrame(gClient->GetRoot(),100,100), fMatrix(0),fVFrame(0),fButtonFrame(0),fButton1(0),fButton2(0),fButton3(0),fButton4(0),fEmCanvas(0),fTotalX(0),fTotalY(0) {   
  //fMatrix = new GH2D(*mat);
  fMatrix = new GH2D(mat);
  fTotalX = fMatrix->ProjectionX();
  fTotalY = fMatrix->ProjectionY();

} 

GG::~GG() { 
  //TGMainFrame::~TGMainFrame();
  fEmCanvas->GetCanvas()->SetCanvasImp(0);
  fEmCanvas->GetCanvas()->Clear();
  fEmCanvas->GetCanvas()->SetName("");
  if(gPad && gPad->GetCanvas() == fEmCanvas->GetCanvas()) {
    gPad = 0;
  }
  
  fTotalX = 0;
  fTotalX = 0;
  if(fMatrix) delete fMatrix;

  if(fEmCanvas) delete fEmCanvas;
  if(fButton1) delete fButton1;
  if(fButton2) delete fButton2;
  if(fButton3) delete fButton3;
  if(fButton4) delete fButton4;
  if(fButtonFrame) delete fButtonFrame;
  if(fVFrame) delete fVFrame;

  //gVirtualX->DestroyWindow(fId);
 
 
}

void GG::CreateWindow() {
  //fMain = new TGMainFrame(gClient->GetRoot(), 100, 100);
  //fMain = new TGMainFrame(0, 100, 100);
  //fMain->SetWindowName("GG");
  if(fVFrame) {
    MapSubwindows();
    //fMain->Resize(fMain->GetDefaultSize());
    Resize(401,401);
    MapWindow();
    return;
  }

  SetWindowName("GG");

  fVFrame = new TGVerticalFrame(this, 100, 100);

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

  //fMain->AddFrame(fVFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 10, 10));
  AddFrame(fVFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 10, 10));

  if(fMatrix) {
    fEmCanvas->GetCanvas()->cd();
    //fMatrix->Draw("colz2");
    fTotalX->Draw();
  }
 

  //fMain->MapSubwindows();
  //fMain->Resize(fMain->GetDefaultSize());
  //fMain->Resize(401,401);
  //fMain->MapWindow();
  MapSubwindows();
  //fMain->Resize(fMain->GetDefaultSize());
  Resize(401,401);
  MapWindow();
}

void GG::DoButtonPress() {
  printf("GG::DoButton() called\n");
}


void GG::CloseWindow() {
  gVirtualX->UnmapWindow(fId);
  //gClient->WaitForUnmap(this);

/*
  printf("i am here\n");
  //TGMainFrame::CloseWindow();
  DeleteWindow();
  gVirtualX->CloseWindow();

  gClient->WaitForUnmap(this);
  gSystem->Sleep(150);
  TGMainFrame::CloseWindow();
  gSystem->Sleep(150);
  gSystem->ProcessEvents();
  //fMain->CloseWindow();
  //fMain->DontCallClose();
*/
}

void GG::ReallyDelete() {
  printf("i am here 2\n");
  TGMainFrame::ReallyDelete();
/*
  //fEmCanvas->GetCanvas()->Close();
  fEmCanvas->GetCanvas()->SetCanvasImp(0);
  fEmCanvas->GetCanvas()->Clear();
  fEmCanvas->GetCanvas()->SetName("");
  if(gPad && gPad->GetCanvas() == fEmCanvas->GetCanvas()) {
    gPad = 0;
  }
  delete this;
*/
  //TGMainFrame::ReallyDelete();



  //TGMainFrame::CloseWindow();
  //DeleteWindow();
  //fMain->CloseWindow();
  //fMain->DontCallClose();
}










