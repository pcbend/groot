#ifndef G_ROOT_FILE_H
#define G_ROOT_FILE_H

#include <GFileReader.h>

class GRootFile : public GFileReader {
  public:
    const char* Name() const override { return "GRootFile"; }
    kFileType Type() const override { return kFileType::ROOTFILE; }
    bool CanOpen(const std::string& filename) const override;
    TObject* Open(const std::string& filename, Option_t* opt = "") const override;
};

#endif
