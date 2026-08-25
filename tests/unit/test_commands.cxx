#include <cmath>
#include <cstdlib>
#include <iostream>

#include <KeySymbols.h>

#include <GCommands.h>
#include <GH1D.h>
#include <GMarker.h>

namespace {

void require(bool condition, const char* message) {
  if(!condition) {
    std::cerr << "FAILED: " << message << '\n';
    std::exit(1);
  }
}

void requireNear(double actual, double expected, double tolerance, const char* message) {
  if(std::abs(actual - expected) > tolerance) {
    std::cerr << "FAILED: " << message << " expected " << expected
              << " got " << actual << '\n';
    std::exit(1);
  }
}

void addPrimaryMarker(TH1* hist, double x) {
  auto* marker = new GMarker;
  marker->AddTo(hist, x);
}

} // namespace

int main() {
  GH1D hist("h", "h", 10, 0.0, 10.0);
  hist.SetDirectory(nullptr);
  for(int bin = 1; bin <= hist.GetNbinsX(); ++bin)
    hist.SetBinContent(bin, bin);

  addPrimaryMarker(&hist, 2.2);
  addPrimaryMarker(&hist, 6.8);
  require(GMarker::Get(&hist, GMarkerType::kPrimary).size() == 2, "markers added");

  GInteractionInfo zoom;
  zoom.py = kKey_e;
  require(GRootInteractHistKeyPress(&hist, zoom), "zoom key dispatch");
  require(zoom.modified, "zoom should mark interaction modified");
  requireNear(hist.GetXaxis()->GetBinLowEdge(hist.GetXaxis()->GetFirst()), 2.0, 1e-12,
              "zoom lower edge");
  requireNear(hist.GetXaxis()->GetBinUpEdge(hist.GetXaxis()->GetLast()), 6.0, 1e-12,
              "zoom upper edge");
  require(GMarker::Get(&hist, GMarkerType::kPrimary).empty(), "zoom should remove markers");

  addPrimaryMarker(&hist, 1.0);
  addPrimaryMarker(&hist, 8.0);
  GInteractionInfo remove;
  remove.py = kKey_m;
  require(GRootInteractHistKeyPress(&hist, remove), "remove marker key dispatch");
  require(remove.modified, "remove markers should mark interaction modified");
  require(GMarker::Get(&hist, GMarkerType::kAll).empty(), "m should remove markers");
  return 0;
}
