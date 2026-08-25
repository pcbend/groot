#ifndef G_FILE_READER_H
#define G_FILE_READER_H

#include <string>

#include <Rtypes.h>

#include "Gtypes.h"

class TObject;

class GFileReader {
  public:
    virtual ~GFileReader() = default;

    virtual const char* Name() const = 0;
    virtual kFileType Type() const = 0;
    virtual bool CanOpen(const std::string& filename) const = 0;
    virtual TObject* Open(const std::string& filename, Option_t* opt = "") const = 0;
};

#endif
