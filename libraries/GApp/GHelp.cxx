#include <GHelp.h>

#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

std::string Trim(std::string value) {
  auto isSpace = [](unsigned char c) { return std::isspace(c); };
  value.erase(value.begin(), std::find_if(value.begin(), value.end(),
                                          [&](unsigned char c) { return !isSpace(c); }));
  value.erase(std::find_if(value.rbegin(), value.rend(),
                           [&](unsigned char c) { return !isSpace(c); }).base(),
              value.end());
  return value;
}

std::string Lower(std::string value) {
  std::transform(value.begin(), value.end(), value.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return value;
}

std::vector<std::string> Tokens(std::string line) {
  line = Trim(line);
  if(line.rfind("#line", 0) == 0) {
    const auto newline = line.find('\n');
    if(newline != std::string::npos)
      line = Trim(line.substr(newline + 1));
  }

  while(!line.empty() && line.back() == ';')
    line.pop_back();
  line = Trim(line);

  std::vector<std::string> tokens;
  std::istringstream stream(line);
  std::string token;
  while(stream >> token)
    tokens.push_back(Lower(token));
  return tokens;
}

std::string Topic(const std::string& line) {
  const auto tokens = Tokens(line);
  if(tokens.empty())
    return "";
  if(tokens[0] == "?") {
    return tokens.size() > 1 ? tokens[1] : "";
  }
  if(tokens[0] == "help") {
    return tokens.size() > 1 ? tokens[1] : "";
  }
  return "";
}

void PrintOverview(std::ostream& out) {
  out
    << "groot help\n"
    << "\n"
    << "Canvas:\n"
    << "  click        add marker\n"
    << "  ctrl-click   add fit/cut marker in GCanvas\n"
    << "  e            expand between markers\n"
    << "  m            remove markers\n"
    << "  o            unzoom\n"
    << "  l,z          toggle log scale\n"
    << "\n"
    << "1D:\n"
    << "  g            gaussian fit between markers\n"
    << "  f            photopeak fit\n"
    << "  w/q          rebin/unbin\n"
    << "  b/B          show/toggle background\n"
    << "  r            toggle residuals\n"
    << "  s            show searched peaks\n"
    << "\n"
    << "2D:\n"
    << "  x/y          project onto X/Y\n"
    << "  c            start cut or set projection background\n"
    << "  g            create cut from cut markers\n"
    << "\n"
    << "Projection:\n"
    << "  p            project parent matrix through marker range\n"
    << "  c            set background range for p\n"
    << "\n"
    << "Try: help commands, help markers, help fitting, help projections, help journal, help root\n";
}

void PrintCommands(std::ostream& out) {
  out
    << "groot command summary\n"
    << "\n"
    << "Mouse:\n"
    << "  click                primary marker\n"
    << "  ctrl-click           fit marker on 1D, cut marker on 2D in GCanvas\n"
    << "  shift-click          draw-copy in a new canvas\n"
    << "\n"
    << "Keys:\n"
    << "  b/B                  show/toggle background on GH1D\n"
    << "  c                    projection background range, or 2D cut mode\n"
    << "  e                    expand/zoom between markers\n"
    << "  f                    photopeak fit on 1D\n"
    << "  g                    gaussian fit on 1D, or TCutG creation on 2D\n"
    << "  l,z                  toggle log Y/Z\n"
    << "  m                    remove markers\n"
    << "  n                    clear fits, markers, residuals, shown peaks\n"
    << "  o                    unzoom\n"
    << "  p                    project parent matrix from a 1D projection\n"
    << "  q/w                  unbin/rebin on GH1D\n"
    << "  r                    toggle residual display\n"
    << "  s                    show searched peaks\n"
    << "  x/y                  make GH2D X/Y projection\n"
    << "  X                    use ROOT TH2::SetShowProjectionX\n";
}

void PrintRoi(std::ostream& out) {
  out
    << "groot ROI help\n"
    << "\n"
    << "ROI support currently exists as a GROI object/API feature, not as a\n"
    << "key-bound interactive mode. The active 1D keyboard workflow is fitting and\n"
    << "residual inspection: g/f create fits, and r toggles residual display.\n"
    << "\n"
    << "Useful API entry points:\n"
    << "  GROI roi(xlow, xhigh)\n"
    << "  GROI::CreateFromMarkers(hist)\n"
    << "  GROI::RemoveAll(hist)\n";
}

void PrintMarkers(std::ostream& out) {
  out
    << "groot marker help\n"
    << "\n"
    << "  click                add a primary marker\n"
    << "  ctrl-click on 1D     add a fit marker in GCanvas\n"
    << "  ctrl-click on 2D     add a cut marker in GCanvas\n"
    << "  m                    remove markers\n"
    << "\n"
    << "Primary markers usually define ranges for zooming, fitting, gates, and\n"
    << "projections. Fit markers are used for double-Gaussian fits. Background\n"
    << "markers on projections are set with c after selecting two primary markers.\n";
}

void PrintFitting(std::ostream& out) {
  out
    << "groot fitting help\n"
    << "\n"
    << "  1. Place two primary markers around a 1D peak.\n"
    << "  2. Press g for GausFit(), or f for PhotoPeakFit().\n"
    << "  3. Use r to toggle residual display on GH1D.\n"
    << "\n"
    << "For a double-Gaussian fit, place four fit markers with Ctrl-click in a\n"
    << "GCanvas and press g.\n";
}

void PrintProjections(std::ostream& out) {
  out
    << "groot projection help\n"
    << "\n"
    << "  x/y                  make a full GH2D X/Y projection\n"
    << "  p                    from a 1D projection, project the parent GH2D\n"
    << "                       through the two primary markers on the opposite axis\n"
    << "  c                    on a 1D projection, convert two primary markers into\n"
    << "                       the background range used by p\n"
    << "\n"
    << "Current background subtraction preserves the selected gate ranges and is not\n"
    << "normalized by gate width.\n";
}

void PrintRoot(std::ostream& out) {
  out
    << "groot in a normal ROOT session\n"
    << "\n"
    << "Load the libraries, draw a histogram, and add the interaction TExec:\n"
    << "\n"
    << "  gSystem->Load(\"libGTOOLS\");\n"
    << "  gSystem->Load(\"libGROOTLIB\");\n"
    << "  auto c = new TCanvas;\n"
    << "  h->Draw();\n"
    << "  c->AddExec(\"groot_interact\", \"GRootInteract()\");\n"
    << "\n"
    << "GCanvas adds this exec automatically. Arrow keys, space, and modifier-key\n"
    << "tracking are GCanvas behavior, not plain TExec behavior.\n";
}

void PrintJournal(std::ostream& out) {
  out
    << "groot GUI history help\n"
    << "\n"
    << "Semantic GUI actions are appended to .gui_history as JSON Lines by default. This is\n"
    << "separate from .gint_history, which records prompt input.\n"
    << "\n"
    << "Useful controls:\n"
    << "  GGuiHistory::SetEnabled(false)\n"
    << "  GGuiHistory::SetEnabled(true)\n"
    << "  GGuiHistory::SetPath(\"my.gui_history\")\n"
    << "  GGuiHistory::Clear()\n"
    << "\n"
    << "Configuration keys:\n"
    << "  Gui.HistSize: 10000\n"
    << "  GGuiHistory.File: .gui_history\n";
}

} // namespace

namespace GHelp {

bool IsHelpCommand(const std::string& line) {
  const auto tokens = Tokens(line);
  if(tokens.empty())
    return false;
  return tokens[0] == "?" || tokens[0] == "help";
}

void Print(const std::string& line, std::ostream& out) {
  const std::string topic = Topic(line);
  if(topic.empty() || topic == "overview" || topic == "short") {
    PrintOverview(out);
  } else if(topic == "commands" || topic == "keys" || topic == "key") {
    PrintCommands(out);
  } else if(topic == "markers" || topic == "marker") {
    PrintMarkers(out);
  } else if(topic == "fitting" || topic == "fits" || topic == "fit") {
    PrintFitting(out);
  } else if(topic == "roi" || topic == "rois" || topic == "region") {
    PrintRoi(out);
  } else if(topic == "projections" || topic == "projection" || topic == "2d") {
    PrintProjections(out);
  } else if(topic == "journal" || topic == "history" || topic == "gui_history") {
    PrintJournal(out);
  } else if(topic == "root" || topic == "texec" || topic == "tcanvas") {
    PrintRoot(out);
  } else {
    out << "Unknown help topic: " << topic << "\n\n";
    PrintOverview(out);
  }
}

void Print(const std::string& line) {
  Print(line, std::cout);
}

} // namespace GHelp
