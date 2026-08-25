#include <cmath>
#include <cstdlib>
#include <iostream>

#include <GH1D.h>
#include <GMarker.h>
#include <GROI.h>

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

  GROI roi(2.0, 4.0, "manual_roi");
  roi.SetParent(&hist);
  require(roi.GetBinLow() == 3, "manual ROI low bin");
  require(roi.GetBinHigh() == 5, "manual ROI high bin");
  requireNear(roi.GetCounts(), 12.0, 1e-12, "manual ROI counts");

  GROI reversed(4.0, 2.0, "reversed_roi");
  reversed.SetParent(&hist);
  requireNear(reversed.GetXLow(), 2.0, 1e-12, "reversed ROI low range");
  requireNear(reversed.GetXHigh(), 4.0, 1e-12, "reversed ROI high range");
  requireNear(reversed.GetCounts(), 12.0, 1e-12, "reversed ROI counts");

  addPrimaryMarker(&hist, 1.2);
  addPrimaryMarker(&hist, 3.7);
  GROI* fromMarkers = GROI::CreateFromMarkers(&hist, "from_markers");
  require(fromMarkers != nullptr, "CreateFromMarkers should create an ROI");
  require(hist.GetListOfFunctions()->FindObject("from_markers") == fromMarkers,
          "created ROI should be attached to histogram");
  requireNear(fromMarkers->GetXLow(), 1.0, 1e-12, "marker ROI low range");
  requireNear(fromMarkers->GetXHigh(), 3.0, 1e-12, "marker ROI high range");
  requireNear(fromMarkers->GetCounts(), 9.0, 1e-12, "marker ROI counts");

  GROI::RemoveAll(&hist);
  require(hist.GetListOfFunctions()->FindObject("from_markers") == nullptr,
          "RemoveAll should remove attached ROIs");
  require(GMarker::Get(&hist, GMarkerType::kPrimary).size() == 2,
          "RemoveAll should not remove markers");
  GMarker::RemoveAll(&hist);
  return 0;
}
