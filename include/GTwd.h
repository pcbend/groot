#ifndef G_TWD_H
#define G_TWD_H

#include <GFileReader.h>

class GTwd : public GFileReader {
  public:
    const char* Name() const override { return "GTwd"; }
    kFileType Type() const override { return kFileType::TWD; }
    bool CanOpen(const std::string& filename) const override;
    TObject* Open(const std::string& filename, Option_t* opt = "") const override;
};

#endif
