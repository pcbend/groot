
#include <Histomatic.h>
#include <utils.h>
#include <vector>

#include <TROOT.h>
#include <TVirtualPad.h>
#include <TObjString.h>
#include <TH1.h>
#include <TH2.h>
#include <TFrame.h>
#include <THStack.h>
#include <TGraph.h>

#include <GObjectManager.h>
#include <GCanvas.h>
#include <GH1D.h>
#include <GH2D.h>

#include <GGlobals.h>
#include <GUtils.h>


Histomatic *gHistomatic=0;


ClassImp(GListTreeCanvas);

GListTreeCanvas::GListTreeCanvas(const TGWindow *p, UInt_t w, UInt_t h, UInt_t options, Pixel_t back) : TGCanvas(p,w,h,options,back) { }

GListTreeCanvas::~GListTreeCanvas() { } 


bool GListTreeCanvas::HandleButton(Event_t *event) {
  return TGCanvas::HandleButton(event);
}



ClassImp(GListTree);

GListTree::GListTree(TGCanvas *parent, Histomatic *hist) : TGListTree(parent, kHorizontalFrame) {

  fLastSelected = 0;

  fHistomatic  = hist;

  //Connect("Clicked(TGListTreeItem*,Int_t)","GListTree",this,"OnClicked(TGListTreeItem*,Int_t)");
  //Connect("DoubleClicked(TGListTreeItem*,Int_t)","GListTree",this,"OnDoubleClicked(TGListTreeItem*,Int_t)");

}

GListTree::~GListTree() {
  //delete fListTree;
}

void GListTree::InsertObject(TObject *obj,TGListTreeItem *parent) {
  if(obj->InheritsFrom("TTree")) return;

  //bool checkBox = false;
  //if(obj->InheritsFrom(TH1::Class()))
  //  checkBox = true;
  //TGListTreeItem *item = this->AddItem(parent,obj->GetName(),GetIcon(obj->IsA()),GetIcon(obj->IsA()),checkBox);
  //item->CheckItem(false);
  TGListTreeItem *item = this->AddItem(parent,obj->GetName(),GetIcon(obj->IsA()),GetIcon(obj->IsA()));
  if(obj->IsFolder()) {
    TIter iter(((TDirectory*)obj)->GetListOfKeys());
    while(TKey *key = (TKey*)iter.Next()) { 
      TObject *keyObject = NULL;
      try{      
        //InsertObject(key->ReadObj(),item);
        keyObject = key->ReadObj();
      } catch(std::exception const &e) {
        std::cout << "caught: " << e.what() << std::endl;
      }
      if(keyObject) InsertObject(keyObject,item);
    }
  }
}

const TGPicture *GListTree::GetIcon(TClass *cls) {
  std::string path = programPath();
  path+="/../icons";
  if(cls->InheritsFrom(TFile::Class())) {
    path+="/rootdb_t.gif";
    return gClient->GetPicture(path.c_str());
  } else if(cls->InheritsFrom(TDirectory::Class())) { 
    path+="/folder_t.gif";
    return gClient->GetPicture(path.c_str());
  } else if(cls->InheritsFrom(TH2::Class())) { 
    path+="/h2_t.gif";
    return gClient->GetPicture(path.c_str());
  } else if(cls->InheritsFrom(TH1::Class())) { 
    path+="/hdb_t.gif";
    return gClient->GetPicture(path.c_str());
  }
  return 0;//gClient->GetPicture(path.c_str());

}


void GListTree::Clicked(TGListTreeItem *item, int btn, unsigned int mask, int x, int y) { 
  //printf("single clicked\n");
  //printf("mask: 0x%08x\n",mask);
  //printf("kKeyShiftMask: 0x%08x\n",kKeyShiftMask); 
  //printf("current: %s\n",item->GetText());
  //printf("parent: 0x%p\n",item->GetParent());
  //printf(" x = %i \t y = %i\n",x,y); 
  //if(fLastSelected) 
  //  printf("last:    %s\n",fLastSelected->GetText());

  ClearActive(); // clears fSelected.
  if((mask&kKeyShiftMask)  &&
     fLastSelected         && 
     item->GetParent() == fLastSelected->GetParent()) {

    TGListTreeItem *pitem = fLastSelected; 
    int count=0;
    while(pitem != item) {
      pitem->SetActive(true);
      fSelected.push_back(pitem);
      //printf("\t %s \n",pitem->GetText());
      if(y<fLastY) {
        pitem = pitem->GetPrevSibling();
      } else {
        pitem = pitem->GetNextSibling();
      }
    }
  } 
  fSelected.push_back(item);
  fLastSelected = item;
  fLastY = y;
  fLastX = x;

} 

void GListTree::DoubleClicked(TGListTreeItem *item, int btn, int x, int y) { 
  //printf("double clicked\n");
  //printf("%s\n",programPath().c_str()); 
  //printf("%s\n",GetFullPath(item).c_str());
  //printf("fSelected size = %i\n",fSelected.size()); 
  //TObject *obj = GetObject(item);
  //THStack *hs = new THStack;
  //bool oneCanvas = true 
 
  if(fSelected.size())
    fHistomatic->doDraw(fSelected);
  return;

/*
  if(fSelected.size()==1) {
    TObject *obj = GetObject(item);
    if(fHistomatic) fHistomatic->doDraw(obj);
  } else if (fSelected.size()>1) {
    TList *list = new TList;
    for(auto it=fSelected.begin();it!=fSelected.end();it++) { 
      TObject *obj = GetObject(*it);
      list->Add(obj);
    }
    if(fHistomatic) fHistomatic->doDraw(list);
  }
*/
  //if(GetObject(item)->InheritsFrom(TH1::Class()) && fHistomatic) fHistomatic->doDraw(hs);
} 

//void GListTree::OnDoubleClicked(TGListTreeItem *item, Int_t btn) { 
//  printf("%s\n",programPath().c_str()); 
//  printf("%s\n",GetFullPath(item).c_str()); 
//  TObject *obj = GetObject(item);
//  if(fHistomatic) fHistomatic->doDraw(obj);
//} 

std::string GListTree::GetPath(TGListTreeItem *item) const {
  if(!item) return "";
  std::vector<std::string> path;
  while(item) {
    path.push_back(item->GetText());
    item = item->GetParent();
  }
  if(path.size()==1) return "";
  //last element contains the filename, let's drop it.
  path.pop_back();
  std::string fullPath = path.back();
  std::vector<std::string>::reverse_iterator it;
  for(it=path.rbegin()+1;it!=path.rend();++it) {
    fullPath += "/";
    fullPath += *it;
  } 
  //printf("GETFULLPATH: %s\n",fullPath.c_str());
  return fullPath;    
}

std::string GListTree::GetFileName(TGListTreeItem *item) const {
  if(!item) return "";
  std::vector<std::string> path;
  while(item) {
    path.push_back(item->GetText());
    item = item->GetParent();
  }
  return path.back(); 
}

std::string GListTree::GetFullPath(TGListTreeItem *item) const {
  std::string path = GetFileName(item);
  path += ":";
  path += GetPath(item);
  return path;
}

TObject *GListTree::GetObject(TGListTreeItem *item) const {
  TObject *obj=0;
  std::string   path = GetPath(item);
  std::string   file = GetFileName(item);
  TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(file.c_str());
  //printf("file: %s\n",file.c_str());
  //printf("path: %s\n",path.c_str());
  if(f) obj = f->Get(path.c_str());
  //printf("obj: 0x%p\n",obj);
  return obj;
}


TKey *GListTree::GetKey(TGListTreeItem *item) const {
  TKey *key=0;
  std::string   path = GetPath(item);
  std::string   file = GetFileName(item);
  TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(file.c_str());
  if(!f || path.length()==0) return key;  
  TDirectory *current = gDirectory;
  std::vector<std::string> parts = tokenizeString(path);    
  f->cd();
  for(size_t i=0;i<parts.size()-1;i++) {
    f->cd(parts.at(i).c_str());
  }
  key = gDirectory->FindKey(parts.back().c_str());
  current->cd(); 
  return key;
}


/*
TList *GListTree::GetAllActive(TGListTreeItem *item) {
  if(!item) item = GetFirstItem();
  while(item) { 
    if(item->IsActive()) 
      std::cout << item->GetText() << std::endl;
    if(item->GetFirstChild()) GetAllActive(item->GetFirstChild());
    item = item->GetNextSibling();    
  }
  return 0;
}
*/

void GListTree::ClearActive() {
  for(auto it=fSelected.begin();it!=fSelected.end();it++) 
    (*it)->SetActive(false);
  fSelected.clear();
}



bool GListTree::HandleButton(Event_t *event) {
  bool handled = false;
  if(event->fType == EGEventType::kButtonPress) {
    //std::cout << "fcode:  " << event->fCode  << std::endl;
    //std::cout << "fstate: " << event->fState << std::endl;
  
    TGListTreeItem *item = 0;
    item = FindItem(event->fY);
    //if(GetSelected()!=0) {
      //std::cout<< "old: " << GetSelected()->GetText() << std::endl; 
    //}
    //if(item!=0) {
      //std::cout<< "new: " << item->GetText() << std::endl; 
    //}
  }  
  if(handled) SetUserControl(true);
  bool temp =  TGListTree::HandleButton(event);
  if(handled) SetUserControl(false);
  return temp;
}


bool GListTree::IsDrawable(const TGListTreeItem *item) const {
  //if(!item) return false; 
  //if(item->InheritsFrom(TH1::Class()) ||
  //   item->InheritsFrom(TGraph::Class()))
    return true;
  //return false;

}


////////////////////

ClassImp(Histomatic);

//Histomatic::Histomatic() : TGMainFrame(gClient->GetRoot(),200,600) {   
Histomatic::Histomatic() {

  int width  = 350;
  int height = 780;

  fMainWindow = new TGMainFrame(0,width,height);

  //TGLayoutHints *hints = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,1,1,1,1);

  //TGTextButton *button1 = new TGTextButton(vframe,"button1");
  //vframe->AddFrame(button1,hints);
  //vframe->AddFrame(button1,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,1,1,1,1));

  //TGTextButton *button2 = new TGTextButton(vframe,"button2");
  //vframe->AddFrame(button2,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,10,10,10,1));

  //this->AddFrame(vframe,hints);
  //this->AddFrame(vframe,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,1,1,1,1));


  CreateWindow();
  fMainWindow->SetWindowName("hist-o-matic");

  //MoveResize(1,1,200,600);
  fMainWindow->Resize(width,height);
  //SetWMPosition(1,1);


  //this->Resize(200,600);
  fMainWindow->MapWindow();

  fEventTimer = new GEventTimer();
  fEventTimer->Start();
 
  //this->DontCallClose();
}

Histomatic::~Histomatic() {
  if(gHistomatic = this)
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

  delete fGListTree;
  delete fGListTreeCanvas;

  //delete fStatusBar;

  delete fButtonRow1;
  delete fButtonRow2;
  delete fButtonContainer;

  delete fVf;
}

void Histomatic::CreateWindow() {

  fVf = new TGVerticalFrame(fMainWindow,100,100);

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
  fButton4 = new TGTextButton(fButtonRow1,"button4");
  fButton4->Connect("Clicked()","Histomatic",this,"buttonAction()");
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
  fButton8 = new TGTextButton(fButtonRow2,"button8");
  fButton8->Connect("Clicked()","Histomatic",this,"buttonAction()");
  fButtonRow2->AddFrame(fButton5,fLH1);
  fButtonRow2->AddFrame(fButton6,fLH1);
  fButtonRow2->AddFrame(fButton7,fLH1);
  fButtonRow2->AddFrame(fButton8,fLH1);

  fButtonContainer->AddFrame(fButtonRow1,fLH1);  
  fButtonContainer->AddFrame(fButtonRow2,fLH1);  

  fGListTreeCanvas = new GListTreeCanvas(fVf,10,10);
  fGListTree = new GListTree(fGListTreeCanvas,this); 

  fStatusBar = new TGStatusBar(fVf,100,50);
  fStatusBar->SetParts(4);
  {
    //fStatusBar->SetText("I AM A STATUS BAR");
    SetStatusText("some junk",0);
    fStatusBar->SetBackgroundColor(fStatusBar->GetBlackPixel());
    fStatusBar->SetForegroundColor(fStatusBar->GetWhitePixel());
  }

  fVf->AddFrame(fButtonContainer,fLH0);
  fVf->AddFrame(fGListTreeCanvas,fLH1);
  fVf->AddFrame(fStatusBar,fLH0);

  fMainWindow->AddFrame(fVf,fLH1);
  fMainWindow->MapSubwindows();
  fMainWindow->Resize(fMainWindow->GetDefaultSize());


}


void Histomatic::CloseWindow() {
  fMainWindow->CloseWindow();  
  //  DeleteWindow();
}


void Histomatic::buttonAction() {

  printf("button ACTION!!! \n");

}

void Histomatic::doDraw(std::vector<TGListTreeItem*> selected, Option_t *opt) {
  
  TList hList;
  for(auto item=selected.begin();item!=selected.end();item++) {
    
    //std::string   file = fGListTree->GetFileName(*item);
    //std::string   path = fGListTree->GetPath(*item);
    TKey          *key = fGListTree->GetKey(*item);  
    /*
    TObject      *obj  = fGListTree->GetObject(*item);
    if(key)
      printf("key:      %s\n",key->GetClassName());
    if(obj)
      printf("obj:      %s\n",obj->GetName());
    printf("file:     %s\n",file.c_str());
    printf("path:     %s\n",path.c_str());
    printf("fullPath: %s\n\n",fGListTree->GetFullPath(*item).c_str());
    */
    if(!key) continue;
    bool drawable =false;
    if(TClass::GetClass(key->GetClassName())->InheritsFrom(TH1::Class())) // || 
       //TClass::GetClass(key->GetClassName())->InheritsFrom(TGraph::Class())) 
      drawable = true;
    if(drawable) {
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
      hList.Add(GetListTree()->fObjReadMap[fullPath]);
    }    
  }
  //TODO - do something with the opt....

  if(hList.GetSize()<1) return;

  bool only1D = true;
  THStack hs;
  TIter iter(&hList);

  GCanvas *g = 0;
  while(TObject *obj = iter.Next()) {
    if(obj->InheritsFrom(TH2::Class())) { 
      only1D = false;
    }
    if(obj->InheritsFrom(TH1::Class())) { //TODO: also draw non-histograms....
      //printf("obj = 0x%p\n",obj);
      hs.Add(dynamic_cast<TH1*>(obj));  
    }
  }
//return;

  if(only1D) {
    if(!g)
      g = new GCanvas;
    if(hs.GetNhists()<=5) {
      g->Divide(1,hs.GetNhists(),0.01,0);
      TIter nexth(hs.GetHists());
      int cpad=1;
      while(TObject *h=nexth.Next()) {
        g->cd(cpad++);
        h->Draw();
      }
    } else {
      hs.Draw("pads");
    }
  } else {  
    if(!g)
      g = new GCanvas;
    hs.Draw("pads");
  }
  doUpdate(); 
}

void Histomatic::doUpdate() {
  if(gPad) {
    gPad->Modified();
    gPad->Update();
  }


}



//TList *Histomatic::GetAllActive() { return fGListTree->GetAllActive(); } 
