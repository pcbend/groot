#include "GTransition.h"

ClassImp(GTransition)

GTransition::GTransition(){
  Clear();
}

GTransition::~GTransition(){
  // empty
}

void GTransition::Clear(Option_t *opt){
  fEnergy          = 0;
  fEngUncertainty  = 0;
  fIntensity       = 0;
  fIntUncertainty  = 0;
}

void GTransition::Print(Option_t *opt) const{

  if(!std::isnan(fEngUncertainty))
    printf("Energy:    %.02f +/- %.02f",fEnergy,fEngUncertainty);
  else
    printf("Energy:    %.02f ",fEnergy);
  if(!std::isnan(fIntensity)) {
    if(!std::isnan(fIntUncertainty))
      printf("\tIntensity: %.02f +/- %.02f\n",fIntensity,fIntUncertainty);
    else 
     printf("\tIntensity: %.02f \n",fEnergy);
  } else {
    printf("\n");
  }

   //printf("**************************\n");
}

std::string GTransition::PrintToString(){
  std::string toString;
  toString.append(Form("%f\t",fEnergy));
  toString.append(Form("%f\t",fEngUncertainty));
  toString.append(Form("%f\t",fIntensity));
  toString.append(Form("%f\t",fIntUncertainty));

  return toString;
}

int GTransition::CompareIntensity(const TObject *obj) const {
  if(this->fIntensity > static_cast<const GTransition*>(obj)->fIntensity)
    return -1;
  else if(this->fIntensity == static_cast<const GTransition*>(obj)->fIntensity)
    return 0;
  else
    return 1;
  return -9;
}

int GTransition::Compare(const TObject* obj) const {

  return CompareIntensity(obj);
  
  //Compares the intensities of the GTransitions and returns
  if(this->fEnergy > static_cast<const GTransition*>(obj)->fEnergy)
    return -1;
  else if(this->fEnergy == static_cast<const GTransition*>(obj)->fEnergy)
    return 0;
  else
    return 1;
  return -9;
}                                  
  




















