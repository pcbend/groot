
#include <GUtils.h>

#include <TList.h>
#include <TMethod.h>
#include <TClass.h>
#include <TVirtualPad.h>


#include <GGlobals.h>
#include <Histomatic.h>

TList *GetSignals(const char *classname) { 
  TMethod *method     = 0;
  const char *comment = 0;
  TList *siglist = new TList();
  siglist->Clear();
  TList *methodsList = TClass::GetClass(classname)->GetListOfMethods();
  TIter nextMethod(methodsList);
  while((method = (TMethod*)nextMethod())) {
    comment = method->GetCommentString();
    if(comment && strlen(comment) && strstr(comment,"*SIGNAL"))
      siglist->Add(method);
  }
  return siglist;
}

bool GEventTimer::Notify() {
  //printf("\n gHistomatic = 0x%08x \n",gHistomatic); fflush(stdout); 
  if(gHistomatic) { 
    gHistomatic->SetStatusText("gPad",0);
    if(gPad) 
      gHistomatic->SetStatusText(gPad->GetTitle(),1);  

  }
  return TTimer::Notify(); 
}

