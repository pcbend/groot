#ifndef __GMARKER_H__
#define __GMARKER_H__

#include<TLine.h>
#include<TQObject.h>

#include <Gtypes.h>

class TH1;
class TCutG;

enum class GMarkerType {
  kAll =0,
  kPrimary,
  kBackground,
  kZoom,
  kFit,
  kCut,
  kProjection
};

//class GMarker : public TNamed {
class GMarker  : public TObject {
// public TLine { //, public TQObject {
  public:
    GMarker();
    ~GMarker();  

    void AddTo(TH1 *h, double x, double y=sqrt(-1),bool ignoreMax=false,Option_t *opt="");
    void Remove();

    //void Update();
    //void doUpdate();

    void Paint(Option_t *opt="") override;

    static void                  RemoveAll(TH1 *h, bool removeBGMarkers=true);  //remove all markers from h
    static std::vector<GMarker*> Get(TH1 *h,GMarkerType = GMarkerType::kAll);     //return all markers in h
    static std::vector<GMarker*> GetBG(TH1 *h);     //return all markers in h

    

    void SetX(double x) { fX = x; }
    void SetY(double y) { fY = y; }

    double GetX() const { return fX; }
    double GetY() const { return fY; }

    void SetLineColor(Color_t color);

    void ExecuteEvent(int event, int px, int py) override;
    int  DistancetoPrimitive(int px, int py) override;

    //bool IsBG() const { return fIsBgRegion; }
    //void SetBG() { fIsBgRegion = true; SetLineColor(1); }

    GMarkerType GetType() const { return fType; }
    void SetType(GMarkerType type) { fType = type; }

    bool IsPrimary()    const  { return fType == GMarkerType::kPrimary; }
    bool IsBackground() const  { return fType == GMarkerType::kBackground; }
    bool IsZoom()       const  { return fType == GMarkerType::kZoom; }
    bool IsFit()        const  { return fType == GMarkerType::kFit; }
    bool IsCut()        const  { return fType == GMarkerType::kCut; }
    bool IsProjection() const  { return fType == GMarkerType::kProjection; }

    static int  GetMaxMarkers(GMarkerType type);
    static void SetMaxMarkers(GMarkerType type,int value);
    
    static TCutG* MakeTCutG(TH1* h,GMarkerType type=GMarkerType::kPrimary); 

  public:
    double X() const { return fX; }
    double Y() const { return fY; }

  private:
    GMarkerType fType = GMarkerType::kPrimary;
    TH1 *fHist;
    TLine *fLineX;
    TLine *fLineY;  

    double fX;
    double fY;  


  ClassDefOverride(GMarker,0)
};

/*
class GBox : public TBox {
  public:
    GBox();
    ~GBox();

};
*/

#endif
