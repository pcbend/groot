
#include "GFunctions.h"
#include "TF1.h"

#include <Math/Math.h>
#include <Math/SpecFuncMathMore.h>


NamespaceImp(GFunctions)

#define PI TMATH::Pi()

Double_t GFunctions::PolyBg(Double_t *dim, Double_t *par, Int_t order) {
  Double_t result = 0.0;
  int j=0;
  for(Int_t i=0;i<=order;i++) {
    result += *(par+j) *TMath::Power(dim[0],i);
    j++;
  }
    //result += par[i]*TMath::Power(dim[0]-par[order+1],i);
  return result;
}

Double_t GFunctions::LinFit(Double_t *dim, Double_t *par) {
  return PolyBg(dim,par,1);
}

//Double_t GFunctions::LinearBG(Double_t *dim,Double_*par) {
//  //  -dim[0]: channels to fit
//  //  -par[0]: offset
//  //  -par[1]: slope
//  //  -par[2]: begin of exclude; should be fixed.
//  //  -par[3]: end of exclude; should be fixed.
// 
//  if(!std::is_nan(par[2]) && !std::is_nan(par[3])) {
//    if(par[2]>par[3])
//      std::swap(par[2],par[3]);
//    if(x[0]>par[2] && x[0]<par[3]) {
//      TF1::RejectPoint();
//      return 0;
//    }
//  }
//  return par[0] + par[1]*x[0];
//}


Double_t GFunctions::QuadFit(Double_t *dim, Double_t *par) {
  return PolyBg(dim,par,2);
}

Double_t GFunctions::StepFunction(Double_t *dim, Double_t *par) {
  //  -dim[0]: channels to fit
  //  -par[0]: height of peak
  //  -par[1]: centroid of peak
  //  -par[2]: sigma of peak
  //  -par[3]: size of step in step function.

  Double_t x       = dim[0];
  
  Double_t height  = par[0];
  Double_t cent    = par[1];
  Double_t sigma   = par[2];
  //Double_t R       = par[4];
  Double_t step    = par[3];

  //return TMath::Abs(step)*height/100.0*TMath::Erfc((x-cent)/(TMath::Sqrt(2.0)*sigma));
  return height*(step/100.0) *TMath::Erfc((x-cent)/(TMath::Sqrt(2.0)*sigma));

}

Double_t GFunctions::StepBG(Double_t *dim, Double_t *par) {
  return StepFunction(dim,par) + PolyBg(dim,(par+4),0);
}

Double_t GFunctions::Gaus(Double_t *dim, Double_t *par) {
  // - dim[0]: channels to fit
  // - par[0]: height of peak
  // - par[1]: cent of peak
  // - par[2]: sigma
  // - par[3]: relative height of skewed gaus to gaus

  Double_t x      = dim[0];
  Double_t height = par[0];
  Double_t cent   = par[1];
  Double_t sigma  = par[2];
  Double_t R      = par[3];

  return height*(1.0-R/100.0)*TMath::Gaus(x,cent,sigma);
}


Double_t GFunctions::DoubleGaus(Double_t *dim, Double_t *par) {
  // - dim[0]: channels to fit
  // - par[0]: height of peak1
  // - par[1]: cent of peak1
  // - par[2]: height of peak2
  // - par[3]: cent of peak2
  // - par[4]: shared sigma
  // - par[5]: bg offset
  // - par[6]: bg slope

  Double_t x      = dim[0];
  Double_t height1 = par[0];
  Double_t cent1   = par[1];

  Double_t height2 = par[2];
  Double_t cent2   = par[3];
  
  Double_t sigma   = par[4];

  return height1*TMath::Gaus(x,cent1,sigma) + height2*TMath::Gaus(x,cent2,sigma) + PolyBg(dim,(par+5),2);
}






Double_t GFunctions::SkewedGaus(Double_t *dim,Double_t *par) {

  // StepFunction(dim,par) + PolyBg
  // - par[0]: height of peak
  // - par[1]: cent of peak
  // - par[2]: sigma
  // - par[3]: relative height of skewed gaus to gaus
  // - par[4]: "skewedness" of the skewed gaussin

  Double_t x      = dim[0]; //channel number used for fitting
  Double_t height = par[0]; //height of photopeak
  Double_t cent   = par[1]; //Peak Centroid of non skew gaus
  Double_t sigma  = par[2]; //standard deviation of gaussian
  Double_t R      = par[3]; //relative height of skewed gaussian
  Double_t beta   = par[4]; //"skewedness" of the skewed gaussian

  double scaling = R*height/100.0;
  //double x_rel = (x - cent)/sigma;

  double fterm = (x-cent)/(sigma*TMath::Sqrt(2.));
  double sterm = sigma /  (beta *TMath::Sqrt(2.));

  return scaling * TMath::Exp((x-cent)/beta) * TMath::Erfc(fterm + sterm); 
}

Double_t GFunctions::PhotoPeak(Double_t *dim,Double_t *par) {
  return Gaus(dim,par) + SkewedGaus(dim,par);
}

Double_t GFunctions::PhotoPeakBG(Double_t *dim,Double_t *par) {
  // - dim[0]: channels to fit
  // - par[0]: height of peak
  // - par[1]: cent of peak
  // - par[2]: sigma
  // - par[3]: relative height of skewed gaus to gaus
  // - par[4]: "skewedness" of the skewed gaussin
  // - par[5]: size of stepfunction step.

  // - par[6]: base bg height.
  // - par[7]: slope of bg.
  
  double spar[4];
  spar[0] = par[0];
  spar[1] = par[1];
  spar[2] = par[2];
  spar[3] = par[5];  //stepsize;
  return Gaus(dim,par) + SkewedGaus(dim,par) + StepFunction(dim,spar) + PolyBg(dim,par+6,0);
}

Double_t GFunctions::PhotoPeakBGExcludeRegion(Double_t *dim,Double_t *par) {
  // - dim[0]: channels to fit
  // - par[0]: height of peak
  // - par[1]: cent of peak
  // - par[2]: sigma
  // - par[3]: relative height of skewed gaus to gaus
  // - par[4]: "skewedness" of the skewed gaussin
  // - par[5]: size of stepfunction step.
  
  // - par[6]: base bg height.
  
  // - par[7]: exclude low;
  // - par[8]: exclude high;

  if(dim[0]>par[7] && dim[0]<par[8]) {
    TF1::RejectPoint();
    return 0;
  }
  double spar[4];
  spar[0] = par[0];
  spar[1] = par[1];
  spar[2] = par[2];
  spar[3] = par[5];  //stepsize;
  return Gaus(dim,par) + SkewedGaus(dim,par) + StepFunction(dim,spar) + PolyBg(dim,par+6,0);
}


// For fitting Ge detector efficiencies.
Double_t GFunctions::Efficiency(Double_t *dim, Double_t *par){
  // - dim[0]: energy.
  // - par[0]: zeroth order
  // - par[1]: first order
  // - par[2]: second order
  // - par[3]: inverse energy squared term.
  // - Formula : 10**(0+1*Log(x)+2*Log(x)**2+3/x**2)

  Double_t x  = dim[0];
  Double_t p0 = par[0];
  Double_t p1 = par[1];
  Double_t p2 = par[2];
  Double_t p3 = par[3];

  if(x!=0)
    return pow(10.0,(p0+p1*TMath::Log10(x)+p2*std::pow(TMath::Log10(x),2.0)+p3/(std::pow(x,2.0))));
  else
    return 0;

}


Double_t GFunctions::GausExpo(Double_t *x, Double_t *pars) {

  double result;

  // gaus + step*expo conv with a gaus.

  // par[0] = height
  // par[1] = cent
  // par[2] = sigma
  // par[3] = decay parameter

  result = TMath::Gaus(pars[0],pars[1],pars[2])+(double)(x[0]>pars[1])*pars[0]*TMath::Exp(-pars[3]); 
  return result;
}





Double_t GFunctions::LanGaus(Double_t *x, Double_t *pars){
   double dy, y, conv, spec, gaus;
   conv = 0;

   for(int i=0; i<10; i++){
    dy = 5*pars[2]/10.0; // truncate the convolution by decreasing number of evaluation points and decreasing range [2.5 sigma still covers 98.8% of gaussian]
    y = x[0]-2.5*pars[2]+dy*i;
    spec = pars[0]+pars[1]*y; // define background SHOULD THIS BE CONVOLUTED ????? *************************************
    //for( int n=0; n<(int)(pars[0]+0.5); n++) // the implementation of landau function should be done using the landau function
      spec +=pars[3]*TMath::Landau(-y,-pars[4],pars[5])/TMath::Landau(0,0,100); // add peaks, dividing by max height of landau
    gaus = TMath::Gaus(-x[0],-y,pars[2])/sqrt(2*TMath::Pi()*pars[2]*pars[2]); // gaus must be normalisd so there is no sigma weighting
    conv += gaus*spec*dy; // now convolve this [integrate the product] with a gaussian centered at x;
  }

  return conv;
}


Double_t GFunctions::LanGausHighRes(Double_t *x, Double_t *pars){ // 5x more convolution points with 1.6x larger range
  double dy, y, conv, spec, gaus;
  conv = 0;

  for(int i=0; i<50; i++){
    dy = 8*pars[2]/50.0; // 4 sigma covers 99.99% of gaussian
    y  = x[0]-4*pars[2]+dy*i;

    spec = pars[0]+pars[1]*y;
    //for( int n=0; n<(int)(pars[0]+0.5); n++)
    spec +=pars[3]*TMath::Landau(-y,-pars[4],pars[5])/TMath::Landau(0,0,100);

    gaus = TMath::Gaus(-x[0],-y,pars[2])/sqrt(2*TMath::Pi()*pars[2]*pars[2]);
    conv += gaus*spec*dy;
  }
  return conv;
}


Double_t GFunctions::GammaEff(Double_t *x,Double_t *par) {
  // LOG(EFF) = A0 + A1*LOG(E) + A2*LOG(E)^2 + A3/E^2 

  double logE = TMath::Log10(x[0]);
  double temp =  par[0] + par[1]*logE + par[2]*logE*logE +par[3]/(x[0]*x[0]);
  return pow(10,temp);

}

Double_t GFunctions::AlignedAD(Double_t *x,Double_t *par) {
  double p2 = ::ROOT::Math::legendre(2,TMath::Cos(TMath::DegToRad()*x[0]));
  double p4 = ::ROOT::Math::legendre(4,TMath::Cos(TMath::DegToRad()*x[0]));
  return par[0] + par[1]*p2 + par[2]*p4;
}

Double_t GFunctions::AlignedAD_Norm(Double_t *x,Double_t *par) {
  double p2 = ::ROOT::Math::legendre(2,TMath::Cos(TMath::DegToRad()*x[0]));
  double p4 = ::ROOT::Math::legendre(4,TMath::Cos(TMath::DegToRad()*x[0]));
  return par[0] * (1 + par[1]*p2 + par[2]*p4);
}


Double_t GFunctions::AlignedADPol_Norm(Double_t *x,Double_t *par) {
  
  double p2 = ::ROOT::Math::legendre(2,TMath::Cos(TMath::DegToRad()*x[0]));
  double p4 = ::ROOT::Math::legendre(4,TMath::Cos(TMath::DegToRad()*x[0]));
  double ap2 = ::ROOT::Math::assoc_legendre(2,2,TMath::Cos(TMath::DegToRad()*x[0]));
  double ap4 = ::ROOT::Math::assoc_legendre(4,2,TMath::Cos(TMath::DegToRad()*x[0]));

  double phi = TMath::DegToRad()*x[1];

  double part2 =  par[1]*(p2 +(1./2.)*TMath::Cos(2*phi)*ap2);
  double part4 =  par[2]*(p4 -(1./12.)*TMath::Cos(2*phi)*ap4);

  return par[0] * (1 + part2 + part4);
}



Double_t GFunctions::Polarization(Double_t *x,Double_t *par) {

  //par[0] = Ao
  //par[1] = a2
  //par[2] = a4

  double p2 = ::ROOT::Math::legendre(2,TMath::Cos(TMath::DegToRad()*x[0]));
  double p4 = ::ROOT::Math::legendre(4,TMath::Cos(TMath::DegToRad()*x[0]));
  double ap2 = ::ROOT::Math::assoc_legendre(2,2,TMath::Cos(TMath::DegToRad()*x[0]));
  double ap4 = ::ROOT::Math::assoc_legendre(4,2,TMath::Cos(TMath::DegToRad()*x[0]));


  double k2 = (1./2.);
  double k4 = (-1./12);

  return (1/par[0]) * ( par[1]*k2*ap2 + par[2]*k4*ap4 ) / ( 1 + par[1]*p2 + par[2]*p4 );

}



Double_t GFunctions::ComptonEnergy(Double_t *x,Double_t *par) {
  //par[0] = inital gamma energy; in keV.
  double lower = 1 + (par[0]/511.)*(1-TMath::Cos(TMath::DegToRad()*x[0]));
  return par[0]/lower;

}

Double_t GFunctions::ComptonAngle(Double_t *x,Double_t *par) {
  //par[0] = inital gamma energy; in keV.
  if(x[0]>par[0])
    return 0.00;
  
  double inside = (1./x[0] - 1./par[0]);
  inside*=511.;
  inside+= -1;
  //printf("inside[%.02f]  = %.023f\n",x[0],inside);
  return TMath::ACos(inside)*TMath::RadToDeg();

}

Double_t GFunctions::ComptonRatio(Double_t *x,Double_t *par) {
  //no parameters 
  //x[0] = Initial Energy
  //x[1] = Scattering Angle (deg) 
  //returns E'/E

  double temp = 1 + (x[0]/511.)*(1-TMath::Cos(x[1]*TMath::DegToRad()));
  return 1/temp;
}

Double_t GFunctions::KN_unpol(Double_t *x,Double_t *par) {

  //par[0] = energy of the intial gamma;
  //x[0]   = compton scattering angle;

  double e0    =  8.854187817620E-12;
  double J2KeV =  6.242e+12;
  double me    =  0.511046 / J2KeV;
  double re    =  (1/(4.*TMath::Pi()*e0)) * (TMath::Qe()*TMath::Qe())/me;

  double y[2] = {par[0],x[0]};
  double cr   = ComptonRatio(y,0);

  return  0.5*re * (cr)*(cr) * ( (1/cr)  + cr - TMath::Power(TMath::Sin(x[0]*TMath::DegToRad()),2) ); 
  //return   0.5*((4.*TMath::Pi()))* (cr)*(cr) * ( (1/cr)  + cr - TMath::Power(TMath::Sin(x[0]*TMath::DegToRad()),2) ); 

}

Double_t GFunctions::KN_unpol_norm(Double_t *x,Double_t *par) {

  //par[0] = energy of the intial gamma;
  //par[1] = energy of the normalization gamma;
  //x[0]   = compton scattering angle;

  return KN_unpol(x,&par[0]) / KN_unpol(x,&par[1]);

}

Double_t GFunctions::KN_pol(Double_t *x,Double_t *par) {

  //par[0] = energy of the intial gamma;
  //x[0]   = compton scattering angle;
  //x[1]   = angle between electric field vector and scattering plane

  double e0    =  8.854187817620E-12;
  double J2KeV =  6.242e+12;
  double me    =  0.511046 / J2KeV;
  double re    =  (1/(4.*TMath::Pi()*e0)) * (TMath::Qe()*TMath::Qe())/me;

  double y[2] = {par[0],x[0]};
  double cr   = ComptonRatio(y,0);

  return  0.5*re * (cr)*(cr) * ( (1/cr)  + cr - 
          2 * TMath::Power(TMath::Sin(x[0]*TMath::DegToRad()),2) 
            * TMath::Power(TMath::Cos(x[1]*TMath::DegToRad()),2) ); 

}

Double_t GFunctions::AnalyzingPower(Double_t *x,Double_t *par) {

  //par[0] = inital gamma energy; in keV.
  //x[0]   = compton scattering angle.
  double scattered = ComptonEnergy(x,par);

  double sin2 = (TMath::Sin(TMath::DegToRad()*x[0]))*(TMath::Sin(TMath::DegToRad()*x[0]));
  double lower = scattered/par[0] + par[0]/scattered  - sin2;
  return sin2/lower;
}



Double_t GFunctions::PolarizationAsymmetry(Double_t *x,Double_t *par) {
  // par[0]  -> overall constant ( includes integral of the analyizing power over the observed compton scattering angle)
  // par[1]  -> Ao
  // par[2]  -> a2
  // par[3]  -> a4

  // x[0] = theta;
  // x[1] = angle between the reaction plane and the compton scattered plane.

  return par[0] * Polarization(&x[0],&(par[1])) * TMath::Cos(2*TMath::DegToRad()*x[1]);

}

Double_t GFunctions::Bateman(Double_t *x, Double_t *par, Int_t gen=1){
  // gen: generation of one isotope in the decay chain (parent = 1, daughter = 2, granddaughter = 3 ...
  // par[0] = partilce[gen] activity
  // par[i] = half-life of particle[i] (gen >= i >= 1);

  double mul = 1;
  double sum = 0;
  for(int i=1;i<=gen;i++){
    double lami = 0.693/par[i];
    if(i>=2 && (i<=gen-1)) mul = mul*lami;
    double denominator = 1;
    for(int j=1;j<=gen;j++){
      if(j==i) continue;
      double lamj = 0.693/par[j];
      denominator = denominator * (lamj - lami);
    }
    sum += TMath::Exp(-lami*x[0])/denominator;
  }
  if(gen==1) return par[0]*mul*sum;
  return (0.693/par[gen])*(par[0]*mul*sum);
}

//Double_t Bateman(double *x, double *par){
//  // par[0] = generation of isotopes in the decay chain (parent = 1, daughter = 2 ...);
//  // par[1] = activity;
//  // par[i+2] = half-life of particle_i(i>0)
//  int n = par[0];
//  double A = par[1];
//  std::vector<double> lam;
//  for(int i=2;i<n+2;i++){
//    lam.push_back(0.693/par[i]);
//  }
// 
// 
//  double mul = 1;
//  double sum = 0;
//  for(int i=0;i<lam.size();i++){
//    if(i>=1 && (i<lam.size()-1)) mul = mul*lam[i];
//    double denominator = 1;
//    for(int j=0;j<lam.size();j++){
//      if(j==i) continue;
//      denominator = denominator * (lam[j] - lam[i]);
//    }
//    sum += TMath::Exp(-lam[i]*x[0])/denominator;
//  }
//  if(lam.size()==1) return A*mul*sum;
//  return (lam.back())*(A*mul*sum);
//}

/*Double_t GFunction::DecayChain(double *x, double *par){
  // par[0] = generation of isotopes in the decay chain (parent = 1, daughter = 2 ...);
  // par[1] = activity;
  // par[i+2] = half-life of particle_i(i>0)

  int gen = par[0];
  std::vector<double *> vec_par; 
  for(int i=1;i<=gen;i++){
    double subpar[i+1];
    for(int j=0;j<=i;j++){
      subpar[j] = par[j+1];
    }
    vec_par.push_back(subpar); 
  }
  
  double sum = 0;
  for(int i=1;i<=gen;i++){
    sum += Bateman(x,vec_par[i-1],i);
  }

  return sum;
}*/


//Double_t GFunctions::DecayMap()















