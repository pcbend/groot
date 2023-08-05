#ifndef __GINT_H__
#define __GINT_H__

#include <Gtypes.h>

#include <TRint.h>

class TFile;

class Gint : public TRint {
  private:
    Gint(int argc, char **argv);
    static Gint *fGint; 
  public:
    static Gint *Get(int argc=0,char **argv=0);
    virtual ~Gint();



  public:
    void      LoadOptions(int argc, char **argv);
    kFileType DetermineFileType(const std::string& filename) const;
    //bool      FileAutoDetect(const std::string& filename);
    TFile*    OpenRootFile(const std::string& filename, Option_t *opt="");

  private:
    int fRootFilesOpened;


  ClassDef(Gint,0)
};


#endif

