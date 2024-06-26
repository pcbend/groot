#ifndef GPEAK2_H
#define GPEAK2_H

#include <TF1.h>
#include <TFitResultPtr.h>

#include <string>
#include <algorithm>

class GPeak2 : public TF1 {
  public:
    //GPeak2(double peak,Option_t *opt="gsc");
    GPeak2(double xlow,double xhigh,Option_t *opt="gsc");
    //GPeak2(double xlow,double xhigh, TF1 *background,Option_t *opt="gsc");
    
    //GPeak2(const GPeak2&);
    GPeak2();
    virtual ~GPeak2();

    //void Copy(TObject&) const;
    //void Print(Option_t *opt="") const;
    //void Clear(Option_t *opt="");

    void InitNames();
    bool InitParams(TH1* fithist = 0);

    //TFitResultPtr Fit(TH1*,Option_t *opt="");
    //bool FitExclude(TH1*,double low, double high,Option_t *opt="");
    //void DrawResiduals(TH1*) const;
    //void DrawResiduals(); // *MENU*

    //TF1  *Background(Option_t *opt="TF1") { return &fBGFit; }
    //void DrawBackground(Option_t* opt = "SAME") const; // *MENU*

    //Double_t GetCentroid() const     { return GetParameter("centroid"); }
    //Double_t GetCentroidErr() const  { return GetParError(GetParNumber("centroid")); }
    //Double_t GetArea() const         { return fArea; }
    //Double_t GetAreaErr() const      { return fDArea; }
    //Double_t GetSum() const          { return fSum; }
    //Double_t GetSumErr() const       { return fDSum; }
    //Double_t GetFWHM() const         { return GetParameter("sigma")*2.3548;}
    //Double_t GetFWHMErr() const      { return GetParError(GetParNumber("sigma"))*2.3548;}
    //Double_t GetIntegralArea();
    //Double_t GetIntegralArea(Double_t int_low, Double_t int_high);
    //Double_t GetIntegralAreaErr();
    //Double_t GetIntegralAreaErr(Double_t int_low, Double_t int_high);

  //protected:
    //void SetArea(Double_t a)      { fArea = a; }
    //void SetAreaErr(Double_t d_a) { fDArea = d_a; }
    //void SetSum(Double_t a)       { fSum = a; }
    //void SetSumErr(Double_t d_a)  { fDSum = d_a; }
    //void SetArea(Double_t a, Double_t dA) { SetArea(a); SetAreaErr(dA);}
    //void SetChi2(Double_t chi2)   { fChi2 = chi2; }
    //void SetNdf(Double_t Ndf)     { fNdf  = Ndf; }
  
  public: 
    //void CheckArea();
    //void CheckArea(Double_t int_low, Double_t int_high);
    //static Bool_t CompareEnergy(const GPeak2& lhs, const GPeak2& rhs) {return lhs.GetCentroid()<rhs.GetCentroid();}
    //static Bool_t CompareArea(const GPeak2& lhs, const GPeak2& rhs)   {return lhs.GetArea()<rhs.GetArea();}



  private:
    //double fArea;
    //double fDArea;
    //double fSum;
    //double fDSum;
    //double fChi2;
    //double fNdf;

    //void DetachBackground();

    //Bool_t IsInitialized() const { return init_flag; }
    //void SetInitialized(Bool_t flag = true) {init_flag = flag;}
    //bool init_flag;

//    TF1 fBGFit;
    //    TF1 fBGHist;

  ClassDef(GPeak2,4)
};


#endif
