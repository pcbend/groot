
#include<GH2D.h>
#include<GH1D.h>

#include<TVirtualPad.h>

#include<TSpectrum2.h>


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



struct TH2Data {
  int nx{0};
  int ny{0};
  double*  data{nullptr};
  double** row{nullptr};
  
  TH2Data(TH2D* h) { 
    //if(h->GetDimensions()!=2) return;
    const int nx = h->GetNbinsX();
    const int ny = h->GetNbinsY();
    alloc(nx,ny);
    for(int iy=0;iy<ny;iy++) 
      for(int ix=0;ix<nx;ix++) 
        row[iy][ix] = h->GetBinContent(ix+1,iy+1);
  }
  ~TH2Data() { clear(); }

  void alloc(int NX,int NY) {
    clear();
    nx = NX;
    ny = NY;
    data = new double[nx*ny];
    row  = new double*[ny];
    for(int y=0;y<ny;y++) row[y] = data+y*nx;
  }
  void clear() {
    delete[] row;  row  = nullptr;
    delete[] data; data = nullptr;
    nx=0;
    ny=0;
  }
  void smooth(int passes=1) {
    if(passes<1 || !data) return;
    std::vector<double> A(data,data+nx*ny);
    std::vector<double> B(nx*ny);
    std::vector<double> win; win.reserve(9);
    auto idx = [this](int x,int y){ return (size_t)y*(size_t)nx + (size_t)x; };

    for(int p=0;p<passes;p++) {
      for(int y=0;y<ny;y++) {
        int y0 = (y>0?y-1:0);
        int y1 = y;
        int y2 = (y<ny-1?y+1:ny-1);
        for(int x=0;x<nx;x++) { 
          int x0 = (x>0?x-1:0);
          int x1 = x;
          int x2 = (x<nx-1?x+1:nx-1);
          win.clear();
          auto push = [&](int X,int Y){
            double v = A[idx(X,Y)];                 // fetch neighbor value
            if ( v != 0.0)           // optionally skip zeros
              win.push_back(v);                     // append to the window vector
          };
          push(x0,y0); push(x1,y0); push(x2,y0);
          push(x0,y1); push(x1,y1); push(x2,y1);
          push(x0,y2); push(x1,y2); push(x2,y2);
          if (win.empty()) { 
            B[idx(x,y)] = 0.0; 
          } else {
            int mid = win.size()/2;
            std::nth_element(win.begin(), win.begin()+mid, win.end());
            B[idx(x,y)] = win[mid];
          }
        }
      }
      A.swap(B);
    }
    std::copy(A.begin(),A.end(),data); 
  }

  double* operator[](int y) { return row[y]; }
  const double* operator[](int y) const { return row[y]; }
};



GH2D *GH2D::GetBackground(int nIterX,int nIterY,bool doSmoothing) const {
  GH2D* h2 = (GH2D*)Clone(Form("%s_clone",GetName()));  
  h2->SetDirectory(nullptr);
  h2->Sumw2(kTRUE);

  GH2D* h2_bg = (GH2D*)h2->Clone(Form("%s_background",GetName())); 
  h2_bg->SetDirectory(nullptr);

  TSpectrum2 spec;
  TH2Data data(h2_bg);
  const char* msg = spec.Background(data.row,GetNbinsX(),GetNbinsY(),nIterX,nIterY,0,0);
  if(msg) printf("msg = %s\n",msg);

  data.smooth();

  h2_bg->Reset();
  for(int x=0;x<GetNbinsX();x++) 
    for(int y=0;y<GetNbinsY();y++) 
      h2_bg->SetBinContent(x+1,y+1,data[y][x]);


  return h2_bg;
}




void GH2D::Paint(Option_t *opt) {
  TString sopt(opt);
  if(sopt.Length()==0)  
    sopt.Append("colz2");
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
  if(gPad)
    GetXaxis()->UnZoom();

  int blow,bup;
  if(low!=low) 
    blow = GetYaxis()->GetFirst();
  else 
    blow = GetYaxis()->FindBin(low);
  if(up!=up) 
    bup  = GetYaxis()->GetLast();
  else
    bup  = GetYaxis()->FindBin(up);

  std::string pname      = Form("%s_x_%i_%i",GetName(),blow,bup);
  std::string pname_temp = Form("%s_x_%i_%i_temp",GetName(),blow,bup);
  //projection = new GH1D(*(dynamic_cast<TH2D*>(this)->ProjectionX(pname.c_str(),blow,bup)));
  TH1D* temp = TH2D::ProjectionX(pname_temp.c_str(),blow,bup);
  projection = new GH1D(*temp);
  projection->SetNameTitle(pname.c_str(),pname.c_str());
  temp->Delete();
  projection->SetParent(this);  
  projection->SetBit(GH1D::kProjectionX,true);

  if(gPad) {
  //reset x axis.
    GetXaxis()->SetRangeUser(first,last);        
    projection->GetXaxis()->SetRangeUser(first,last);        
  }
  fProjections.Add(projection);
  return projection;
}    

GH1D* GH2D::ProjectionX(double low, double up, double bg_low, double bg_high, Option_t *opt) {
  int blow,bup,bbglow,bbgup;
  blow = GetYaxis()->FindBin(low);
  bup  = GetYaxis()->FindBin(up);
  bbglow = GetYaxis()->FindBin(bg_low);
  bbgup  = GetYaxis()->FindBin(bg_high);
  GH1D *add = ProjectionX(low,up,opt);
  GH1D *sub = ProjectionX(bg_low,bg_high,opt);
  
  std::string pname = Form("%s_x_%i_%i_%i_%i",GetName(),blow,bup,bbglow,bbgup);
  add->SetNameTitle(pname.c_str(),pname.c_str());
  add->Add(sub,-1);

  return add;
}




GH1D* GH2D::ProjectionY(double low,double up,Option_t *option) { 
  GH1D* projection=0;
  if(low>up) std::swap(low,up);
  //current y-range ;
  double first = GetYaxis()->GetBinLowEdge(GetYaxis()->GetFirst());
  double last  = GetYaxis()->GetBinUpEdge(GetYaxis()->GetLast());
 
  //unzoom y - otherwise the projection will be truncated.
  GetYaxis()->UnZoom();

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
  fProjections.Add(projection);

  return projection;
} 

GH1D* GH2D::ProjectionY(double low, double up, double bg_low, double bg_high, Option_t *opt) {

  int blow,bup,bbglow,bbgup;
  blow = GetXaxis()->FindBin(low);
  bup  = GetXaxis()->FindBin(up);
  bbglow = GetXaxis()->FindBin(bg_low);
  bbgup  = GetXaxis()->FindBin(bg_high);

  GH1D *add = ProjectionY(low,up,opt);
  GH1D *sub = ProjectionY(bg_low,bg_high,opt);
  
  std::string pname = Form("%s_y_%i_%i_%i_%i",GetName(),blow,bup,bbglow,bbgup);
  add->SetNameTitle(pname.c_str(),pname.c_str());
  add->Add(sub,-1);

  return add;
}



GH1D* GH2D::Next(const GH1D* current) const { 
  printf("in Next, current = %p\n",current);

 if(!current)
  return 0;
 if(fProjections.GetEntries()==0)
   return 0;
 TObject *obj = fProjections.FindObject(current);
 if(!obj) 
   return 0;
 obj = fProjections.After(current);
 if(!obj)
   obj = fProjections.First();
 return dynamic_cast<GH1D*>(obj); 

}


GH1D* GH2D::Previous(const GH1D* current) const {

  if(!current)
    return 0;

 if(fProjections.GetEntries()==0)
   return 0;
 TObject *obj = fProjections.FindObject(current);
 if(!obj) 
   return 0;
 obj = fProjections.Before(current);
 if(!obj)
   obj = fProjections.Last();
 return dynamic_cast<GH1D*>(obj); 


  }

