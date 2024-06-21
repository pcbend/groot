#ifndef __GINT_H__
#define __GINT_H__

#ifndef __CINT__
#include <thread>
#endif

#include <TRint.h>

#include <Gtypes.h>
#include <GFunctions.h>

class TFile;

class Gint : public TRint {
  private:
    Gint(int argc, char **argv);
    static Gint *fGint; 
  public:
    static Gint *Get(int argc=0,char **argv=0);
    virtual ~Gint();
  
    int  TabCompletionHook(char* buf, int* pLoc, std::ostream& out) override;
    long ProcessLine(const char *line, bool sync=true,int *error=0) override;

    void Terminate(int status) override;

  public:
    void      LoadOptions(int argc, char **argv);
    kFileType DetermineFileType(const std::string& filename) const;
    //bool      FileAutoDetect(const std::string& filename);
    TFile*    OpenRootFile(const std::string& filename, Option_t *opt="");

    void      LoadStyle();    

  private:
    int fRootFilesOpened;
    bool fTabLock;

#ifndef __CINT__
    std::thread::id fMainThreadId;
#endif

  ClassDefOverride(Gint,0)
};


#endif

