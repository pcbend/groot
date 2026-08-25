#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <unistd.h>

#include <KeySymbols.h>
#include <TEnv.h>

#include <GCommands.h>
#include <GH1D.h>
#include <GMarker.h>
#include <GGuiHistory.h>

namespace {

void require(bool condition, const char* message) {
  if(!condition) {
    std::cerr << "FAILED: " << message << '\n';
    std::exit(1);
  }
}

void requireContains(const std::string& text,
                     const std::string& needle,
                     const char* message) {
  if(text.find(needle) == std::string::npos) {
    std::cerr << "FAILED: " << message << "\nmissing: " << needle
              << "\noutput:\n" << text << '\n';
    std::exit(1);
  }
}

std::string ReadFile(const std::string& path) {
  std::ifstream input(path);
  std::ostringstream out;
  out << input.rdbuf();
  return out.str();
}

void addPrimaryMarker(TH1* hist, double x) {
  auto* marker = new GMarker;
  marker->AddTo(hist, x);
}

} // namespace

int main() {
  const std::string path = std::string("/private/tmp/groot_gui_history_") +
                           std::to_string(getpid()) + ".gui_history";
  gEnv->SetValue("Gui.HistSize", 10000);
  GGuiHistory::SetPath(path);
  GGuiHistory::SetEnabled(true);
  GGuiHistory::Clear();

  GInteractionInfo direct;
  direct.targetName = "h";
  direct.py = kKey_g;
  GGuiHistory::Record("fit.gaus", direct, {
    {"hist", "h"},
    {"xlow", "1"},
    {"xhigh", "3"}
  });

  std::string text = ReadFile(path);
  requireContains(text, "\"action\":\"fit.gaus\"", "direct action");
  requireContains(text, "\"object\":", "direct object section");
  requireContains(text, "\"event\":", "direct event section");
  requireContains(text, "\"range\":", "direct range section");
  requireContains(text, "\"projection\":", "direct projection section");
  requireContains(text, "\"fit\":", "direct fit section");
  requireContains(text, "\"marker\":", "direct marker section");
  requireContains(text, "\"display\":", "direct display section");
  requireContains(text, "\"extra\":", "direct extra section");
  requireContains(text, "\"name\":\"h\"", "direct target");
  requireContains(text, "\"key\":\"g\"", "direct key");
  requireContains(text, "\"xlow\":1", "direct field");

  GH1D hist("h", "h", 10, 0.0, 10.0);
  hist.SetDirectory(nullptr);
  for(int bin = 1; bin <= hist.GetNbinsX(); ++bin)
    hist.SetBinContent(bin, bin);

  addPrimaryMarker(&hist, 2.2);
  addPrimaryMarker(&hist, 6.8);
  GInteractionInfo zoom;
  zoom.py = kKey_e;
  zoom.target = &hist;
  zoom.targetName = hist.GetName();
  require(GRootInteractHistKeyPress(&hist, zoom), "zoom dispatch");

  text = ReadFile(path);
  requireContains(text, "\"action\":\"hist.zoom\"", "zoom action");
  requireContains(text, "\"name\":\"h\"", "zoom hist field");
  requireContains(text, "\"xlow\":2", "zoom low field");
  requireContains(text, "\"xhigh\":6", "zoom high field");

  GGuiHistory::SetEnabled(false);
  GGuiHistory::Record("disabled.action");
  const std::string afterDisabled = ReadFile(path);
  require(afterDisabled == text, "disabled journal should not append");

  GGuiHistory::SetEnabled(true);
  GGuiHistory::Clear();
  gEnv->SetValue("Gui.HistSize", 2);
  GGuiHistory::Record("history.one");
  GGuiHistory::Record("history.two");
  GGuiHistory::Record("history.three");

  text = ReadFile(path);
  require(text.find("\"action\":\"history.one\"") == std::string::npos,
          "Gui.HistSize should trim oldest line");
  requireContains(text, "\"action\":\"history.two\"", "Gui.HistSize should keep second line");
  requireContains(text, "\"action\":\"history.three\"", "Gui.HistSize should keep newest line");

  std::remove(path.c_str());
  return 0;
}
