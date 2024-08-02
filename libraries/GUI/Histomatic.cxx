
#include <Histomatic.h>
//#include <utils.h>
#include <vector>

#include "TQObject.h"
#include <TROOT.h>
#include <TVirtualPad.h>
#include <TObjString.h>
#include <TH1.h>
#include <TH2.h>
#include <TFrame.h>
#include <THStack.h>
#include <TGraph.h>
#include <TF1.h>
#include <TList.h>
#include <TStyle.h>
#include <TPad.h>
#include <TVirtualX.h>

//#include <GObjectManager.h>
#include <GCanvas.h>
#include <GH1D.h>
#include <GH2D.h>

#include <GGlobals.h>
#include <GUtils.h>

#include <GCommands.h>

#include <GListTree.h>

extern Histomatic *gHistomatic;

////////////////////

ClassImp(Histomatic);

Histomatic *Histomatic::fInstance = 0;

//Histomatic::Histomatic() : TGMainFrame(gClient->GetRoot(),200,600) {   
Histomatic::Histomatic() : TGMainFrame(gClient->GetRoot(),350,780), fVf(0) {

  int width  = 350;
  int height = 780;

  CreateWindow();
  this->SetWindowName("hist-o-matic");
  Show(width,height);

  fEventTimer = new GEventTimer();
  fEventTimer->Start();

}

Histomatic *Histomatic::Get() {
  if(!fInstance) fInstance = new Histomatic();
  gHistomatic = fInstance;
  return fInstance;
}

void Histomatic::Show(int width, int height) {
  //MoveResize(1,1,200,600);
  this->Resize(width,height);
  //SetWMPosition(1,1);
  //this->Resize(200,600);
  this->MapWindow();
}

Histomatic::~Histomatic() {
  if(gHistomatic == this)
    gHistomatic = 0;

  if(fEventTimer) {
    fEventTimer->Stop();
    delete fEventTimer;
  }

  delete fLH0;
  delete fLH1;
  delete fLH2;

  //TGMenuBar *fMenuBar;
  //TGHorizontalFrame *fTopMenuFrame;               
  //TGHorizontalFrame *fPreMenuFrame; 

  //TGPopupMenu       *fMenuFile;
  //TGPopupMenu       *fMenuHelp;     

  delete fButton1; 
  delete fButton2; 
  delete fButton3; 
  delete fButton4; 
  delete fButton5; 
  delete fButton6; 
  delete fButton7; 
  delete fButton8; 

  delete fDrawComboBox;
  delete fDrawColz;
  delete fDrawNormalized;

  delete fGListTree;
  delete fGListTreeCanvas;

  //delete fStatusBar;

  delete fButtonRow1;
  delete fButtonRow2;
  delete fButtonContainer;
  delete fDrawOptionContainer;

  delete fVf;
}

void Histomatic::CreateWindow() {

  if(fVf) {
    Show();
    return;
  }


  fVf = new TGVerticalFrame(this,100,100);

  fLH0 = new TGLayoutHints(kLHintsExpandX,0,0,0,0);
  fLH1 = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0);
  fLH2 = new TGLayoutHints(kLHintsCenterX,5,5,3,4);

  //fTopMenuFrame = new TGHorizontalFrame(fVf,100,20);

  //fPreMenuFrame = new TGHorizontalFrame(fTopMenuFrame,0,20,kRaisedFrame);
  //fMenuBar  = new TGMenuBar(fPreMenuFrame,10,10,kHorizontalFrame);
  //fMenuFile = new TGPopupMenu(gClient->GetDefaultRoot());
  //fMenuFile->AddEntry("thing1",kThing1);
  //fMenuFile->AddEntry("thing2",kThing2);

  //fMenuHelp = new TGPopupMenu(fClient->GetRoot());
  //fMenuHelp->AddEntry("Send Help", kSendHelp);

  //fPreMenuFrame->AddFrame(fMenuBar, fLH1);
  //fTopMenuFrame->AddFrame(fPreMenuFrame, fLH0);

  fButtonContainer = new TGVerticalFrame(fVf,200,200);

  fButtonRow1 = new TGHorizontalFrame(fButtonContainer,10,10);
  fButton1 = new TGTextButton(fButtonRow1,"button1");
  fButton1->Connect("Clicked()","Histomatic",this,"buttonAction()");
  fButton2 = new TGTextButton(fButtonRow1,"button2");
  fButton2->Connect("Clicked()","Histomatic",this,"buttonAction()");
  fButton3 = new TGTextButton(fButtonRow1,"button3");
  fButton3->Connect("Clicked()","Histomatic",this,"buttonAction()");
  fButton4 = new TGTextButton(fButtonRow1,"CloseAll");
  fButton4->Connect("Clicked()","Histomatic",this,"closeAllCanvases()");
  fButtonRow1->AddFrame(fButton1,fLH1);
  fButtonRow1->AddFrame(fButton2,fLH1);
  fButtonRow1->AddFrame(fButton3,fLH1);
  fButtonRow1->AddFrame(fButton4,fLH1);

  fButtonRow2 = new TGHorizontalFrame(fButtonContainer,10,10);
  fButton5 = new TGTextButton(fButtonRow2,"button5");
  fButton5->Connect("Clicked()","Histomatic",this,"buttonAction()");
  fButton6 = new TGTextButton(fButtonRow2,"button6");
  fButton6->Connect("Clicked()","Histomatic",this,"buttonAction()");
  fButton7 = new TGTextButton(fButtonRow2,"button7");
  fButton7->Connect("Clicked()","Histomatic",this,"buttonAction()");
  fButton8 = new TGTextButton(fButtonRow2,"do Draw");
  fButton8->Connect("Clicked()","Histomatic",this,"doDraw()");


  fButtonRow2->AddFrame(fButton5,fLH1);
  fButtonRow2->AddFrame(fButton6,fLH1);
  fButtonRow2->AddFrame(fButton7,fLH1);
  fButtonRow2->AddFrame(fButton8,fLH1);

  fButtonContainer->AddFrame(fButtonRow1,fLH1);  
  fButtonContainer->AddFrame(fButtonRow2,fLH1);  

  fDrawOptionContainer = new TGHorizontalFrame(fVf,100,40);

  fDrawComboBox = new TGComboBox(fDrawOptionContainer,100);
  fDrawComboBox->AddEntry("new canvas ",EDrawOption::eDrawNew);
  fDrawComboBox->AddEntry("same canvas",EDrawOption::eDrawSame);
  fDrawComboBox->AddEntry("stacked    ",EDrawOption::eDrawStacked);
  fDrawComboBox->Select(0);

  fDrawNormalized = new TGCheckButton(fDrawOptionContainer,"normalized",1);
  fDrawNormalized->SetState(kButtonUp);

  fDrawColz = new TGCheckButton(fDrawOptionContainer,"colz",1);
  fDrawColz->SetState(kButtonDown);

  fLockPads = new TGCheckButton(fDrawOptionContainer,"lock pads",1);
  fLockPads->SetState(kButtonUp);
  fLockPads->Connect("Clicked()","Histomatic",this,"doLockPads(TPad*)");
  //Connect("doLockPads()","Histomatic",this,"doLockPads()");
  //Connect("TCanvas","Picked()",this,"doLockPads()");
  TQObject::Connect("TCanvas","Picked(TPad*,TObject*,Int_t)","Histomatic",this,"doLockPads(TPad*)");
  /*
  Connect (const char *sender_class, 
           const char *signal, 
           const char *receiver_class, 
           void *receiver, c
           onst char *slot) */



  fDrawOptionContainer->AddFrame(fDrawComboBox,fLH1);
  fDrawOptionContainer->AddFrame(fDrawNormalized,fLH1);
  fDrawOptionContainer->AddFrame(fDrawColz,fLH0);
  fDrawOptionContainer->AddFrame(fLockPads,fLH0);

  fGListTreeCanvas = new GListTreeCanvas(fVf,10,10);
  fGListTree = new GListTree(fGListTreeCanvas); 

  fStatusBar = new TGStatusBar(fVf,100,50);
  fStatusBar->SetParts(4);
  {
    //fStatusBar->SetText("I AM A STATUS BAR");
    SetStatusText("some junk",0);
    fStatusBar->SetBackgroundColor(fStatusBar->GetBlackPixel());
    fStatusBar->SetForegroundColor(fStatusBar->GetWhitePixel());
  }

  fVf->AddFrame(fButtonContainer,fLH0);
  fVf->AddFrame(fDrawOptionContainer,fLH0);
  fVf->AddFrame(fGListTreeCanvas,fLH1);
  fVf->AddFrame(fStatusBar,fLH0);

  this->AddFrame(fVf,fLH1);
  this->MapSubwindows();
  this->Resize(this->GetDefaultSize());


}

void Histomatic::doLockPads(TPad *pad) {
  if(!gPad)
    return;
  if(!gPad->GetCanvas()->InheritsFrom(GCanvas::Class())) 
    return;

  //printf("pad = 0x%p\n",pad);
  //printf("gPad = 0x%p\n",gPad);


  if(pad!=gPad) { // in a new pad - set the button
    if(pad && pad->GetCanvas()->InheritsFrom(GCanvas::Class())) {
      fLockPads->SetState(((GCanvas*)pad->GetCanvas())->GetLockPads() ? kButtonDown : kButtonUp);
    }
  }

  //TODO -the feedback below is just broken.  need to fix
  return;


  //the pad and the gPad are always the same...(?) 
  if(((GCanvas*)gPad->GetCanvas())->GetLockPads()) {
    fLockPads->SetState(kButtonDown);
  } else {
    fLockPads->SetState(kButtonUp);
  }

  //if(fLockPads->GetState() == kButtonDown) {
  //  ((GCanvas*)gPad->GetCanvas())->SetLockPads(true);
  //} else {
  //  ((GCanvas*)gPad->GetCanvas())->SetLockPads(false);
  //}
}


void Histomatic::CloseWindow() {
  gVirtualX->UnmapWindow(fId);
  //fMainWindow->CloseWindow();  
  //  DeleteWindow();
}


void Histomatic::buttonAction() {

  printf("button ACTION!!! \n");

}

void Histomatic::doDraw() {
  doDraw(fGListTree->GetSelected(),"");
}

void Histomatic::doDraw(std::vector<TGListTreeItem*> selected, Option_t *opt) const {


  TList hList;

  std::vector<TH1*>    hists1D;
  std::vector<TH2*>    hists2D;
  std::vector<TGraph*> graphs;
  std::vector<TF1*>    functions;

  int drawables = 0;
  for(auto item=selected.begin();item!=selected.end();item++) {

    TKey          *key = fGListTree->GetKey(*item);  
    if(!key) continue;
    std::string fullPath = fGListTree->GetFullPath(*item);

    if(GetListTree()->fObjReadMap.find(fullPath) == GetListTree()->fObjReadMap.end()) { //obj not in map.
      TObject *obj = fGListTree->GetObject(*item);
      if(obj) {
        if(obj->InheritsFrom(TH2D::Class())) {
          GetListTree()->fObjReadMap[fullPath] = new GH2D(*static_cast<TH2D*>(obj));
        } else if(obj->InheritsFrom(TH1D::Class())) {
          GH1D *gh1d = new GH1D(*static_cast<TH1D*>(obj));
          gh1d->SetTitle(fullPath.c_str());
          GetListTree()->fObjReadMap[fullPath] = gh1d;
          //printf("I AM HERE\n");
          //GetListTree()->fObjReadMap[fullPath] = new GH1D(*static_cast<TH1D*>(obj));
        } else if(obj->InheritsFrom(TH1F::Class())) {
          GH1D *gh1d = new GH1D(*static_cast<TH1F*>(obj));
          gh1d->SetTitle(fullPath.c_str());
          //printf("I AM HERE\n");
          GetListTree()->fObjReadMap[fullPath] = new GH1D(*static_cast<TH1F*>(obj));
        } else {
          GetListTree()->fObjReadMap[fullPath] = obj;
        }
      }
    }

    TObject *obj = GetListTree()->fObjReadMap[fullPath];
    if(obj->InheritsFrom(TH2::Class())) {
      hists2D.push_back(static_cast<TH2*>(obj));
      drawables++;
    } else if(obj->InheritsFrom(TH1::Class())) {
      hists1D.push_back(static_cast<TH1*>(obj));
      drawables++;
    } else if(obj->InheritsFrom(TGraph::Class())) {
      graphs.push_back(static_cast<TGraph*>(obj));
      drawables++;
    } else if(obj->InheritsFrom(TF1::Class())) {
      functions.push_back(static_cast<TF1*>(obj));
      drawables++;
    } else {
      //pass;
    }
  }
  
  //printf("\n\nnum drawables: %i\n",drawables);
  //printf("found  %lu TH1 to draw.\n",    hists1D.size());
  //printf("found  %lu TH2 to draw.\n",    hists2D.size());
  //printf("found  %lu TGraph to draw.\n", graphs.size());
  //printf("found  %lu TF1    to draw.\n", functions.size());
  //printf("\n\n");

  if(drawables<1) return;

  //TODO - do something with the opt....

  //if(hList.GetSize()<1) return;

  //printf("doDraw\n");
  //printf("draw option: %i\n",fDrawComboBox->GetSelected());
  //printf("normalized:  %i\n",fDrawNormalized->GetState());
  //printf("colz:        %i\n",fDrawColz->GetState());


  TCanvas *g = 0;
  switch (fDrawComboBox->GetSelected()) {
    case EDrawOption::eDrawNew:
      //g = new GCanvas;
      break;
    case EDrawOption::eDrawSame:
    case EDrawOption::eDrawStacked:
      if(gPad)
        g = gPad->GetCanvas();
      else
        g = gROOT->MakeDefCanvas();  //new GCanvas;
      break;
  }


  if(hists1D.size()>0) {

    for(auto it=hists1D.begin();it!=hists1D.end();it++) {
      if((*it)->InheritsFrom(GH1D::Class())) {
        GH1D *current = static_cast<GH1D*>(*it);
        if(fDrawNormalized->GetState() == kButtonDown) {
          if(!current->IsNormalized()) {
            current->Normalize();
          }
        } else {
          if(current->IsNormalized()) {
            current->Normalize(); // this will un-normalize the histogram.
          }
        }
      }
    }
    drawHists(hists1D,g);

    /*
       if(hists1D.size()==1) {
       hists1D.at(0)->Draw();
       } else {
       if(hists1D.size()<=5) {
       g->Divide(1,hists1D.size(),0.01,0);
       int padN=1;
       for(auto it=hists1D.begin();it!=hists1D.end();it++) {
       g->cd(padN++);
       (*it)->Draw();
       }
       } else {
       THStack hs;
       for(auto it=hists1D.begin();it!=hists1D.end();it++) 
       hs.Add(*it);

       if(fDrawComboBox->GetSelected() == EDrawOption::eDrawSame)
       hs.Draw();
    //hs.DrawNormalized();
    else
    hs.Draw("pads");
    //hs.DrawNormalized("pads");

    }
    }
     */
  }
  if(hists2D.size()==1) {
    if(fDrawComboBox->GetSelected() != EDrawOption::eDrawNew)
      g = gROOT->MakeDefCanvas();//new GCanvas;

    GH2D *current = static_cast<GH2D*>(hists2D[0]);
    current->Draw();
  } else if(hists2D.size()>1) {
    if(fDrawComboBox->GetSelected() != EDrawOption::eDrawNew)
      g = gROOT->MakeDefCanvas(); //new GCanvas;
    THStack hs;
    for(auto it=hists1D.begin();it!=hists1D.end();it++) 
      hs.Add(*it);
    hs.Draw("pads");
  }


  //doUpdate(); 
}

void Histomatic::drawHists(std::vector<TH1*> hists, TCanvas *g) const {

  if(fDrawComboBox->GetSelected() == EDrawOption::eDrawSame ||
      fDrawComboBox->GetSelected() == EDrawOption::eDrawStacked) {
    TList *found = GrabHists(gPad->GetCanvas());
    if(found->GetEntries() > 0 ) { 
      TIter iter(found);
      while(TH1 *hist = (TH1*)iter.Next()) {
        hists.push_back(hist);
      }
      g->Clear();
    }
  }

  if(hists.size()==1) {   
    if(!g) 
      g = gROOT->MakeDefCanvas(); //new GCanvas;
    hists.at(0)->Draw();
    return;
  } 

  int ic;
  switch(fDrawComboBox->GetSelected()) {
    case EDrawOption::eDrawNew:
      for (auto it=hists.begin();it!=hists.end();it++) {
        drawHists(std::vector<TH1*>(it,it+1),0);
      } 
      break;
    case EDrawOption::eDrawSame:
      //THStack *hs = new THStack("hs","");
      ic = gStyle->GetColorPalette(0); 
      for(auto it=hists.begin();it!=hists.end();it++) {
        //gPad->IncrementPaletteColor(ic,"plc");
        (*it)->SetLineColor(ic++);
        if(GrabHist()) 
          (*it)->Draw("plc same");
        else 
          (*it)->Draw("plc");
      }
      gPad->BuildLegend(0.75,0.75,0.95,0.95,"");
      break;  
    case EDrawOption::eDrawStacked:
      if(hists.size()<=5) {
        g->Divide(1,hists.size(),0.01,0);
        int padN=1;
        for(auto it=hists.begin();it!=hists.end();it++) {
          g->cd(padN++);
          (*it)->Draw();
        }
      } else {
        THStack *hs = new THStack(Form("hs_%s",gPad->GetTitle()),"");
        for(auto it=hists.begin();it!=hists.end();it++) 
          hs->Add(*it);
        //printf("hs.GetNhists() = %i\n",hs.GetNhists());
        hs->Draw("pads");
      }
      break;
    default:
      break;
  };
}


void Histomatic::closeAllCanvases() {
  TSeqCollection *l = gROOT->GetListOfCanvases();
  TIter iter(l);
  while(TObject *obj = iter.Next()) {
    TCanvas *c = (TCanvas*)obj;
    c->Close();
  }
  return;
}


void Histomatic::doUpdate() {
  if(gPad) {
    gPad->Modified();
    gPad->Update();
  }
}



//TList *Histomatic::GetAllActive() { return fGListTree->GetAllActive(); } 
