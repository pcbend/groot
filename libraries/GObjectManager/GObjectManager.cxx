
#include <GObjectManager.h>

#include <TH1.h>
#include <TH2.h>

#include <GH1D.h>
#include <GH2D.h>

GObjectManager *GObjectManager::fGObjectManager = 0;

GObjectManager::GObjectManager() : 
	TDirectory("GObjectManager","GObjectManager","GObjectManager",0) { }

GObjectManager::~GObjectManager() { }

GObjectManager *GObjectManager::instance() {
	if(!fGObjectManager) {
		TDirectory *current = gDirectory;
		fGObjectManager = new GObjectManager;
		current->cd();
	}
	return fGObjectManager;
}

TObject *GObjectManager::CreateGObject(TObject *obj) {
	TObject *newObj = 0;
  //if(obj) {
    if(obj->InheritsFrom(TH2D::Class())) {
      newObj = new GH2D(*static_cast<TH2D*>(obj));
    } else if(obj->InheritsFrom(TH1D::Class())) {
      newObj = new GH1D(*static_cast<TH1D*>(obj));
    } else if(obj->InheritsFrom(TH1F::Class())) {
      newObj = new GH1D(*static_cast<TH1F*>(obj));
    } else {
      newObj = obj;
    }
  //}
	if(newObj && newObj->InheritsFrom(TH1::Class()))
		dynamic_cast<TH1*>(newObj)->SetDirectory(GObjectManager::instance());
	return newObj;
}


void GObjectManager::Add(TObject* obj) {
	if(instance()->GetList()->FindObject(obj)) {
		printf("obj: %s appears to already be managed.\n",obj->GetName());
		return;
	}
	obj = instance()->CreateGObject(obj);

	instance()->GetList()->Add(obj);
	return;
}

TObject* GObjectManager::Get(TObject* obj) {
	TObject *obj2 = instance()->FindObject(obj);
	if(!obj2) {
		GObjectManager::instance()->Add(obj);
		obj2 = obj;
	}
	return obj2;
}

TObject* GObjectManager::Get(std::string name) {
	TObject *obj = instance()->FindObject(name.c_str());
	if(!obj) {
		printf("obj: %s not found in manager.\n",name.c_str());
	}
	return obj;
}






