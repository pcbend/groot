#ifndef __GROI_H__ 
#define __GROI_H__ 

#include<TNamed.h>

class GMarker;
class TH1;
class TGraph;
class TF1;
class TLine;

class GROI : public TNamed {

  public:
    GROI();
    GROI(const char* name, const char* title="");
    GROI(GMarker *m1, GMarker *m2, const char* name="ROI", const char* title="ROI");
    GROI(double x1, double x2, const char* name="ROI", const char* title="ROI");

    virtual ~GROI();
    
    void SetParent(TH1* parent); // { fParent = parent; }
    TH1* GetParent()  const     { return fParent; }

    void SetFit(TF1* fit) { fFit = fit; }
    TF1* GetFit() const { return fFit; }


    void Draw(Option_t* opt="") override;
    void Paint(Option_t* opt="") override;
    void Update();

    void ExecuteEvent(int event, int px, int py) override;
    int  DistancetoPrimitive(int px, int py) override;

    static void RemoveAll(TH1* h);

/////

  private:
    //GMarker* fMarker1;
    //GMarker* fMarker2;
    //GMarker* fCurrentMarker;
    double xlow;
    double xhigh;

    TH1*    fFill;
    TF1*    fFit;

    //TLine*  fLow;
    //TLine*  fHigh;


  private:
    void CreateFill();
    TH1* fParent; 

  ClassDefOverride(GROI, 1)

};

#endif 
