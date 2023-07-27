#ifndef __MYCLASS_H__
#define __MYCLASS_H__

#include <string>

#include <TObject.h>

class myClass : public TObject {
  public:
    myClass();
    ~myClass();

    void print();

  private:
    std::string fName;

  ClassDef(myClass,1)

};

#endif
