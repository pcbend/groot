
#include <Histomatic.h>
#include <utils.h>

#include <TObjString.h>
#include <TH1.h>
#include <TH2.h>

ClassImp(HistList);

Histomatic *gHistomatic=0;

HistList::HistList(TGWindow *parent,UInt_t w,UInt_t h, UInt_t options,Pixel_t back) : TGCanvas(parent,w,h,options,back) { 
  fListTree = new TGListTree(this,kHorizontalFrame);
  fListTree->Connect("DoubleClicked(TGListTreeItem*,Int_t)","HistList",this,"OnDoubleClicked(TGListTreeItem*,Int_t)");
}

HistList::~HistList() {
  delete fListTree;
  if(fActive) {
    ClearActive();
    delete fActive;
  }
}

TGListTree *HistList::GetListTree() { return fListTree; }

void HistList::InsertObject(TObject *obj,TGListTreeItem *parent) {
  TGListTreeItem *item = fListTree->AddItem(parent,obj->GetName(),GetIcon(obj->IsA()),GetIcon(obj->IsA()));
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

const TGPicture *HistList::GetIcon(TClass *cls) {
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


void HistList::OnDoubleClicked(TGListTreeItem *item, Int_t btn) { 
  printf("%s\n",programPath().c_str()); 
} 

TList *HistList::GetAllActive(TGListTreeItem *item) {
  if(!item) item = fListTree->GetFirstItem();
  while(item) { 
    if(item->IsActive()) 
      std::cout << item->GetText() << std::endl;
    if(item->GetFirstChild()) GetAllActive(item->GetFirstChild());
    item = item->GetNextSibling();    
  }
  return 0;
}

void HistList::ClearActive() {
  //unhighlight??
  TIter iter(fActive);
  while(TObject *obj = iter.Next()) 
    obj->Delete();
  fActive->Clear();
}


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

  //this->DontCallClose();
}

Histomatic::~Histomatic() {

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

  delete fHistList;

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

  fHistList = new HistList(fVf,10,10); 

  fVf->AddFrame(fButtonContainer,fLH0);
  fVf->AddFrame(fHistList,fLH1);

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

TList *Histomatic::GetAllActive() { return fHistList->GetAllActive(); } 
