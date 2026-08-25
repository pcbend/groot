#ifndef G_MAT_H
#define G_MAT_H

#include <GFileReader.h>

class GMat : public GFileReader {
  public:
    const char* Name() const override { return "GMat"; }
    kFileType Type() const override { return kFileType::MAT; }
    bool CanOpen(const std::string& filename) const override;
    TObject* Open(const std::string& filename, Option_t* opt = "") const override;
};

#endif
