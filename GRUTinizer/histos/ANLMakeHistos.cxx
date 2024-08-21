
#include "TRuntimeObjects.h"

#include <iostream>
#include <map>
#include <cstdio>

#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TRandom.h>
#include <TObject.h>

#include "TGretina.h"
#include "TBank88.h"

#include "TChannel.h"
#include "GValue.h"
#include "GCutG.h"


#define Q1 15
#define Q2 7
#define Q3 8
#define Q4 16
#define Q5 9
#define Q6 14
#define Q7 17
#define Q8 6
#define Q9 19

//#define BETA .37

std::map<int,int> HoleQMap;
std::map<int,std::string> LayerMap;

void InitMap() {
  HoleQMap[Q1] = 1;
  HoleQMap[Q2] = 2;
  HoleQMap[Q3] = 3;
  HoleQMap[Q4] = 4;
  HoleQMap[Q5] = 5;
  HoleQMap[Q6] = 6;
  HoleQMap[Q7] = 7;
  HoleQMap[Q8] = 8;
  HoleQMap[Q9] = 9;

  LayerMap[0] = "alpha";
  LayerMap[1] = "beta";
  LayerMap[2] = "gamma";
  LayerMap[3] = "delta";
  LayerMap[4] = "epsilon";
  LayerMap[5] = "phi";

}



class FMA {
  public:
    FMA() { Clear(); }
    ~FMA() {} 

    double dE() { return ic1; }
    double E()  { return ic1+ic2+ic3; }
    double X()  { return (mcpl/(mcpl+mcpr))*1000.;}
    double Y()  { return (mcpr/(mcpl+mcpr))*1000.;}
    double Time()  { return timestamp;}

    void Set(TBank88 *bank) {
      for(size_t i=0;i<bank->Size();i++) {
        int id = bank->GetMode3Hit(i).GetChannel();
        double value = bank->GetMode3Hit(i).Charge();
        double time = bank->GetMode3Hit(i).GetLed();
        switch(id) {
          case 0:
            mcpl = value;
            break;
          case 1:
            mcpr = value;
            break;
          case 2:
            wobbler = value;
            break;
          case 5:
            ic1 = value;
            break;
          case 6:
            ic2 = value;
            break;
          case 7:
            rfTof = value;
            break;
          case 8:
            ic3 = value;
            break;
          case 9:
            particleTrigger = value;
            timestamp = time;
            break;
        }
      }
    }

    void Clear() {
      mcpl = -1;
      mcpr = -1;
      wobbler = -1;
      ic1 = -1;
      ic2 = -1;
      ic3 = -1;
      rfTof = -1;
      particleTrigger = -1;
      timestamp = -1;
    }

    double mcpl;
    double mcpr;
    double wobbler;
    double ic1;
    double ic2;
    double ic3;
    double rfTof;
    double particleTrigger;
    double timestamp;
};

TCutG *gg_time=0;
TCutG *te=0;
TCutG *Ne25_xe=0;
TCutG *Ne25n_xe=0;
TCutG *Ne25w_xe=0;
TCutG *NeAll25w_xe=0;


#define INTEGRATION 128.0

// extern "C" is needed to prevent name mangling.
// The function signature must be exactly as shown here,
//   or else bad things will happen.
extern "C"
void MakeHistograms(TRuntimeObjects& obj) {
  InitMap();
  TGretina *gretina = obj.GetDetector<TGretina>();
  TBank88  *bank88  = obj.GetDetector<TBank88>();

  FMA fma;

  TList *list = &(obj.GetObjects());
  int numobj = list->GetSize();

  //double BETA = 0.0463;      //= GValue::Value("BETA");
  double BETA = 0.050;       //= GValue::Value("BETA");
  double GAMMA = 1./sqrt(1. - BETA*BETA);

  TList *gates = &(obj.GetGates());
  if(!gg_time) gg_time = (TCutG*)gates->FindObject("gg_timing");
  if(!te) te = (TCutG*)gates->FindObject("timeenergy");
  if(!Ne25_xe) Ne25_xe = (TCutG*)gates->FindObject("Ne25XE");
  if(!Ne25n_xe) Ne25n_xe = (TCutG*)gates->FindObject("Ne25XEn");
  if(!Ne25w_xe) Ne25w_xe = (TCutG*)gates->FindObject("Ne25XEw");
  if(!NeAll25w_xe) NeAll25w_xe = (TCutG*)gates->FindObject("NeAll25XEw");

  //GCutG *ne_all = (GCutG*)gates->FindObject("NeAll");
  //GCutG *ne25 = (GCutG*)gates->FindObject("Ne25");
  //GCutG *ne22 = (GCutG*)gates->FindObject("Ne22");
  //GCutG *z1 = (GCutG*)gates->FindObject("z1");



  if(bank88) {
    fma.Set(bank88);
    obj.FillHistogram("pid",2000,0,10000,fma.E(),2000,0,4000,fma.dE());
    obj.FillHistogram("x_position",1000,0,1000,fma.X());
    obj.FillHistogram("x_de",1000,0,1000,fma.X(),2000,0,4000,fma.dE());
    obj.FillHistogram("x_e",1000,0,1000,fma.X(),2000,0,10000,fma.E());
    //for(unsigned int x=0;x<bank88->Size();x++) {
    //  TMode3Hit &hit = (TMode3Hit&)bank88->GetHit(x);
    //  std::string histname = Form("bank88_%i",hit.GetChannel());
    //  obj.FillHistogram(histname,16000,0,64000,hit.Charge());
    //}
  }

  if(!gretina)
    return;

  std::vector<TGretinaHit> hits;
  double calorimeterEnergy = 0.0;

  for(unsigned int y=0;y<gretina->Size();y++) {
    TGretinaHit hit = gretina->GetGretinaHit(y);
    std::string histname;
    
    double dt = fma.Time()-hit.GetTime();

    obj.FillHistogram("dt_e",2000,-1000,1000,dt,2000,0,10000,fma.E());
    obj.FillHistogram("g_fma_time",2000,-1000,1000,dt);
    obj.FillHistogram("g_fma_time_2d",2000,-1000,1000,dt,
        8192,0,8192,hit.GetCoreEnergy(3));

    histname = Form("GretinaEnergyTheta");
    obj.FillHistogram("ungated",histname,8192,0,8192,hit.GetCoreEnergy(3),
        180,0,TMath::Pi(),hit.GetTheta());

    histname = Form("SummaryEnergy");
    obj.FillHistogram("ungated",histname,8192,0,8192,hit.GetCoreEnergy(3),
        200,0,200,hit.GetCrystalId());

    histname = Form("SummaryDoppler");
    obj.FillHistogram("ungated",histname,8192,0,8192,hit.GetDopplerANL(BETA),
        200,0,200,hit.GetCrystalId());

    histname = Form("Energy%02i",hit.GetCrystalId());
    obj.FillHistogram("xtal",histname,8192,0,8192,hit.GetCoreEnergy(3));

    histname = Form("GretinaPosition");
    obj.FillHistogram("ungated",histname,360,0,2.*TMath::Pi(),hit.GetPhi(),
        180,0,TMath::Pi(),hit.GetTheta());

    histname = "GretinaEnergyTotal";
    obj.FillHistogram("ungated",histname,8192,0,8192,hit.GetCoreEnergy(3));

    histname = "GretinaDopplerTotal";
    obj.FillHistogram("ungated",histname,8192,0,8192,hit.GetDopplerANL(BETA));
    obj.FillHistogram("ungated","GretinaDopplerTotal_0.0415",8192,0,8192,hit.GetDopplerANL(0.0415));
    //double b_ug=0.040;
    //for(int i=0; i<200; i++){
      //obj.FillHistogram("ungated","GretinaDoppler_beta",200,0.040,0.060,b_ug,8192,0,8192,hit.GetDopplerANL(b_ug));
      //b_ug+=0.0001;
    //}

    //if(dt<110 || dt>170) continue;
    if(te->IsInside(dt,hit.GetCoreEnergy(3)) == 0){
      obj.FillHistogram("ungated","GretinaDoppler_outside_dt_prompt",8192,0,8192,hit.GetDopplerANL(BETA));
      obj.FillHistogram("ungated","GretinaDoppler_outside_dt_prompt_0.0415",8192,0,8192,hit.GetDopplerANL(0.0415));
      //double b_ug2=0.040;
      //for(int i=0; i<200; i++){
        //obj.FillHistogram("ungated","GretinaDoppler_outside_dt_prompt_beta",200,0.040,0.060,b_ug2,8192,0,8192,hit.GetDopplerANL(b_ug2));
        //b_ug2+=0.0001;
      //}
    }
    if(te->IsInside(dt,hit.GetCoreEnergy(3)) == 0) continue;

    obj.FillHistogram("ungated","GretinaDoppler_inside_dt_prompt",8192,0,8192,hit.GetDopplerANL(BETA));
    obj.FillHistogram("ungated","GretinaDoppler_inside_dt_prompt_0.0415",8192,0,8192,hit.GetDopplerANL(0.0415));
    //double b_ug3=0.040;
    //for(int i=0; i<200; i++){
      //obj.FillHistogram("ungated","GretinaDoppler_inside_dt_prompt_beta",200,0.040,0.060,b_ug3,8192,0,8192,hit.GetDopplerANL(b_ug3));
      //b_ug3+=0.0001;
    //}

    //Addback preprocessing (note: only taking hits in time-energy cut)
    if(hit.GetCoreEnergy(3) > 5 && hit.GetCoreEnergy(3) < 8192){
      calorimeterEnergy += hit.GetCoreEnergy(3);
      hits.push_back(hit);
    }

    obj.FillHistogram("doppler_X",1000,0,1000,fma.X(),
        8192,0,8192,hit.GetDopplerANL(BETA));

    obj.FillHistogram("mcp",1000,0,2000,fma.mcpl,1000,0,2000,fma.mcpr);
    obj.FillHistogram("rfTof",1000,0,2000,fma.rfTof);


    for(unsigned int z=y+1;z<gretina->Size();z++) {
        TGretinaHit hit2 = gretina->GetGretinaHit(z);
        double dt2 = fma.Time()-hit2.GetTime();
        //if(dt2<110 || dt2>170) continue;
        if(te->IsInside(dt2,hit2.GetCoreEnergy(3)) == 0) continue;
        if(hit.GetCoreEnergy(3) > hit2.GetCoreEnergy(3)) {
          obj.FillHistogram("gdt",2000,-1000,1000,hit.GetTime()-hit2.GetTime(),
              8192,0,8192,hit2.GetDopplerANL(BETA));
          obj.FillHistogramSym("gg",8192,0,8192,hit.GetDopplerANL(BETA),
              8192,0,8192,hit2.GetDopplerANL(BETA));
          if(gg_time && gg_time->IsInside(hit.GetTime()-hit2.GetTime(),hit2.GetDopplerANL(BETA)) && gg_time->IsInside(hit2.GetTime()-hit.GetTime(),hit.GetDopplerANL(BETA))){
            obj.FillHistogram("gdt_gated",2000,-1000,1000,hit.GetTime()-hit2.GetTime(),
                8192,0,8192,hit2.GetDopplerANL(BETA));
            obj.FillHistogramSym("gg_gdtgated",8192,0,8192,hit.GetDopplerANL(BETA),
                8192,0,8192,hit2.GetDopplerANL(BETA));
          }
        }
        if(hit2.GetCoreEnergy(3) > hit.GetCoreEnergy(3)) {
          obj.FillHistogram("gdt",2000,-1000,1000,hit2.GetTime()-hit.GetTime(),
              8192,0,8192,hit.GetDopplerANL(BETA));
          obj.FillHistogramSym("gg",8192,0,8192,hit.GetDopplerANL(BETA),
              8192,0,8192,hit2.GetDopplerANL(BETA));
          if(gg_time && gg_time->IsInside(hit2.GetTime()-hit.GetTime(),hit.GetDopplerANL(BETA)) && gg_time->IsInside(hit.GetTime()-hit2.GetTime(),hit2.GetDopplerANL(BETA))){
            obj.FillHistogram("gdt_gated",2000,-1000,1000,hit2.GetTime()-hit.GetTime(),
                8192,0,8192,hit.GetDopplerANL(BETA));
            obj.FillHistogramSym("gg_gdtgated",8192,0,8192,hit.GetDopplerANL(BETA),
                8192,0,8192,hit2.GetDopplerANL(BETA));
          }
        }
      }



    TIter iter(gates);
    while(TObject *gobj = iter.Next()) {

      GCutG *cut = (GCutG*)gobj;
      if(strcmp(cut->GetTag(),"pid")!=0) continue;
      if(strcmp(cut->GetName(),"Ne25")==0 && Ne25_xe->IsInside(fma.X(),fma.E())==0) continue;
      if(strcmp(cut->GetName(),"Ne25n")==0 && Ne25n_xe->IsInside(fma.X(),fma.E())==0) continue;
      if(strcmp(cut->GetName(),"Ne25w")==0 && Ne25w_xe->IsInside(fma.X(),fma.E())==0) continue;
      if(strcmp(cut->GetName(),"NeAll25")==0 && NeAll25w_xe->IsInside(fma.X(),fma.E())==0) continue;
      if(cut->IsInside(fma.E(),fma.dE())) {
        if(strcmp(cut->GetName(),"Mg25")==0){
          BETA = 0.048;
        }
        if(strcmp(cut->GetName(),"Na25")==0){
          BETA = 0.047;
        }
        if(strcmp(cut->GetName(),"Ne22")==0){
          BETA = 0.0436;
        }
        if(strcmp(cut->GetName(),"F22")==0){
          BETA = 0.059;
        }
        if(strcmp(cut->GetName(),"Ne25")==0 || strcmp(cut->GetName(),"Ne25n")==0 || strcmp(cut->GetName(),"Ne25w")==0 || strcmp(cut->GetName(),"NeAll25")==0){
          BETA = 0.05;
          GAMMA = 1./sqrt(1. - BETA*BETA);
          //Effective BETA using FMA energy relative to 3800 for Ne25
          GAMMA = 1. - fma.E()/3800. + (fma.E()/3800.)*GAMMA;
          BETA = sqrt(1. - 1./(GAMMA*GAMMA));
        }
  

      obj.FillHistogram(cut->GetName(),"doppler",8192,0,8192,hit.GetDopplerANL(BETA));
      obj.FillHistogram(cut->GetName(),"doppler_0.05",8192,0,8192,hit.GetDopplerANL(0.05));
      obj.FillHistogram(cut->GetName(),"doppler_0.0415",8192,0,8192,hit.GetDopplerANL(0.0415));
      obj.FillHistogram(cut->GetName(),"energy_theta",8192,0,8192,hit.GetCoreEnergy(3),
          180,0,TMath::Pi(),hit.GetTheta());
      obj.FillHistogram(cut->GetName(),"doppler_theta",8192,0,8192,hit.GetDopplerANL(BETA),
          180,0,TMath::Pi(),hit.GetTheta());
      obj.FillHistogram(cut->GetName(),"SummaryEnergy",8192,0,8192,hit.GetCoreEnergy(3),
          200,0,200,hit.GetCrystalId());
      obj.FillHistogram(cut->GetName(),"SummaryDoppler",8192,0,8192,hit.GetDopplerANL(BETA),
          200,0,200,hit.GetCrystalId());
      obj.FillHistogram(cut->GetName(),"doppler_fmaE",2000,0,10000,fma.E(),
          8192,0,8192,hit.GetDopplerANL(BETA));
      obj.FillHistogram(cut->GetName(),"doppler_fmadE",2000,0,4000,fma.dE(),
          8192,0,8192,hit.GetDopplerANL(BETA));
      obj.FillHistogram(cut->GetName(),"pid",2000,0,10000,fma.E(),2000,0,4000,fma.dE());
      obj.FillHistogram(cut->GetName(),"x_de",1000,0,1000,fma.X(),2000,0,4000,fma.dE());
      obj.FillHistogram(cut->GetName(),"x_e",1000,0,1000,fma.X(),2000,0,10000,fma.E());
      obj.FillHistogram(cut->GetName(),"dt_e",2000,-1000,1000,dt,2000,0,10000,fma.E());
      obj.FillHistogram(cut->GetName(),"doppler_dt",8192,0,8192,hit.GetDopplerANL(BETA),
          2000,-1000,1000,dt);

      if(hit.GetDopplerANL(BETA) > 1690 && hit.GetDopplerANL(BETA) < 1711){
        obj.FillHistogram(cut->GetName(),"pid_1701",2000,0,10000,fma.E(),2000,0,4000,fma.dE());
        obj.FillHistogram(cut->GetName(),"x_de_1701",1000,0,1000,fma.X(),2000,0,4000,fma.dE());
        obj.FillHistogram(cut->GetName(),"x_e_1701",1000,0,1000,fma.X(),2000,0,10000,fma.E());
      }

      if(hit.GetDopplerANL(BETA) > 1265 && hit.GetDopplerANL(BETA) < 1285){
        obj.FillHistogram(cut->GetName(),"pid_1275",2000,0,10000,fma.E(),2000,0,4000,fma.dE());
        obj.FillHistogram(cut->GetName(),"x_de_1275",1000,0,1000,fma.X(),2000,0,4000,fma.dE());
        obj.FillHistogram(cut->GetName(),"x_e_1275",1000,0,1000,fma.X(),2000,0,10000,fma.E());
      }

      if(gretina->Size()==1){
        obj.FillHistogram(cut->GetName(),"doppler_m1",8192,0,8192,hit.GetDopplerANL(BETA));
      }
        

      double b=0.040;
      double b_use=b;
      double g=1./sqrt(1. - b*b);					    
      for(int i=0;i<200;i++) {
        //obj.FillHistogram(cut->GetName(),Form("doppler_theta_beta%.04f",b),8192,0,8192,hit.GetDopplerANL(b),
        //                                                180,0,TMath::Pi(),hit.GetTheta());

        if(strcmp(cut->GetName(),"Ne25")==0 || strcmp(cut->GetName(),"Ne25n")==0 || strcmp(cut->GetName(),"Ne25w")==0 || strcmp(cut->GetName(),"NeAll25")==0){
          //Effective BETA using FMA energy relative to 3800 for Ne25
          g = 1./sqrt(1. - b*b);
          g = 1. - fma.E()/3800. + (fma.E()/3800.)*g;
          b_use = sqrt(1. - 1./(g*g));
        } else {
	  b_use = b;
	}

        obj.FillHistogram(cut->GetName(),"doppler_beta",200,0.040,0.060,b,
            8192,0,8192,hit.GetDopplerANL(b_use));
        /*if(fma.X()>580){
          obj.FillHistogram(cut->GetName(),"doppler_beta_Xg580",200,0.035,0.055,b,
          8192,0,8192,hit.GetDopplerANL(b));
          }*/
        b+=0.0001;							 
      }						     
      obj.FillHistogram(cut->GetName(),"doppler_X",1000,0,1000,fma.X(),
          8192,0,8192,hit.GetDopplerANL(BETA));

      for(unsigned int z=y+1;z<gretina->Size();z++) {
        TGretinaHit hit2 = gretina->GetGretinaHit(z);
        double dt2 = fma.Time()-hit2.GetTime();
        //if(dt2<110 || dt2>170) continue;
        if(te->IsInside(dt2,hit2.GetCoreEnergy(3)) == 0) continue;

        if(hit.GetCoreEnergy(3) > hit2.GetCoreEnergy(3)) {
          obj.FillHistogram(cut->GetName(),"gdt",2000,-1000,1000,hit.GetTime()-hit2.GetTime(),
              8192,0,8192,hit2.GetDopplerANL(BETA));
          obj.FillHistogramSym(cut->GetName(),"gg",8192,0,8192,hit.GetDopplerANL(BETA),
              8192,0,8192,hit2.GetDopplerANL(BETA));
          if(gg_time && gg_time->IsInside(hit.GetTime()-hit2.GetTime(),hit2.GetDopplerANL(BETA)) && gg_time->IsInside(hit2.GetTime()-hit.GetTime(),hit.GetDopplerANL(BETA))){
            obj.FillHistogram(cut->GetName(),"gdt_gated",2000,-1000,1000,hit.GetTime()-hit2.GetTime(),
                8192,0,8192,hit2.GetDopplerANL(BETA));
            obj.FillHistogramSym(cut->GetName(),"gg_gdtgated",8192,0,8192,hit.GetDopplerANL(BETA),
                8192,0,8192,hit2.GetDopplerANL(BETA));
          }
        }
        if(hit2.GetCoreEnergy(3) > hit.GetCoreEnergy(3)) {
          obj.FillHistogram(cut->GetName(),"gdt",2000,-1000,1000,hit2.GetTime()-hit.GetTime(),
              8192,0,8192,hit.GetDopplerANL(BETA));
          obj.FillHistogramSym(cut->GetName(),"gg",8192,0,8192,hit.GetDopplerANL(BETA),
              8192,0,8192,hit2.GetDopplerANL(BETA));
          if(gg_time && gg_time->IsInside(hit2.GetTime()-hit.GetTime(),hit.GetDopplerANL(BETA)) && gg_time->IsInside(hit.GetTime()-hit2.GetTime(),hit2.GetDopplerANL(BETA))){
            obj.FillHistogram(cut->GetName(),"gdt_gated",2000,-1000,1000,hit2.GetTime()-hit.GetTime(),
                8192,0,8192,hit.GetDopplerANL(BETA));
            obj.FillHistogramSym(cut->GetName(),"gg_gdtgated",8192,0,8192,hit.GetDopplerANL(BETA),
                8192,0,8192,hit2.GetDopplerANL(BETA));
          }
        }
      }//end for loop for gamma-gamma


      // =========================
      //polarization stuff:
      // =========================
      //
      TVector3 *beam = new TVector3(0,0,1);
      if(hit.GetPad()==0) {
        hit.CleanInteractions();
        //for(size_t z=0;z<hit.Size();z++) {
        //  //printf("segment: %02i\n",hit.GetSegmentId(z));
        //  h->Fill(hit.GetSegmentId(z));
        //}
        if(hit.Size()==2) {
          int xtal = hit.GetCrystalId();
          int w1   = hit.GetInteractionPoint(0).Wedge();
          int w2   = hit.GetInteractionPoint(1).Wedge();
          std::string name  = Form("xtal%03i_%i_%i",xtal,w1,w2);
          std::string dname = Form("%s_pol",cut->GetName());
          obj.FillHistogram(dname,name,8192,0,8192,hit.GetDopplerANL(BETA));
          obj.FillHistogram(dname,"Doppler vs Xi hit==2",8192,0,8192,hit.GetDopplerANL(BETA),360,0,2.*TMath::Pi(),hit.GetXi(beam,0,1));
	  if((180./TMath::Pi())*hit.GetTheta() > 80 && (180./TMath::Pi())*hit.GetTheta() < 100){
	    obj.FillHistogram(dname,"Doppler vs Xi hit==2 theta_80_100",8192,0,8192,hit.GetDopplerANL(BETA),360,0,2.*TMath::Pi(),hit.GetXi(beam,0,1));
	  }
	  if((180./TMath::Pi())*hit.GetTheta() > 70 && (180./TMath::Pi())*hit.GetTheta() < 110){
	    obj.FillHistogram(dname,"Doppler vs Xi hit==2 theta_70_110",8192,0,8192,hit.GetDopplerANL(BETA),360,0,2.*TMath::Pi(),hit.GetXi(beam,0,1));
	  }
	  if((180./TMath::Pi())*hit.GetTheta() > 70 && (180./TMath::Pi())*hit.GetTheta() < 140){
	    obj.FillHistogram(dname,"Doppler vs Xi hit==2 theta_70_140",8192,0,8192,hit.GetDopplerANL(BETA),360,0,2.*TMath::Pi(),hit.GetXi(beam,0,1));
	  }
	  if((180./TMath::Pi())*hit.GetTheta() > 70 && (180./TMath::Pi())*hit.GetTheta() < 165){
	    obj.FillHistogram(dname,"Doppler vs Xi hit==2 theta_70_165",8192,0,8192,hit.GetDopplerANL(BETA),360,0,2.*TMath::Pi(),hit.GetXi(beam,0,1));
	  }
        }
        if(hit.Size()>=2) {
          //int xtal = hit.GetCrystalId();
          //int w1   = hit.GetInteractionPoint(0).Wedge();
          //int w2   = hit.GetInteractionPoint(1).Wedge();
          //std::string name  = Form("xtal%03i_%i_%i",xtal,w1,w2);
          std::string dname = Form("%s_pol",cut->GetName());
          //obj.FillHistogram(dname,name,8192,0,8192,hit.GetDopplerANL(BETA));
          obj.FillHistogram(dname,"Doppler vs Xi hit>=2",8192,0,8192,hit.GetDopplerANL(BETA),360,0,2.*TMath::Pi(),hit.GetXi(beam,0,1));
          if((180./TMath::Pi())*hit.GetTheta() > 80 && (180./TMath::Pi())*hit.GetTheta() < 100){
	    obj.FillHistogram(dname,"Doppler vs Xi hit>=2 theta_80_100",8192,0,8192,hit.GetDopplerANL(BETA),360,0,2.*TMath::Pi(),hit.GetXi(beam,0,1));
	  }
	  if((180./TMath::Pi())*hit.GetTheta() > 70 && (180./TMath::Pi())*hit.GetTheta() < 110){
	    obj.FillHistogram(dname,"Doppler vs Xi hit>=2 theta_70_110",8192,0,8192,hit.GetDopplerANL(BETA),360,0,2.*TMath::Pi(),hit.GetXi(beam,0,1));
	  }
	  if((180./TMath::Pi())*hit.GetTheta() > 70 && (180./TMath::Pi())*hit.GetTheta() < 140){
	    obj.FillHistogram(dname,"Doppler vs Xi hit>=2 theta_70_140",8192,0,8192,hit.GetDopplerANL(BETA),360,0,2.*TMath::Pi(),hit.GetXi(beam,0,1));
	  }
	  if((180./TMath::Pi())*hit.GetTheta() > 70 && (180./TMath::Pi())*hit.GetTheta() < 165){
	    obj.FillHistogram(dname,"Doppler vs Xi hit>=2 theta_70_165",8192,0,8192,hit.GetDopplerANL(BETA),360,0,2.*TMath::Pi(),hit.GetXi(beam,0,1));
	  }
        }
      }

      }//end if cut isinside E,dE pid
      }//end while loop over cuts			

		   
    }//end loop over all gretina hits


    //Loop over addback hits
      std::vector<double> energy_addback;
      std::vector<double> doppler_addback;
      std::vector<double> doppler_addback_n0;
      std::vector<double> doppler_addback_n1;
      std::vector<double> doppler_addback_n2;
      std::vector<double> doppler_addback_ng;
      std::vector<double> time_addback;
      TVector3 distance;
      double tdiff = -20000;
      obj.FillHistogram("calorimeter",8192,0,8192,calorimeterEnergy);
      
      while(hits.size() > 0){
        TGretinaHit currentHit = hits.back();
        hits.pop_back();
        
      // Find and add all hits in a cluster of adjacent crystals including
      // the current hit.
      //
      // CAUTION: This clustering includes neighbors of neighbors!
      std::vector<TGretinaHit> cluster;
      cluster.push_back(currentHit);
      unsigned int lastClusterSize = 0;
      while(lastClusterSize < cluster.size()){
        for(unsigned int i = 0; i < cluster.size(); i++){
	  for(unsigned int j = 0; j < hits.size(); j++){
	    //distance = cluster[i].GetCrystalPosition() - hits[j].GetCrystalPosition();	    
	    distance = cluster[i].GetPosition() - hits[j].GetPosition();
	    obj.FillHistogram("crystal_separation",1000, 0., 1000., distance.Mag());
	    if(cluster.size() > 0 && hits.size() > 0) tdiff = cluster[i].GetTime() - hits[j].GetTime();
	    obj.FillHistogram("tdiff",1000,0,1000,abs(tdiff));
	    if(distance.Mag() < 100. && abs(tdiff)<50.){ // Neighbors; was 80 from UCGretina wth GetCrystalPosition but no neighbors ever
	      cluster.push_back(hits.back());
	      hits.pop_back();
	    }
	  }
        }
        lastClusterSize = cluster.size();
      }
    
      // Calculate the total energy deposited in the cluster,
      // and count the pairs of neighbors.
      int neighbors = 0;
      double addbackEnergy = 0.;
      TVector3 firstHitPos;
      TVector3 secondHitPos;
      double firstHitEnergy = 0;
      double secondHitEnergy = -1;
      double firstHitTime = 0;
      TVector3 distancec;
      double tdiffc = -20000;
      for(unsigned int i = 0; i < cluster.size(); i++){
        addbackEnergy += cluster[i].GetCoreEnergy(3);

        // Find the largest IP in the cluster and save its position
        // for Doppler correction.
	for(int k = 0; k < cluster[i].Size(); k++){
          if(cluster[i].GetIntAssignEng(k) > firstHitEnergy){   
	    firstHitPos = cluster[i].GetIntPosition(k);
	    firstHitEnergy = cluster[i].GetIntAssignEng(k);
            firstHitTime = cluster[i].GetTime();
	  }
	}
      
        for(unsigned int j = i+1; j < cluster.size(); j++){
	  //distance = cluster[i].GetCrystalPosition() - cluster[j].GetCrystalPosition();
	  distancec = cluster[i].GetPosition() - cluster[j].GetPosition();
	  if(cluster.size() > 0 && hits.size() > 0) tdiffc = cluster[i].GetTime() - cluster[j].GetTime();
	  if(distancec.Mag() < 100. && abs(tdiffc)<50.) neighbors++; //was 80 but no neighbors ever?
        }
      }

      // Find the second largest IP and save for linear polarization
      for(unsigned int i = 0; i < cluster.size(); i++){
	for(int j = 0; j < cluster[i].Size(); j++){
          if(cluster[i].GetIntAssignEng(j) != firstHitEnergy){
	    if(secondHitEnergy == -1){
	        secondHitPos = cluster[i].GetIntPosition(j);
	        secondHitEnergy = cluster[i].GetIntAssignEng(j);
	    }
	    else if(cluster[i].GetIntAssignEng(j) > secondHitEnergy) {
	        secondHitPos = cluster[i].GetIntPosition(j);
	        secondHitEnergy = cluster[i].GetIntAssignEng(j);
	    }
	  }
        }
      }

      obj.FillHistogram("clusterSize_vs_neighborPairs",20,0,20,neighbors,10,0,10,cluster.size());

      TIter iter(gates);
      while(TObject *gobj = iter.Next()) {

      GCutG *cut = (GCutG*)gobj;
      if(strcmp(cut->GetTag(),"pid")!=0) continue;
      if(strcmp(cut->GetName(),"Ne25")==0 && Ne25_xe->IsInside(fma.X(),fma.E())==0) continue;
      if(strcmp(cut->GetName(),"Ne25n")==0 && Ne25n_xe->IsInside(fma.X(),fma.E())==0) continue;
      if(strcmp(cut->GetName(),"Ne25w")==0 && Ne25w_xe->IsInside(fma.X(),fma.E())==0) continue;
      if(strcmp(cut->GetName(),"NeAll25")==0 && NeAll25w_xe->IsInside(fma.X(),fma.E())==0) continue;
      if(cut->IsInside(fma.E(),fma.dE())) {
        if(strcmp(cut->GetName(),"Mg25")==0){
          BETA = 0.048;
        }
        if(strcmp(cut->GetName(),"Na25")==0){
          BETA = 0.047;
        }
        if(strcmp(cut->GetName(),"Ne22")==0){
          BETA = 0.0436;
        }
        if(strcmp(cut->GetName(),"F22")==0){
          BETA = 0.059;
        }
        if(strcmp(cut->GetName(),"Ne25")==0 || strcmp(cut->GetName(),"Ne25n")==0 || strcmp(cut->GetName(),"Ne25w")==0 || strcmp(cut->GetName(),"NeAll25")==0){
          BETA = 0.05;
          GAMMA = 1./sqrt(1. - BETA*BETA);
          //Effective BETA using FMA energy relative to 3800 for Ne25
          GAMMA = 1. - fma.E()/3800. + (fma.E()/3800.)*GAMMA;
          BETA = sqrt(1. - 1./(GAMMA*GAMMA));
        }

      // Doppler correct the addback energy
      double dopplerAB = 0.;
      double calorimeterAB = 0.;
      GAMMA = 1./sqrt(1. - BETA*BETA);
      dopplerAB = addbackEnergy*GAMMA*(1-BETA*TMath::Cos(firstHitPos.Theta()));
      calorimeterAB = calorimeterEnergy*GAMMA*(1-BETA*TMath::Cos(firstHitPos.Theta()));
    
      TString addbackType;
      
      energy_addback.push_back(addbackEnergy);
      doppler_addback.push_back(dopplerAB);
      time_addback.push_back(firstHitTime);
      if(neighbors == 0 && cluster.size() == 1){
        addbackType = "n0";
        doppler_addback_n0.push_back(dopplerAB);
      }
      else if(neighbors == 1 && cluster.size() == 2){
        addbackType = "n1";
        doppler_addback_n1.push_back(dopplerAB);
      }
      else if(neighbors == 3 && cluster.size() == 3){
        addbackType = "n2";
	doppler_addback_n2.push_back(dopplerAB);
      }
      else{
        addbackType = "ng";
	doppler_addback_ng.push_back(dopplerAB);
      }

      obj.FillHistogram(cut->GetName(),"dopplerAB",8192,0,8192,dopplerAB);
      obj.FillHistogram(cut->GetName(),"energyAB",8192,0,8192,addbackEnergy);
      obj.FillHistogram(cut->GetName(),"calorimeterdopAB",8192,0,8192,calorimeterAB);
      obj.FillHistogram(cut->GetName(),"calorimeterenAB",8192,0,8192,calorimeterEnergy);

      obj.FillHistogram(cut->GetName(),"dopplerAB_"+addbackType,8192,0,8192,dopplerAB);
      obj.FillHistogram(cut->GetName(),"dopplerAB_"+addbackType+"_theta",8192,0,8192,dopplerAB,
          180,0,TMath::Pi(),firstHitPos.Theta());
      
      if(addbackType == "n0" || addbackType == "n1"){
        obj.FillHistogram(cut->GetName(),"dopplerAB_n0n1",8192,0,8192,dopplerAB);
        obj.FillHistogram(cut->GetName(),"dopplerAB_n0n1_theta",8192,0,8192,dopplerAB,
          180,0,TMath::Pi(),firstHitPos.Theta());
      }

      if(addbackType == "n0" || addbackType == "n1" || addbackType == "n2"){
        obj.FillHistogram(cut->GetName(),"dopplerAB_n0n1n2",8192,0,8192,dopplerAB);
        obj.FillHistogram(cut->GetName(),"dopplerAB_n0n1n2_theta",8192,0,8192,dopplerAB,
          180,0,TMath::Pi(),firstHitPos.Theta());
      }

    
      // =========================
      //polarization stuff:
      // =========================
      //
     if(secondHitEnergy == -1) continue;

     TVector3 *beam = new TVector3(0,0,1); 
     TVector3 comptonPlaneNorm = firstHitPos.Cross(secondHitPos);
     TVector3 reactionPlaneNorm = beam->Cross(firstHitPos);
     TVector3 basisNorm = firstHitPos.Cross(reactionPlaneNorm);

     double xi = reactionPlaneNorm.Angle(comptonPlaneNorm);
     if(basisNorm.Angle(comptonPlaneNorm) > TMath::Pi()/2.) xi = 2.*TMath::Pi() - xi;

        //if(addbackType == "n0" || addbackType == "n1") {
	  std::string dname = Form("%s_pol",cut->GetName());
          obj.FillHistogram(dname,"ABDoppler vs Xi",8192,0,8192,dopplerAB,360,0,2.*TMath::Pi(),xi);
	  if((180./TMath::Pi())*firstHitPos.Theta() > 80 && (180./TMath::Pi())*firstHitPos.Theta() < 100){
	    obj.FillHistogram(dname,"ABDoppler vs Xi theta_80_100",8192,0,8192,dopplerAB,360,0,2.*TMath::Pi(),xi);
	  }
	  if((180./TMath::Pi())*firstHitPos.Theta() > 70 && (180./TMath::Pi())*firstHitPos.Theta() < 110){
	    obj.FillHistogram(dname,"ABDoppler vs Xi theta_70_110",8192,0,8192,dopplerAB,360,0,2.*TMath::Pi(),xi);
	  }
	  if((180./TMath::Pi())*firstHitPos.Theta() > 70 && (180./TMath::Pi())*firstHitPos.Theta() < 140){
	    obj.FillHistogram(dname,"ABDoppler vs Xi theta_70_140",8192,0,8192,dopplerAB,360,0,2.*TMath::Pi(),xi);
	  }
	  if((180./TMath::Pi())*firstHitPos.Theta() > 70 && (180./TMath::Pi())*firstHitPos.Theta() < 165){
	    obj.FillHistogram(dname,"ABDoppler vs Xi theta_70_165",8192,0,8192,dopplerAB,360,0,2.*TMath::Pi(),xi);
	  }
        //}
        

    //}//end if cut isinside E,dE pid
    //}//end while loop over cuts

    //}//end while addback hits   

    //gamma-gamma addback plots
    /*TIter iter(gates);
      while(TObject *gobj = iter.Next()) {

      GCutG *cut = (GCutG*)gobj;
      if(strcmp(cut->GetTag(),"pid")!=0) continue;
      if(strcmp(cut->GetName(),"Ne25")==0 && Ne25_xe->IsInside(fma.X(),fma.E())==0) continue;
      if(strcmp(cut->GetName(),"Ne25n")==0 && Ne25n_xe->IsInside(fma.X(),fma.E())==0) continue;
      if(strcmp(cut->GetName(),"Ne25w")==0 && Ne25w_xe->IsInside(fma.X(),fma.E())==0) continue;
      if(strcmp(cut->GetName(),"NeAll25")==0 && NeAll25w_xe->IsInside(fma.X(),fma.E())==0) continue;
      if(cut->IsInside(fma.E(),fma.dE())) {
        if(strcmp(cut->GetName(),"Mg25")==0){
          BETA = 0.048;
        }
        if(strcmp(cut->GetName(),"Na25")==0){
          BETA = 0.047;
        }
        if(strcmp(cut->GetName(),"Ne22")==0){
          BETA = 0.0436;
        }
        if(strcmp(cut->GetName(),"F22")==0){
          BETA = 0.059;
        }
        if(strcmp(cut->GetName(),"Ne25")==0 || strcmp(cut->GetName(),"Ne25n")==0 || strcmp(cut->GetName(),"Ne25w")==0 || strcmp(cut->GetName(),"NeAll25")==0){
          BETA = 0.05;
          GAMMA = 1./sqrt(1. - BETA*BETA);
          //Effective BETA using FMA energy relative to 3800 for Ne25
          GAMMA = 1. - fma.E()/3800. + (fma.E()/3800.)*GAMMA;
          BETA = sqrt(1. - 1./(GAMMA*GAMMA));
        }*/


    int addback_mult = doppler_addback.size();
    int n0_mult = doppler_addback_n0.size();
    int n1_mult = doppler_addback_n1.size();
    int n2_mult = doppler_addback_n2.size();
    int ng_mult = doppler_addback_ng.size();

    for(int i = 0; i < addback_mult; i++){
      for(int j = i+1; j < addback_mult; j++){
        if(energy_addback.at(i) == energy_addback.at(j)) continue; //events passing multiple cuts staying in vector?
          obj.FillHistogramSym(cut->GetName(),"ab_gg",8192,0,8192,doppler_addback.at(i),8192,0,8192,doppler_addback.at(j));
	if(ng_mult==0){
	  obj.FillHistogramSym(cut->GetName(),"ab_gg_nogarbage",8192,0,8192,doppler_addback.at(i),8192,0,8192,doppler_addback.at(j));
	}
	if(n0_mult + n1_mult + n2_mult == 2 && ng_mult == 0){
	  obj.FillHistogramSym(cut->GetName(),"ab_gg_mult2_nogarbage",8192,0,8192,doppler_addback.at(i),8192,0,8192,doppler_addback.at(j));
	}
	if(energy_addback.at(i) > energy_addback.at(j)){
	  if(gg_time && gg_time->IsInside(time_addback.at(i)-time_addback.at(j),doppler_addback.at(j)) && gg_time->IsInside(time_addback.at(j)-time_addback.at(i),doppler_addback.at(i))){
	    obj.FillHistogramSym(cut->GetName(),"ab_gg_gdtgated",8192,0,8192,doppler_addback.at(i),8192,0,8192,doppler_addback.at(j));
	  }
	}
	if(energy_addback.at(j) > energy_addback.at(i)){
	  if(gg_time && gg_time->IsInside(time_addback.at(j)-time_addback.at(i),doppler_addback.at(i)) && gg_time->IsInside(time_addback.at(i)-time_addback.at(j),doppler_addback.at(j))){
	    obj.FillHistogramSym(cut->GetName(),"ab_gg_gdtgated",8192,0,8192,doppler_addback.at(i),8192,0,8192,doppler_addback.at(j));
	  }
	}
      }
    }

    obj.FillHistogram(cut->GetName(),"addback_mult",20,0.,20.,addback_mult);


    }//end if cut isinside E,dE pid
    }//end while loop over cuts
    /*energy_addback.clear();
    doppler_addback.clear();
    doppler_addback_n0.clear();
    doppler_addback_n1.clear();
    doppler_addback_n2.clear();
    doppler_addback_ng.clear();*/
    }//end while loop over addback hits

    if(numobj!=list->GetSize())
      list->Sort();

  } //end MakeHistograms
