#include <GOptions.h>

#include <sstream>

#include <argParser.h>

GOptions::GOptions()
  : fShowHelp(false), fShowVersion(false), fStartGui(false),
    fParseSucceeded(true) {
}

GOptions::GOptions(int argc, char** argv)
  : GOptions() {
  Parse(argc, argv);
}

bool GOptions::Parse(int argc, char** argv) {
  fShowHelp = false;
  fShowVersion = false;
  fStartGui = false;
  fParseSucceeded = true;
  fError.clear();
  fHelpText.clear();
  fInputs.clear();

  argParser parser;
  std::vector<std::string> input_files;

  parser.default_option(&input_files)
    .description("Input file(s)");
  parser.option("h help ?", &fShowHelp)
    .description("Show this help Message")
    .default_value(false);
  parser.option("g gui", &fStartGui)
    .description("Start the GUI")
    .default_value(false);
  parser.option("v version", &fShowVersion)
    .description("Show version")
    .default_value(false);

  try {
    parser.parse(argc, argv);
  } catch(ParseError& e) {
    std::ostringstream error;
    error << "ERROR: " << e.what() << "\n" << parser;
    fError = error.str();
    fParseSucceeded = false;
  }

  std::ostringstream help;
  help << parser;
  fHelpText = help.str();

  for(const std::string& input : input_files) {
    Input option_input;
    option_input.value = input;
    option_input.type = DetermineInputType(input);
    fInputs.push_back(option_input);
  }

  return fParseSucceeded;
}

GOptions::Input::Type GOptions::DetermineInputType(const std::string& input) {
  if(IsLiveUrl(input)) {
    return Input::Type::LiveUrl;
  }

  switch(DetermineFileType(input)) {
    case kFileType::ROOTFILE:
    case kFileType::TXT3:
    case kFileType::SPE:
    case kFileType::MAT:
    case kFileType::TWD:
    case kFileType::NSM:
      return Input::Type::File;
    case kFileType::MACRO:
      return Input::Type::Macro;
    case kFileType::CALIBRATION:
      return Input::Type::Calibration;
    case kFileType::CUTS:
      return Input::Type::Cuts;
    default:
      return Input::Type::Unknown;
  }
}

kFileType GOptions::DetermineFileType(const std::string& filename) {
  size_t dot = filename.find_last_of('.');
  if(dot == std::string::npos) {
    return kFileType::UNKNOWN;
  }
  std::string ext = filename.substr(dot + 1);

  if((ext == "gz") || (ext == "bz2") || (ext == "zip")) {
    std::string remaining = filename.substr(0, dot);
    ext = remaining.substr(remaining.find_last_of('.') + 1);
  }

  if(ext == "cal") {
    return kFileType::CALIBRATION;
  } else if(ext == "root") {
    return kFileType::ROOTFILE;
  } else if((ext == "c") || (ext == "C") ||
            (ext == "c+") || (ext == "C+") ||
            (ext == "c++") || (ext == "C++")) {
    return kFileType::MACRO;
  } else if(ext == "cuts") {
    return kFileType::CUTS;
  } else if(ext == "txt3") {
    return kFileType::TXT3;
  } else if(ext == "spe") {
    return kFileType::SPE;
  } else if(ext == "mat") {
    return kFileType::MAT;
  } else if(ext == "twd" || ext == "sqr") {
    return kFileType::TWD;
  } else if(ext == "nsm") {
    return kFileType::NSM;
  }

  return kFileType::UNKNOWN;
}

bool GOptions::IsLiveUrl(const std::string& input) {
  return input.compare(0, 7, "http://") == 0 ||
         input.compare(0, 5, "ws://") == 0;
}
