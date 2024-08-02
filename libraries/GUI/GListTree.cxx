
#include <GListTree.h>
#include <utils.h>

#include <TROOT.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TPad.h>

#include<GH1D.h>
#include<GH2D.h>
#include <GObjectManager.h>

#include <GGlobals.h>

// want to remove....
#include <Histomatic.h>
#include <GG.h>

ClassImp(GListTreeCanvas);

GListTreeCanvas::GListTreeCanvas(const TGWindow *p, UInt_t w, UInt_t h, UInt_t options, Pixel_t back) : 
                  TGCanvas(p,w,h,options,back) {
 }

GListTreeCanvas::~GListTreeCanvas() { } 

bool GListTreeCanvas::HandleButton(Event_t *event) {
  return TGCanvas::HandleButton(event);
}

ClassImp(GListTree);

GListTree::GListTree(TGCanvas *parent) : 
  TGListTree(parent, kHorizontalFrame),fLastSelected(0) {
  fCanvas = parent;
}

GListTree::~GListTree() {
  //delete fListTree;
}

///
// This is the only method to get contents from a root file into the list-tree
///
void GListTree::InsertObject(TObject *obj,TGListTreeItem *parent) {
  if(obj->InheritsFrom("TTree")) return;
 
  TClass *cls = 0;
  if(obj->IsA()==TKey::Class()) 
    cls = TClass::GetClass(static_cast<TKey*>(obj)->GetClassName());
  else 
    cls = obj->IsA();
  TGListTreeItem *item = this->AddItem(parent,obj->GetName(),GetIcon(cls),GetIcon(cls));
  if(obj->IsFolder()) {
    if(obj->IsA() == TKey::Class()) //now read the object... 
      obj = ((TKey*)obj)->ReadObj(); 
    TList *keys = ((TDirectory*)obj)->GetListOfKeys(); 
    if(keys && keys->GetEntries()>0) {  // only directories written to a file will have keys
      TIter iter(keys);                 // is we have a dir in memory, it will not have keys...
      while(TKey *key = (TKey*)iter.Next()) { 
        if(key) InsertObject(key,item);
      }
    } else {
      //printf("no keys\n");
      if(obj->InheritsFrom(TDirectory::Class())) {
        TIter iter(((TDirectory*)obj)->GetList());
        while(TObject *obj = iter.Next()) {
          //printf("obj->GetName() = %s\n",obj->GetName());
          InsertObject(obj,item);
       }
      }
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

  if((mask&kKeyControlMask) && 
    fLastSelected && 
    item->GetParent() == fLastSelected->GetParent()) {
  
    //item->SetActive(true);
    //fSelected.push_back(item);
    //printf("adding to active\n");
    //printf("fSelected size = %i\n",fSelected.size());
    for(auto it=fSelected.begin();it!=fSelected.end();it++) 
      (*it)->SetActive(true);

  } else {
    ClearActive(); // clears fSelected.
    //printf("clearing active\n");
  }


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

  if(fSelected.size()) { // && gHistomatic)
    const TGWindow *mf = GetMainFrame();
    if(mf && mf->InheritsFrom(Histomatic::Class())) {
      static_cast<const Histomatic*>(mf)->doDraw(fSelected);
    } else if(mf && mf->InheritsFrom(GG::Class())) {
      static_cast<const GG*>(mf)->doDraw(fSelected);
    }
    //doDraw(fSelected);
    //fHistomatic->doDraw(fSelected);
  }
  return;
} 


void GListTree::doDraw(std::vector<TGListTreeItem*> selected,Option_t *opt) {
  TList hList;
    
  std::vector<TH1*>    hists1D;
  std::vector<TH2*>    hists2D;
  std::vector<TGraph*> graphs;
  std::vector<TF1*>    functions;
    
  int drawables = 0;
  for(auto item=selected.begin();item!=selected.end();item++) {
    
    TKey          *key = GetKey(*item);  
    if(!key) continue;
    std::string fullPath = GetFullPath(*item);
    
    if(fObjReadMap.find(fullPath) == fObjReadMap.end()) { //obj not in map.
      TObject *obj = GetObject(*item);
      if(obj) {
        if(obj->InheritsFrom(TH2D::Class())) {
          fObjReadMap[fullPath] = new GH2D(*static_cast<TH2D*>(obj));
        } else if(obj->InheritsFrom(TH1D::Class())) {
          GH1D *gh1d = new GH1D(*static_cast<TH1D*>(obj));
          gh1d->SetTitle(fullPath.c_str());
          fObjReadMap[fullPath] = gh1d;
          //printf("I AM HERE\n");
          //GetListTree()->fObjReadMap[fullPath] = new GH1D(*static_cast<TH1D*>(obj));
        } else if(obj->InheritsFrom(TH1F::Class())) {
          GH1D *gh1d = new GH1D(*static_cast<TH1F*>(obj));
          gh1d->SetTitle(fullPath.c_str());
          //printf("I AM HERE\n");
          fObjReadMap[fullPath] = new GH1D(*static_cast<TH1F*>(obj));
        } else {
          fObjReadMap[fullPath] = obj;
        }
      }
    }
    
    TObject *obj = fObjReadMap[fullPath];
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
  if(drawables<1) return;
  //TODO - do something with the opt....

  TCanvas *g = 0;
  if(gHistomatic) {
    switch (gHistomatic->GetDrawOption()) {
      case Histomatic::EDrawOption::eDrawNew:
        //g = new GCanvas;
        break;
      case Histomatic::EDrawOption::eDrawSame:
      case Histomatic::EDrawOption::eDrawStacked:
        if(gPad)
          g = gPad->GetCanvas();
        else
          g = gROOT->MakeDefCanvas();  //new GCanvas;
        break;
    }  
  } 
  if(!g) g = gROOT->MakeDefCanvas();  //new GCanvas;

  for(size_t i=0;i<hists1D.size();i++) {
    hists1D.at(i)->Draw();
  }


  for(size_t i=0;i<hists2D.size();i++) {
    hists2D.at(i)->Draw();
  }



}



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

TObject *GListTree::GetObject(TGListTreeItem *item) {
  TObject *obj=0;
  std::string fullPath = GetFullPath(item);
  printf("fullPath: %s\n",fullPath.c_str());
  if(fObjReadMap.find(fullPath) == fObjReadMap.end()) { //obj not in map.

    std::string   path = GetPath(item);
    std::string   file = GetFileName(item);

    TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(file.c_str());
    //printf("file: %s\n",file.c_str());
    //printf("path: %s\n",path.c_str());
    if(f) {
      obj = f->Get(path.c_str());
    } else { //failed to find file, lets try to just find the object.
      obj = gROOT->FindObjectAny(path.c_str());
    }
    fObjReadMap[fullPath] = obj;
  } else {
    obj = fObjReadMap[fullPath];
  }
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

