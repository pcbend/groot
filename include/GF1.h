#ifndef __GF1_H__
#define __GF1_H__

#include <TF1.h>

class TFitResultPtr;

struct GFitRange {
  double fitLow   = 0.0;
  double fitHigh  = 0.0;

  double bgLeftLow   = 0.0;
  double bgLeftHigh  = 0.0;
  double bgRightLow  = 0.0;
  double bgRightHigh = 0.0;

  bool hasSidebands = false;

};

class GF1 : public TF1 {
  public:
    GF1();
    GF1(const char* name, const char* formula,double xmin, double xmax);
    GF1(const char* name, double (*fcn)(double*,double*),double xmin,double xmax,int npar);

    GF1(const GF1&);
    virtual ~GF1();

    void Copy(TObject &obj)      const override;
    void Print(Option_t *opt="")const override;
    void Clear(Option_t *opt="") override;
    void ClearResults(); 

    double GetArea()    const { return fArea;  }
    double GetAreaErr() const { return fDArea; }
    double GetSum()     const { return fSum;   }
    double GetSumErr()  const { return fDSum;  }

    double GetChi2()    const { return fChi2;  }
    double GetNdf()     const { return fNdf;   }

    bool IsInitialized() const { return fInitialized; }
  
    


  protected:
    void SetInitialized(bool flag=true) { fInitialized = flag; }

    void SetArea(double a)       { fArea = a;   }
    void SetAreaErr(double da)   { fDArea = da; }
    void SetSum(double s)        { fSum = s;    }
    void SetSumErr(double ds)    { fDSum = ds;  }
    void SetChi2(double chi2)    { fChi2 = chi2;}
    void SetNdf(double ndf)      { fNdf = ndf;  }

    void CalStatistics(TH1 *hist, TF1 *background=nullptr,TFitResultPtr *result=nullptr);


    GFitRange fFitRange;

  private:

    double fArea  = 0.0;  // integrated area
    double fDArea = 0.0;  // error in integrated area
    double fSum   = 0.0;  // summed area
    double fDSum  = 0.0;  // error in summed area
    double fChi2  = 0.0;  // reduced chi2
    double fNdf   = 0.0;  // ndf

    bool fInitialized = false;

    


  ClassDefOverride(GF1,0)
};

#endif
