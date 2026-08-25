#ifndef G_NSM_H
#define G_NSM_H

#include <GFileReader.h>

class GNsm : public GFileReader {
  public:
    const char* Name() const override { return "GNsm"; }
    kFileType Type() const override { return kFileType::NSM; }
    bool CanOpen(const std::string& filename) const override;
    TObject* Open(const std::string& filename, Option_t* opt = "") const override;
};

#endif
