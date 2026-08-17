#ifndef __GOPTIONS_H__
#define __GOPTIONS_H__

#include <iosfwd>
#include <string>
#include <vector>

#include <Gtypes.h>

class GOptions {
public:
  struct Input {
    enum class Type {
      Unknown,
      File,
      Macro,
      Calibration,
      Cuts,
      LiveUrl
    };

    std::string value;
    Type type = Type::Unknown;
  };

  GOptions();
  GOptions(int argc, char** argv);

  bool Parse(int argc, char** argv);

  bool ShowHelp() const { return fShowHelp; }
  bool ShowVersion() const { return fShowVersion; }
  bool StartGui() const { return fStartGui; }
  bool ParseSucceeded() const { return fParseSucceeded; }

  const std::vector<Input>& Inputs() const { return fInputs; }
  const std::string& Error() const { return fError; }
  const std::string& HelpText() const { return fHelpText; }

  static Input::Type DetermineInputType(const std::string& input);
  static kFileType DetermineFileType(const std::string& filename);
  static bool IsLiveUrl(const std::string& input);

private:
  bool fShowHelp;
  bool fShowVersion;
  bool fStartGui;
  bool fParseSucceeded;
  std::string fError;
  std::string fHelpText;
  std::vector<Input> fInputs;
};

#endif
