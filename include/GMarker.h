#ifndef __GMARKER_H__
#define __GMARKER_H__

#include<TLine.h>
#include<TQObject.h>

class TH1;

class GMarker : public TLine, public TQObject {
  public:
    GMarker();
    ~GMarker();  

    void AddTo(TH1 *h, double x, double y=sqrt(-1),Option_t *opt="");
    void Update();
    //void doUpdate();

  private:
    TH1 *fHist;
    double fX;
    double fY;  

  ClassDef(GMarker,0)
};


#endif
