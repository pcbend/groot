
#include<GH2D.h>

GH2D::GH2D() : TH2D() { }
GH2D::GH2D(std::string name,int nbinsx,double xlow,double xup
                        ,int nbinsy,double ylow,double yup) :
      GH2D(name.c_str(),name.c_str(),nbinsx,xlow,xup,nbinsy,ylow,yup) { }

GH2D::GH2D(std::string name,int nbinsx,const double *xbins
                        ,int nbinsy,double ylow,double yup) : 
      GH2D(name.c_str(),name.c_str(),nbinsx,xbins,nbinsy,ylow,yup) { }

GH2D::GH2D(std::string name,int nbinsx,double xlow,double xup
                        ,int nbinsy,const double *ybins) : 
      GH2D(name.c_str(),name.c_str(),nbinsx,xlow,xup,nbinsy,ybins) { }

GH2D::GH2D(std::string name,int nbinsx,const double *xbins
                        ,int nbinsy,const double *ybins) : 
      GH2D(name.c_str(),name.c_str(),nbinsx,xbins,nbinsy,ybins) { }

GH2D::GH2D(std::string name,int nbinsx,const float  *xbins
                        ,int nbinsy,const float  *ybins) : 
      GH2D(name.c_str(),name.c_str(),nbinsx,xbins,nbinsy,ybins) { }

GH2D::GH2D(const char *name,const char *title,int nbinsx,double xlow,double xup
                                          ,int nbinsy,double ylow,double yup) :
      TH2D(name,title,nbinsx,xlow,xup,nbinsy,ylow,yup) { }

GH2D::GH2D(const char *name,const char *title,int nbinsx,const double *xbins
                                          ,int nbinsy,double ylow,double yup) : 
      TH2D(name,title,nbinsx,xbins,nbinsy,ylow,yup) { }

GH2D::GH2D(const char *name,const char *title,int nbinsx,double xlow,double xup
                                          ,int nbinsy,const double *ybins) : 
      TH2D(name,title,nbinsx,xlow,xup,nbinsy,ybins) { }

GH2D::GH2D(const char *name,const char *title,int nbinsx,const double *xbins
                                          ,int nbinsy,const double *ybins) : 
      TH2D(name,title,nbinsx,xbins,nbinsy,ybins) { }

GH2D::GH2D(const char *name,const char *title,int nbinsx,const float  *xbins
                                          ,int nbinsy,const float  *ybins) : 
      TH2D(name,title,nbinsx,xbins,nbinsy,ybins) { }

GH2D::GH2D(const TMatrixDBase &m) : TH2D(m) { }
 
//GH2D::GH2D(const GH2D &h2d);
GH2D::GH2D(const TH2D &h2d) : TH2D(h2d) { } 

GH2D::~GH2D() { }



void GH2D::Draw(Option_t *opt) {
  TH2D::Draw(opt);
} 

TH1* GH2D::DrawCopy(Option_t *opt, const char *name_postfix) const {
  return TH2D::DrawCopy(opt,name_postfix);
} 

TH1* GH2D::DrawNormalized(Option_t *opt, double norm) const {
  return TH2D::DrawNormalized(opt,norm);
} 

void GH2D::Paint(Option_t *opt) {
  TString sopt(opt);
  if(sopt.Length()==0)  
    sopt.Append("colz");
  TH2D::Paint(sopt.Data());
  return;
}




