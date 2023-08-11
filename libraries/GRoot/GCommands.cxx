
#include <GCommands.h>

#include<TVirtualPad.h>
#include<TList.h>
#include<TH1.h>
#include<TF1.h>


TH1 *GrabHist(int i)  {
  //return the histogram from the current canvas, pad i.
  TH1 *hist = 0;
  if(!gPad)
    return hist;
  TIter iter(gPad->GetListOfPrimitives());
  int j=0;
  while(TObject *obj = iter.Next()) {
    if(obj->InheritsFrom(TH1::Class())) {
      if(j==i) {
        hist = (TH1*)obj;
        break;
      }
      j++;
    }
  }
  return hist;
}

TF1 *GrabFit(int i)  {
  //return the histogram from the current canvas, pad i.
  TH1 *hist = 0;
  TF1 *fit = 0;
  if(!gPad)
    return fit;
  TIter iter(gPad->GetListOfPrimitives());
  int j=0;
  while(TObject *obj = iter.Next()) {
    if(obj->InheritsFrom(TH1::Class())) {
      hist = (TH1*)obj;
      TIter iter2(hist->GetListOfFunctions());
      while(TObject *obj2 = iter2.Next()){
	if(obj2->InheritsFrom(TF1::Class())){
	  if(j==i) {
	    fit=(TF1*)obj2;
	    return fit;
	  }
	  j++;
	}
      }
    }
  }
  return fit;
}

