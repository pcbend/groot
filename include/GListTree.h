#ifndef __GLISTTREE_H__
#define __GLISTTREE_H__

#include <map>
#include <string>
#include <vector>

#include "TKey.h"


#include "TGListTree.h"

class TGWindow;
class GListTreeCanvas : public TGCanvas {
  public:
    GListTreeCanvas(const TGWindow *p=nullptr, UInt_t w=10, UInt_t h=10, UInt_t options=kSunkenFrame|kDoubleBorder, Pixel_t back=GetDefaultFrameBackground());
    ~GListTreeCanvas();

    bool HandleButton(Event_t *event);

  private:

  ClassDef(GListTreeCanvas,0)
};


class GListTree : public TGListTree {
  public: 
    //GListTree(TGWindow *parent=0,UInt_t w=1,UInt_t h=1, UInt_t options=kSunkenFrame|kDoubleBorder,Pixel_t back=GetDefaultFrameBackground());
    GListTree(TGCanvas *parent=0);
    ~GListTree();

    void ApplyThemeColors();
    void DrawRegion(Int_t x, Int_t y, UInt_t w, UInt_t h) override;
    void DrawOutline(Handle_t id, TGListTreeItem *item, Pixel_t col=0xbbbbbb,
                     Bool_t clear=kFALSE) override;
    void DrawActive(Handle_t id, TGListTreeItem *item) override;

	  void ClearActive();

  	void InsertObject(TObject *obj,TGListTreeItem *parent=0);
  	void AddManagedPath(const std::string& source,
  	                    const std::string& path,
  	                    TObject *obj);
  	const TGPicture *GetIcon(TClass *cls);
  
 	 	void Clicked(TGListTreeItem *item, int btn, unsigned int mask, int x, int y) override; 
 	 	void DoubleClicked(TGListTreeItem *item, int btn, int x, int y) override; 
    void doDraw(std::vector<TGListTreeItem*> items,Option_t *opt=""); 


  	std::string GetPath(TGListTreeItem *item) const;
 		std::string GetFullPath(TGListTreeItem *item) const;
  	std::string GetFileName(TGListTreeItem *item) const;
 	 	TObject*    GetObject(TGListTreeItem *item);
  	TKey*       GetKey(TGListTreeItem *item) const;

  	bool HandleButton(Event_t *event) override;

  	bool IsDrawable(const TGListTreeItem *item) const;


    std::vector<TGListTreeItem*> GetSelected() { return fSelected; }

    //const TGWindow *GetParent() { return fCanvas->GetParent(); }
    TGCanvas *GetCanvas() { return fCanvas; }


  private:
    void RestoreThemeGCColors();

    TGCanvas   *fCanvas;
    Pixel_t     fThemeForeground;
    Pixel_t     fThemeBackground;
    Pixel_t     fThemeSelectedForeground;
    Pixel_t     fThemeSelectedBackground;
    Pixel_t     fThemeLineForeground;
    //TGListTree *fListTree;
    TGListTreeItem *fLastSelected;
    int fLastY; 
    int fLastX; 

    std::vector<TGListTreeItem*> fSelected;
    std::map<std::string, TGListTreeItem*> fManagedItems;
  public:  
		std::map<std::string, TObject*> fObjReadMap;
	private:
		//Histomatic *fHistomatic;
  
  ClassDefOverride(GListTree,0)
};


#endif
