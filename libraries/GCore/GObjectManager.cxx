
#include <GObjectManager.h>

#include <algorithm>
#include <atomic>

#include <TH1.h>
#include <TH2.h>
#include <TNamed.h>

#include <GH1D.h>
#include <GH2D.h>

GObjectManager *GObjectManager::fGObjectManager = 0;

namespace {

std::atomic<unsigned long> gUpdateCount(0);

void AddToManagerListIfNeeded(GObjectManager *manager, TObject *obj) {
	if(!manager || !obj) {
		return;
	}

	if(obj->InheritsFrom(TH1::Class())) {
		return;
	}

	if(!manager->GetList()->FindObject(obj)) {
		manager->GetList()->Add(obj);
	}
}

void MarkUpdated() {
	gUpdateCount.fetch_add(1, std::memory_order_relaxed);
}

bool UpdateHistogramContents(TH1 *dest, const TH1 *src) {
	if(!dest || !src) {
		return false;
	}
	if(dest->GetNcells() != src->GetNcells()) {
		return false;
	}

	for(int bin = 0; bin < src->GetNcells(); ++bin) {
		dest->SetBinContent(bin, src->GetBinContent(bin));
		dest->SetBinError(bin, src->GetBinError(bin));
	}
	dest->SetEntries(src->GetEntries());
	dest->ResetStats();
	return true;
}

}

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
	if(!obj) {
		return nullptr;
	}

	TObject *newObj = 0;
	if(obj->InheritsFrom(TH2D::Class())) {
		newObj = new GH2D(*static_cast<TH2D*>(obj));
	} else if(obj->InheritsFrom(TH2F::Class())) {
		newObj = new GH2D(*static_cast<TH2F*>(obj));
  } else if(obj->InheritsFrom(TH1D::Class())) {
		newObj = new GH1D(*static_cast<TH1D*>(obj));
	} else if(obj->InheritsFrom(TH1F::Class())) {
		newObj = new GH1D(*static_cast<TH1F*>(obj));
	} else {
		newObj = obj->Clone();
		if(!newObj) {
			newObj = obj;
		}
	}

	if(newObj && newObj->InheritsFrom(TH1::Class()))
		dynamic_cast<TH1*>(newObj)->SetDirectory(GObjectManager::instance());
	return newObj;
}


TObject *GObjectManager::Add(TObject* obj) {
	if(!obj) {
		return nullptr;
	}
	return AddObject("objects", obj->GetName(), obj);
}

TObject* GObjectManager::Get(TObject* obj) {
	if(!obj) {
		return nullptr;
	}

	TObject *obj2 = GetObject("objects", obj->GetName());
	if(!obj2) {
		obj2 = AddObject("objects", obj->GetName(), obj);
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

std::string GObjectManager::MakeKey(const std::string& source,
                                    const std::string& path) {
	if(path.empty()) {
		return source;
	}
	return source + ":" + path;
}

TObject *GObjectManager::AddObject(const std::string& source,
                                   const std::string& path,
                                   TObject *obj) {
	if(source.empty() || path.empty() || !obj) {
		return nullptr;
	}

	GObjectManager *manager = instance();
	const std::string key = MakeKey(source, path);
	auto found = manager->fManagedObjects.find(key);
	if(found != manager->fManagedObjects.end()) {
		return found->second;
	}

	TObject *managed = CreateGObject(obj);
	if(!managed) {
		return nullptr;
	}

	if(TNamed *named = dynamic_cast<TNamed*>(managed)) {
		named->SetTitle(key.c_str());
	}

	manager->fManagedSources.insert(source);
	manager->fManagedObjects[key] = managed;
	AddToManagerListIfNeeded(manager, managed);
	MarkUpdated();
	return managed;
}

TObject *GObjectManager::UpdateObject(const std::string& source,
                                      const std::string& path,
                                      TObject *obj) {
	if(source.empty() || path.empty() || !obj) {
		return nullptr;
	}

	GObjectManager *manager = instance();
	const std::string key = MakeKey(source, path);
	auto found = manager->fManagedObjects.find(key);
	if(found == manager->fManagedObjects.end()) {
		return AddObject(source, path, obj);
	}

	TObject *managed = found->second;
	if(managed &&
	   managed->InheritsFrom(TH1::Class()) &&
	   obj->InheritsFrom(TH1::Class()) &&
	   UpdateHistogramContents(static_cast<TH1*>(managed),
	                           static_cast<const TH1*>(obj))) {
		MarkUpdated();
		return managed;
	}

	bool copy_compatible =
		managed &&
		managed->IsA() == obj->IsA();
	if(copy_compatible) {
		obj->Copy(*managed);
		MarkUpdated();
		return managed;
	}

	if(managed) {
		manager->GetList()->Remove(managed);
		delete managed;
	}

	TObject *replacement = CreateGObject(obj);
	if(TNamed *named = dynamic_cast<TNamed*>(replacement)) {
		named->SetTitle(key.c_str());
	}
	manager->fManagedObjects[key] = replacement;
	AddToManagerListIfNeeded(manager, replacement);
	manager->fManagedSources.insert(source);
	MarkUpdated();
	return replacement;
}

TObject *GObjectManager::GetObject(const std::string& source,
                                   const std::string& path) {
	GObjectManager *manager = instance();
	auto found = manager->fManagedObjects.find(MakeKey(source, path));
	if(found == manager->fManagedObjects.end()) {
		return nullptr;
	}
	return found->second;
}

bool GObjectManager::HasObject(const std::string& source,
                               const std::string& path) {
	return GetObject(source, path) != nullptr;
}

bool GObjectManager::HasSource(const std::string& source) {
	return instance()->fManagedSources.count(source) != 0;
}

std::vector<std::string> GObjectManager::GetPaths(const std::string& source) {
	std::vector<std::string> paths;
	GObjectManager *manager = instance();
	const std::string prefix = source + ":";
	for(const auto& entry : manager->fManagedObjects) {
		if(entry.first.compare(0, prefix.size(), prefix) == 0) {
			paths.push_back(entry.first.substr(prefix.size()));
		}
	}
	std::sort(paths.begin(), paths.end());
	return paths;
}

unsigned long GObjectManager::UpdateCount() {
	return gUpdateCount.load(std::memory_order_relaxed);
}
