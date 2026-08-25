#include <GFile.h>

#include <iostream>

#include <GFileReader.h>
#include <GMat.h>
#include <GNsm.h>
#include <GRootFile.h>
#include <GSpe.h>
#include <GTwd.h>
#include <GTxt3.h>

std::vector<const GFileReader*>& GFile::ReaderList() {
  static GRootFile rootFile;
  static GTxt3 txt3;
  static GSpe spe;
  static GMat mat;
  static GTwd twd;
  static GNsm nsm;

  static std::vector<const GFileReader*> readers = {
    &rootFile,
    &txt3,
    &spe,
    &mat,
    &twd,
    &nsm
  };
  return readers;
}

const std::vector<const GFileReader*>& GFile::Readers() {
  return ReaderList();
}

const GFileReader* GFile::FindReader(const std::string& filename) {
  for(const auto* reader : ReaderList()) {
    if(reader && reader->CanOpen(filename))
      return reader;
  }
  return nullptr;
}

TObject* GFile::Open(const std::string& filename, Option_t* opt) {
  const auto* reader = FindReader(filename);
  if(!reader) {
    std::cout << "\tDiscarding unknown file: " << filename << std::endl;
    return nullptr;
  }
  return reader->Open(filename, opt);
}
