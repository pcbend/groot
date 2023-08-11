#ifndef __GH1D_H__
#define __GH1D_H__

#include<string>

#include<TH1D.h>

class GH1D : public TH1D {
  public:
    GH1D();
    GH1D(std::string name,int nbinsx,const double *xbins);
    GH1D(std::string name,int nbinsx,const float  *xbins);
    GH1D(std::string name,int nbinsx,double xlow, double xhigh);

    GH1D(const char *name,const char *title,int nbinsx,const double *xbins);
    GH1D(const char *name,const char *title,int nbinsx,const float  *xbins);
    GH1D(const char *name,const char *title,int nbinsx,double xlow, double xhigh);

    //GH1D(const GH1D &h1d);
    GH1D(const TH1D &h1d);
    GH1D(const TVectorD &v);
    
    virtual ~GH1D(); 


  private:

  ClassDef(GH1D,100)
};

#endif
