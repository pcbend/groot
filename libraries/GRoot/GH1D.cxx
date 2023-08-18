#include<GH1D.h>

#include<cstdio>


#include<TString.h>
#include<TVirtualPad.h>
#include<TBox.h>

GH1D::GH1D() : TH1D(), fOriginal(0) { }

GH1D::GH1D(std::string name,int nbinsx,const double *xbins) :
  GH1D(name.c_str(),name.c_str(),nbinsx,xbins)  { }

GH1D::GH1D(std::string name,int nbinsx,const float  *xbins) :
  GH1D(name.c_str(),name.c_str(),nbinsx,xbins)  { }

GH1D::GH1D(std::string name,int nbinsx,double xlow, double xhigh) : 
  GH1D(name.c_str(),name.c_str(),nbinsx,xlow,xhigh)  { }

GH1D::GH1D(const char *name,const char *title,int nbinsx,const double *xbins) :
  TH1D(name,title,nbinsx,xbins), fOriginal(0)   { }
 
GH1D::GH1D(const char *name,const char *title,int nbinsx,const float  *xbins) :
  TH1D(name,title,nbinsx,xbins), fOriginal(0)   { } 

GH1D::GH1D(const char *name,const char *title,int nbinsx,double xlow, double xhigh) :
  TH1D(name,title,nbinsx,xlow,xhigh), fOriginal(0)   { }

//GH1D::GH1D(const GH1D &h1d);
GH1D::GH1D(const TH1D &h1d) :
  TH1D(h1d), fOriginal(0)   { } 

GH1D::GH1D(const TVectorD &v) :
  TH1D(v), fOriginal(0)   { }

GH1D::~GH1D() { 
  //printf("GH1D deleted\n"); fflush(stdout);  
  if(fOriginal)
    delete fOriginal;
} 

void GH1D::Paint(Option_t *opt) {
  //printf("\t-in gh1d paint.\n");
  //fflush(stdout);
  TString sopt(opt);
  bool drawFunctions=false;
  if(!sopt.Length()) {
    if(this->GetSumw2()->GetSize()) {
      sopt.Append("hist");
      drawFunctions=true;
    }
  }
  TH1D::Paint(sopt.Data());
  if(drawFunctions) {
    TIter iter(this->GetListOfFunctions());
    while(TObject *obj = iter.Next()) {
      //if(obj->InheritsFrom(TF1::Class() || TLine::Class())
      if((obj->InheritsFrom("TF1")) || (obj->InheritsFrom("TLine")))
        obj->Paint("lsame");
    }
  }
}

void GH1D::Draw(Option_t *opt) {
  //printf("GH1D Draw!\n"); fflush(stdout);
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
  //printf("GH1D Draw!\n"); fflush(stdout);
  return TH1D::DrawCopy(opt,name_postfix);
}

TH1* GH1D::DrawNormalized(Option_t *opt, double norm) const {
  //printf("GH1D Draw!\n"); fflush(stdout);
  return TH1D::DrawNormalized(opt,norm);
} 


TH1* GH1D::Rebin(int ngroup,const char *newname,const double *xbins) {
  TString sname(newname);
  //find the current viewing range
  double xlow = GetXaxis()->FindBinLowEdge(GetXaxis()->GetFirst());
  double xup  = GetXaxis()->FindBinUpEdge(GetXaxis()->GetLast());

  if(sname.Length()==0) {
    // we are not going to return a new histogram
    // so lets copy the current histogram to allow us to unbin latter...
    if(!fOriginal) {
      fOriginal = new TH1D();  
      fOriginal->SetDirectory(0);
    }
    dynamic_cast<TH1D*>(this)->Copy(*fOriginal);
    fOriginal->SetName(Form("_%s_copy",this->GetName()));
    fOriginal->SetDirectory(0);
  }
  TH1 *temp = TH1D::Rebin(ngroup,newname,xbins);
  if(sname.Length()==0) 
    GetXaxis()->SetRangeUser(xlow,xup);
  return temp;
}

void GH1D::Unbin() {
  if(!fArray) return;
  double xlow = GetXaxis()->FindBinLowEdge(GetXaxis()->GetFirst());
  double xup  = GetXaxis()->FindBinUpEdge(GetXaxis()->GetLast());
  const char *fname = this->GetName();
  TDirectory *current = this->GetDirectory();
  fOriginal->Copy(*(dynamic_cast<TH1D*>(this)));
  this->SetName(fname);
  this->SetDirectory(current);
  if(gPad && gPad->GetListOfPrimitives()->FindObject(this->GetName())) {
    gPad->Modified();
    gPad->Update();
  }
  GetXaxis()->SetRangeUser(xlow,xup);
}

