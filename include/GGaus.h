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

    void Clear(Option_t* opt = "") override;
    void Print(Option_t* opt = "") const override;

    bool InitParams(TH1* fithist = 0);
    bool Fit(TH1*,Option_t *opt="");

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


class GDoubleGaus : public GF1 {
  public:
    GDoubleGaus(Double_t cent1,Double_t cent2,Double_t xlow,Double_t xhigh,Option_t *opt="gsc");

    GDoubleGaus(const GDoubleGaus&);
    GDoubleGaus();
    virtual ~GDoubleGaus();

    void Copy(TObject&) const override;
    void Print(Option_t *opt="") const override;
    void Clear(Option_t *opt="") override;

    void InitNames();
    bool InitParams(TH1* fithist = 0,double cent1=0,double cent2=1);
    bool Fit(TH1*,double c1,double c2,Option_t *opt="");
    void DrawResiduals(TH1*) const;
    //void DrawResiduals(); // *MENU*

    TF1  *Background(Option_t *opt="TF1") { return &fBGFit; }
    //void DrawBackground(Option_t* opt = "SAME") const; // *MENU*

    Double_t GetCentroid1() const     { return GetParameter("centroid1"); }
    Double_t GetCentroid1Err() const  { return GetParError(GetParNumber("centroid1")); }

    Double_t GetCentroid2() const     { return GetParameter("centroid2"); }
    Double_t GetCentroid2Err() const  { return GetParError(GetParNumber("centroid2")); }

    Double_t GetAreaTotal() const         { return fAreaTotal; }
    Double_t GetAreaTotalErr() const      { return fDAreaTotal; }
    Double_t GetArea1() const             { return fArea1; }
    Double_t GetArea1Err() const          { return fDArea1; }
    Double_t GetArea2() const             { return fArea2; }
    Double_t GetArea2Err() const          { return fDArea2; }

    Double_t GetSumTotal() const          { return fSumTotal; }
    Double_t GetSumTotalErr() const       { return fDSumTotal; }
    Double_t GetSum1() const              { return fSum1; }
    Double_t GetSum1Err() const           { return fDSum1; }
    Double_t GetSum2() const              { return fSum2; }
    Double_t GetSum2Err() const           { return fDSum2; }

    Double_t GetFWHM() const         { return GetParameter("sigma")*2.3548;}
    Double_t GetFWHMErr() const      { return GetParError(GetParNumber("sigma"))*2.3548;}

  protected:
    void SetAreaTotal(Double_t a) { fAreaTotal = a; }
    void SetAreaTotalErr(Double_t d_a) { fDAreaTotal = d_a; }
    void SetSumTotal(Double_t a)       { fSumTotal = a; }
    void SetSumTotalErr(Double_t d_a)  { fDSumTotal = d_a; }
    void SetAreaTotal(Double_t a, Double_t dA) { SetAreaTotal(a); SetAreaTotalErr(dA);}
    void SetChi2(Double_t chi2)   { fChi2 = chi2; }
    void SetNdf(Double_t Ndf)     { fNdf  = Ndf; }

  public:
    //void CheckArea();
    //void CheckArea(Double_t int_low, Double_t int_high);
    //static Bool_t CompareEnergy(const GDoubleGaus& lhs, const GDoubleGaus& rhs) {return lhs.GetCentroid()<rhs.GetCentroid();}
    //static Bool_t CompareArea(const GDoubleGaus& lhs, const GDoubleGaus& rhs)   {return lhs.GetArea()<rhs.GetArea();}

  private:
    double fAreaTotal;
    double fDAreaTotal;
    double fArea1;
    double fDArea1;
    double fArea2;
    double fDArea2;

    double fChi2;
    double fNdf;

    double fSumTotal;
    double fDSumTotal;
    double fSum1;
    double fDSum1;
    double fSum2;
    double fDSum2;


    Bool_t IsInitialized() const { return init_flag; }
    void SetInitialized(Bool_t flag = true) {init_flag = flag;}
    bool init_flag;

    TF1 fBGFit;
    TF1 fBGHist;

    TF1 fGaus1;
    TF1 fGaus2;

  ClassDefOverride(GDoubleGaus,0)
};



#endif
