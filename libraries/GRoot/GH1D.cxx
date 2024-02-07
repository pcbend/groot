#include<GH1D.h>

#include<cstdio>

#include<TSpectrum.h>
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
  TH1D(name,title,nbinsx,xbins), fOriginal(0), fParent(0), fSubtract(0)   { 
  Init();
}
 
GH1D::GH1D(const char *name,const char *title,int nbinsx,const float  *xbins) :
  TH1D(name,title,nbinsx,xbins), fOriginal(0), fParent(0), fSubtract(0)    { 
  Init();
} 

GH1D::GH1D(const char *name,const char *title,int nbinsx,double xlow, double xhigh) :
  TH1D(name,title,nbinsx,xlow,xhigh), fOriginal(0), fParent(0), fSubtract(0)     { 
  Init();
}

//GH1D::GH1D(const GH1D &h1d);
GH1D::GH1D(const TH1D &h1d) :
  TH1D(h1d), fOriginal(0), fParent(0), fSubtract(0)     { 
  Init();
} 

GH1D::GH1D(const TH1F &h1f) :
   fOriginal(0), fParent(0), fSubtract(0)     {  
  h1f.Copy(*this); 
  Init();
} 

GH1D::GH1D(const TVectorD &v) :
  TH1D(v), fOriginal(0), fParent(0), fSubtract(0)     { 
  Init();
}

GH1D::~GH1D() { 
  //printf("GH1D deleted\n"); fflush(stdout);  
  if(fOriginal)
    delete fOriginal;
	if(fSubtract)
  	delete fSubtract;
  //TH1D::~TH1D();
} 

void GH1D::Init() { 
  //this->SetBit(kNoTitle);
	SetOriginal();
  fParent = 0;
  fSubtract = 0;
  fIsNormalized = false;
}


//void GH1D::Copy(TObject &newHist) const { 
//  TH1D::Copy(newHist);
//  (*(static_cast<GH1D*>(&newHist))).Init();
//}



void GH1D::Paint(Option_t *opt) {
  TH1D::Paint(opt);

  return;
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

void GH1D::SetOriginal()   {
	if(!fOriginal) {
  	fOriginal = new TH1D();  
    fOriginal->SetDirectory(0);
    fOriginalBins = GetNbinsX();
    dynamic_cast<TH1D*>(this)->Copy(*fOriginal);
    fOriginal->SetName(Form("_%s_copy",this->GetName()));
  }
}

void GH1D::ResetOriginal() { 

  if(!fOriginal) return;
  //double xlow = GetXaxis()->GetBinLowEdge(GetXaxis()->GetFirst());
  //double xup  = GetXaxis()->GetBinUpEdge(GetXaxis()->GetLast());
  std::string fname = this->GetName();
  //printf("Unbinng:\n");
  //printf("name: %s\n",fname.c_str());
  //printf("current  bins = %i\n",GetNbinsX());  
  //printf("original bins = %i\n\n",GetNbinsOriginal());  

  TDirectory *current = this->GetDirectory();
  fOriginal->Copy(*(dynamic_cast<TH1D*>(this)));
  this->SetName(fname.c_str());
  //printf("name2 = %s\n",this->GetName());
  this->SetDirectory(current);
  //printf("name3 = %s\n",this->GetName());
} 


TH1* GH1D::Rebin(int ngroup,const char *newname,const double *xbins) {
  TString sname(newname);
  //find the current viewing range
  //double xlow = GetXaxis()->GetBinLowEdge(GetXaxis()->GetFirst());
  //double xup  = GetXaxis()->GetBinUpEdge(GetXaxis()->GetLast());
  /*
  if(sname.Length()==0) {
    // we are not going to return a new histogram
    // so lets copy the current histogram to allow us to unbin latter...
    if(!fOriginal) {
      fOriginal = new TH1D();  
      fOriginal->SetDirectory(0);
      fOriginalBins = GetNbinsX();
      dynamic_cast<TH1D*>(this)->Copy(*fOriginal);
      fOriginal->SetName(Form("_%s_copy",this->GetName()));
      fOriginal->SetDirectory(0);
    }
  }
	*/
  TH1 *temp = TH1D::Rebin(ngroup,newname,xbins);
  //printf("Rebinng:\n");
  //printf("current  bins = %i\n",GetNbinsX());  
  //printf("original bins = %i\n\n",GetNbinsOriginal());  
  //if(sname.Length()==0) 
  //  GetXaxis()->SetRangeUser(xlow,xup);
  return temp;
}

void GH1D::Unbin(int ngroup) {
  //printf("unbin called!\n");

  if(!fOriginal) return;
  //double xlow = GetXaxis()->GetBinLowEdge(GetXaxis()->GetFirst());
  //double xup  = GetXaxis()->GetBinUpEdge(GetXaxis()->GetLast());
  //std::string fname = this->GetName();
  //printf("Unbinng:\n");
  //printf("name: %s\n",fname.c_str());
  //printf("current  bins = %i\n",GetNbinsX());  
  //printf("original bins = %i\n\n",GetNbinsOriginal());  

  int oldBins = GetNbinsX();
	
	/*
  TDirectory *current = this->GetDirectory();
  fOriginal->Copy(*(dynamic_cast<TH1D*>(this)));
  this->SetName(fname.c_str());
  //printf("name2 = %s\n",this->GetName());
  this->SetDirectory(current);
  //printf("name3 = %s\n",this->GetName());
	*/

	ResetOriginal();

  if(ngroup>0) {
    int aim = oldBins*ngroup;
    //printf("aim = %i\n",aim);
    if(aim<GetNbinsOriginal()) {
      while(GetNbinsX()>(aim+2)) {
        //printf("\tcurrent = %i\n",GetNbinsX());
        Rebin(ngroup);
      }
    }
  }
}

void GH1D::SetSubtract(TH1D *h, double scale) {
	
  if(!fOriginal) {
    fOriginal = new TH1D();  
    fOriginal->SetDirectory(0);
    fOriginalBins = GetNbinsX();
    dynamic_cast<TH1D*>(this)->Copy(*fOriginal);
    fOriginal->SetName(Form("_%s_copy",this->GetName()));
    fOriginal->SetDirectory(0);
  }


	if(fSubtract) {
		printf("DELETING SUBTRACT\n");
		fflush(stdout);
		delete fSubtract;
	}
	fSubtract = new TH1D();
	fSubtract->SetDirectory(0);
	h->Copy(*fSubtract);
	fSubtract->SetName(Form("%s_%s",this->GetName(),h->GetName()));
	fSubtract->SetDirectory(0);

	SetScale(scale);

	DoSubtract();
}

void GH1D::DoSubtract() {
	if(!fOriginal || !fSubtract)
		return;
	
  std::string fname = this->GetName();
  TDirectory *current = this->GetDirectory();
  fOriginal->Copy(*(dynamic_cast<TH1D*>(this)));
  this->SetName(fname.c_str());
  //printf("name2 = %s\n",this->GetName());
  this->SetDirectory(current);
  //printf("name3 = %s\n",this->GetName());

	this->Add(fSubtract,fScale);

}


TH1* GH1D::ShowBackground(int niter,Option_t* opt) {
  return TH1D::ShowBackground(niter,opt);
}

void GH1D::Background() {
  double x1=sqrt(-1);
  double x2=sqrt(-1);
  if(this->GetXaxis()) {
    x1 = this->GetXaxis()->GetBinLowEdge(this->GetXaxis()->GetFirst());
    x2 = this->GetXaxis()->GetBinUpEdge(this->GetXaxis()->GetLast());
  }

  if(this->TestBit(GH1D::kBackgroundRemoved)) {
    //put the background back...
    std::string fname = this->GetName();
    TDirectory *current = this->GetDirectory();
    fOriginal->Copy(*(dynamic_cast<TH1D*>(this)));
    this->SetName(fname.c_str());
    //printf("name2 = %s\n",this->GetName());
    this->SetDirectory(current);

  } else {
    //remove the background...
    if(!fOriginal) {
      fOriginal = new TH1D();  
      fOriginal->SetDirectory(0);
      fOriginalBins = GetNbinsX();
      dynamic_cast<TH1D*>(this)->Copy(*fOriginal);
      fOriginal->SetName(Form("_%s_copy",this->GetName()));
      fOriginal->SetDirectory(0);
    }
    this->GetXaxis()->UnZoom();
    TH1 *bg = TSpectrum::StaticBackground(this,12,"");
    this->Add(bg,-1); 
    this->SetBit(GH1D::kBackgroundRemoved);
  }
  if(x1==x1) {
    //printf("%s I AM HERE!\n",GetName());
    this->GetXaxis()->SetRangeUser(x1,x2);
  }
}

void GH1D::Normalize() {
  
  if(!fIsNormalized) {

    double sum = GetSumOfWeights();
    if(sum==0) {
      printf("GH1D::Normalize sum of weights is zero\n");
      return;
    }
    double max = GetMaximum();
    double min = GetMinimum();
    this->Scale(1.0/sum);
    if (TMath::Abs(max+1111) > 1e-3) SetMaximum(max*1.0/sum);
    if (TMath::Abs(min+1111) > 1e-3) SetMinimum(min*1.0/sum);
    Sumw2(false);  
    fIsNormalized = true;
  } else if(fIsNormalized) {
    std::string fname = this->GetName();
    fOriginal->Copy(*(dynamic_cast<TH1D*>(this)));
    this->SetName(fname.c_str());
    fIsNormalized = false;
  }

}






