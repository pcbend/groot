#ifndef G_SPE_H
#define G_SPE_H

#include <GFileReader.h>

class GSpe : public GFileReader {
  public:
    const char* Name() const override { return "GSpe"; }
    kFileType Type() const override { return kFileType::SPE; }
    bool CanOpen(const std::string& filename) const override;
    TObject* Open(const std::string& filename, Option_t* opt = "") const override;
};

#endif
