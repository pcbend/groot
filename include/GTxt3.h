#ifndef G_TXT3_H
#define G_TXT3_H

#include <string>

#include <GFileReader.h>

class GTxt3 : public GFileReader {
  public:
    const char* Name() const override { return "GTxt3"; }
    kFileType Type() const override { return kFileType::TXT3; }
    bool CanOpen(const std::string& filename) const override;
    TObject* Open(const std::string& filename, Option_t* opt = "") const override;
};

#endif
