
#include<GH2D.h>
#include<GH1D.h>

#include <TPad.h>

#include <GEventHandler.h>

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

GH2D::GH2D(const TH2F &h2f) { h2f.Copy(*this); } 

GH2D::GH2D(const TH2 *h2) { if(h2) h2->Copy(*this); }  

GH2D::~GH2D() { 
  if(fEventHandler) delete fEventHandler;
}

void GH2D::Init() {
  fEventHandler = new GH2DEventHandler(this); 
}

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

GH1D* GH2D::ProjectionX(double low,double up,Option_t *option) { 
  GH1D* projection=0;
  if(low>up) std::swap(low,up);
  //current x-range ;
  double first = GetXaxis()->GetBinLowEdge(GetXaxis()->GetFirst());
  double last  = GetXaxis()->GetBinUpEdge(GetXaxis()->GetLast());
 
  //unzoom y - otherwise the projection will be truncated.
  if(gPad) GetXaxis()->UnZoom();

  int blow,bup;
  if(low!=low) 
    blow = GetYaxis()->GetFirst();
  else 
    blow = GetYaxis()->FindBin(low);
  if(up!=up) 
    bup  = GetYaxis()->GetLast();
  else
    bup  = GetYaxis()->FindBin(up);

  std::string pname = Form("%s_x_%i_%i",GetName(),blow,bup);
  projection = new GH1D(*(dynamic_cast<TH2D*>(this)->ProjectionX(pname.c_str(),blow,bup)));
  projection->SetParent(this);  
  projection->SetBit(GH1D::kProjectionX,true);

  //reset x axis.
  GetXaxis()->SetRangeUser(first,last);        
  projection->GetXaxis()->SetRangeUser(first,last);        

  return projection;
}    

GH1D* GH2D::ProjectionY(double low,double up,Option_t *option) { 
  GH1D* projection=0;
  if(low>up) std::swap(low,up);
  //current y-range ;
  double first = GetYaxis()->GetBinLowEdge(GetYaxis()->GetFirst());
  double last  = GetYaxis()->GetBinUpEdge(GetYaxis()->GetLast());
 
  //unzoom y - otherwise the projection will be truncated.
  if(gPad) GetYaxis()->UnZoom();

  int blow,bup;
  if(low!=low) 
    blow = GetXaxis()->GetFirst();
  else 
    blow = GetXaxis()->FindBin(low);
  if(up!=up) 
    bup  = GetXaxis()->GetLast();
  else
    bup  = GetXaxis()->FindBin(up);

  std::string pname = Form("%s_y_%i_%i",GetName(),blow,bup);
  projection = new GH1D(*(dynamic_cast<TH2D*>(this)->ProjectionY(pname.c_str(),blow,bup)));
  projection->SetParent(this);  
  projection->SetBit(GH1D::kProjectionX,false);

  //reset y axis.
  GetYaxis()->SetRangeUser(first,last);        
  projection->GetXaxis()->SetRangeUser(first,last);        

  return projection;
} 



