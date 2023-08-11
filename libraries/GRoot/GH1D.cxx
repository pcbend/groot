#include<GH1D.h>

GH1D::GH1D() : TH1D() { }

GH1D::GH1D(std::string name,int nbinsx,const double *xbins) :
  GH1D(name.c_str(),name.c_str(),nbinsx,xbins) { }

GH1D::GH1D(std::string name,int nbinsx,const float  *xbins) :
  GH1D(name.c_str(),name.c_str(),nbinsx,xbins) { }

GH1D::GH1D(std::string name,int nbinsx,double xlow, double xhigh) : 
  GH1D(name.c_str(),name.c_str(),nbinsx,xlow,xhigh) { }

GH1D::GH1D(const char *name,const char *title,int nbinsx,const double *xbins) :
  TH1D(name,title,nbinsx,xbins) { }
 
GH1D::GH1D(const char *name,const char *title,int nbinsx,const float  *xbins) :
  TH1D(name,title,nbinsx,xbins) { } 

GH1D::GH1D(const char *name,const char *title,int nbinsx,double xlow, double xhigh) :
  TH1D(name,title,nbinsx,xlow,xhigh) { }

//GH1D::GH1D(const GH1D &h1d);
GH1D::GH1D(const TH1D &h1d) :
  TH1D(h1d) { } 

GH1D::GH1D(const TVectorD &v) :
  TH1D(v) { }

GH1D::~GH1D() { } 

