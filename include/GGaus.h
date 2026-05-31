#ifndef GGAUS_H
#define GGAUS_H

#include <GF1.h>

class TH1;

class GGaus : public GF1 {
  public:
    GGaus();
    GGaus(Double_t xlow,Double_t xhigh,Option_t *opt="gsc");
    //GGaus(Double_t xlow1,Double_t xlow2,Double_t xhigh1,Double_t xhigh2,Option_t *opt="gsc");
    GGaus(Double_t xlow,Double_t xhigh,TF1 *background,Option_t *opt="gsc");
    GGaus(const GGaus&);
    virtual ~GGaus();

    void InitNames();

    void Copy(TObject& obj) const override;
    void Clear(Option_t* opt = "") override;
    void Print(Option_t* opt = "") const override;

    bool InitParams(TH1* fithist = 0);
    bool Fit(TH1*,Option_t *opt="");
    //void DrawResiduals(); // *MENU*
    void DrawResiduals(TH1* hist) const;

    Double_t GetCentroid()    const { return GetParameter("centroid"); }
    Double_t GetCentroidErr() const { return GetParError(GetParNumber("centroid")); }
    Double_t GetFWHM()        const { return GetParameter("sigma")*2.3548;}
    Double_t GetFWHMErr()     const { return GetParError(GetParNumber("sigma"))*2.3548;}
  
  public: 
    //void CheckArea();
    //void CheckArea(Double_t int_low, Double_t int_high);
    static Bool_t CompareEnergy(const GGaus& lhs, const GGaus& rhs) {return lhs.GetCentroid()<rhs.GetCentroid();}
    static Bool_t CompareArea(const GGaus& lhs, const GGaus& rhs)   {return lhs.GetArea()<rhs.GetArea();}

  ClassDefOverride(GGaus,0)
};


#endif
