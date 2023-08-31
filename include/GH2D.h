#ifndef __GH2D_H__
#define __GH2D_H__

#include<TH2D.h>

class GH1D;

class GH2D : public TH2D {
  public:
    GH2D();
    GH2D(std::string name,int nbinsx,double xlow,double xup
                         ,int nbinsy,double ylow,double yup);
    GH2D(std::string name,int nbinsx,const double *xbins
                         ,int nbinsy,double ylow,double yup);
    GH2D(std::string name,int nbinsx,double xlow,double xup
                         ,int nbinsy,const double *ybins);
    GH2D(std::string name,int nbinsx,const double *xbins
                         ,int nbinsy,const double *ybins);
    GH2D(std::string name,int nbinsx,const float  *xbins
                         ,int nbinsy,const float  *ybins);
    
    GH2D(const char *name,const char *title,int nbinsx,double xlow,double xup
                                           ,int nbinsy,double ylow,double yup);
    GH2D(const char *name,const char *title,int nbinsx,const double *xbins
                                           ,int nbinsy,double ylow,double yup);
    GH2D(const char *name,const char *title,int nbinsx,double xlow,double xup
                                           ,int nbinsy,const double *ybins);
    GH2D(const char *name,const char *title,int nbinsx,const double *xbins
                                           ,int nbinsy,const double *ybins);
    GH2D(const char *name,const char *title,int nbinsx,const float  *xbins
                                           ,int nbinsy,const float  *ybins);
    
    GH2D(const TMatrixDBase &m);
    //GH2D(const GH2D &h2d);
    GH2D(const TH2D &h2d);
    virtual ~GH2D();
    
    
    void Draw(Option_t *opt="") override;
    TH1* DrawCopy(Option_t *opt="", const char *name_postfix="_copy") const override; 
    TH1* DrawNormalized(Option_t *opt="", double norm=1) const override; 
    
    void Paint(Option_t *opt="") override;

    //TH1D* ProjectionX(const char *name="_px",int firstybin=0,int lastybin=-1, Option_t *option="") const;    
    //TH1D* ProjectionY(const char *name="_py",int firstybin=0,int lastybin=-1, Option_t *option="") const;    
    GH1D* ProjectionX(double low=sqrt(-1),double high=sqrt(-1), Option_t *option="");    
    GH1D* ProjectionY(double low=sqrt(-1),double high=sqrt(-1), Option_t *option="");    


  private:
    

  ClassDefOverride(GH2D,100)
};


#endif
