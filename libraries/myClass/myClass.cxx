
#include <cstdio>

#include <TString.h>

#include <myClass.h>

myClass::myClass() {
  printf("myClass constructed.\n");
}

myClass::~myClass() {
  printf("myClass destroyed.\n");
}

void myClass::print() {
  TString s = "hello from root";

  printf("hello from myClassi:\t%s.\n",s.Data());
}

