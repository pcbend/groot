#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>

#include <TAxis.h>
#include <TH1D.h>

#include <GCalibrator.h>

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

} // namespace

int main() {
  GCalibrator calibrator;
  calibrator.AddPeak(1.0, 12.0);
  calibrator.AddPeak(2.0, 14.0);
  calibrator.AddPeak(3.0, 16.0);

  require(calibrator.Fit(1), "linear calibration fit should succeed");
  require(calibrator.HasFit(), "calibrator should report fit");
  requireNear(calibrator.GetParameter(0), 10.0, 1e-9, "linear intercept");
  requireNear(calibrator.GetParameter(1), 2.0, 1e-9, "linear slope");
  requireNear(calibrator.Eval(4.0), 18.0, 1e-9, "linear eval");
  require(calibrator.Check(), "calibration check");

  TH1D source("source", "source", 2, 0.0, 2.0);
  source.SetDirectory(nullptr);
  source.SetBinContent(1, 5.0);
  source.SetBinContent(2, 7.0);
  std::unique_ptr<TH1D> calibrated(calibrator.ApplyCalibration(&source, "cal", "cal"));
  require(calibrated != nullptr, "ApplyCalibration should produce a histogram");
  requireNear(calibrated->GetXaxis()->GetBinLowEdge(1), 10.0, 1e-9, "calibrated low edge");
  requireNear(calibrated->GetXaxis()->GetBinUpEdge(2), 14.0, 1e-9, "calibrated high edge");
  requireNear(calibrated->GetBinContent(1), 5.0, 1e-12, "calibrated bin 1 content");
  requireNear(calibrated->GetBinContent(2), 7.0, 1e-12, "calibrated bin 2 content");

  require(calibrator.SetPeakEnabled(1, false), "disable existing peak");
  require(!calibrator.HasFit(), "disabling a peak should clear fit");
  require(calibrator.Fit(1), "fit should succeed with two enabled peaks");
  requireNear(calibrator.GetParameter(0), 10.0, 1e-9, "refit intercept");
  requireNear(calibrator.GetParameter(1), 2.0, 1e-9, "refit slope");
  return 0;
}
