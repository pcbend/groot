#ifndef G_FILE_H
#define G_FILE_H

#include <string>
#include <vector>

#include <Rtypes.h>

class GFileReader;
class TObject;

class GFile {
  public:
    static TObject* Open(const std::string& filename, Option_t* opt = "");
    static const GFileReader* FindReader(const std::string& filename);
    static const std::vector<const GFileReader*>& Readers();

  private:
    static std::vector<const GFileReader*>& ReaderList();
};

#endif
