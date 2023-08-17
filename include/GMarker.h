#ifndef __GMARKER_H__
#define __GMARKER_H__

#include<TLine.h>
#include<TQObject.h>

class TH1;

class GMarker : public TLine { //, public TQObject {
  public:
    GMarker();
    ~GMarker();  

    void AddTo(TH1 *h, double x, double y=sqrt(-1),Option_t *opt="");
    void Remove();

    //void Update();
    //void doUpdate();

    void Paint(Option_t *opt="") override;
    void ExecuteEvent(int event, int px, int py) override;

    static void                  RemoveAll(TH1 *h);  //remove all markers from h
    static std::vector<GMarker*> GetAll(TH1 *h); //return all markers in h
    
  public:
    double X() const { return fX; }
    double Y() const { return fY; }

  private:
    TH1 *fHist;
    double fX;
    double fY;  

  ClassDefOverride(GMarker,0)
};


#endif
