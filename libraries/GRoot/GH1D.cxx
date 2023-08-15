#include<GH1D.h>

#include<cstdio>

#include<TVirtualPad.h>
#include<TBox.h>

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

GH1D::~GH1D() { 
  //printf("GH1D deleted\n"); fflush(stdout);  
} 


void GH1D::Draw(Option_t *opt) {
  printf("GH1D Draw!\n"); fflush(stdout);
  TH1D::Draw(opt);
  if(gPad) {
    //gPad->Modified(); 
    gPad->Update();
    TBox *frame = (TBox*)gPad->GetListOfPrimitives()->FindObject("TFrame");
    if(frame)
      frame->SetBit(TBox::kCannotMove); 
  }
  return; 
}

TH1* GH1D::DrawCopy(Option_t *opt, const char *name_postfix) const {
  printf("GH1D Draw!\n"); fflush(stdout);
  return TH1D::DrawCopy(opt,name_postfix);
}

TH1* GH1D::DrawNormalized(Option_t *opt, double norm) const {
  printf("GH1D Draw!\n"); fflush(stdout);
  return TH1D::DrawNormalized(opt,norm);
} 



