#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>

#include <GH1D.h>
#include <GH2D.h>

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

GH2D makeMatrix() {
  GH2D matrix("m", "m", 3, 0.0, 3.0, 3, 0.0, 3.0);
  matrix.SetDirectory(nullptr);
  for(int x = 1; x <= 3; ++x) {
    for(int y = 1; y <= 3; ++y)
      matrix.SetBinContent(x, y, 100.0 * x + 10.0 * y);
  }
  return matrix;
}

} // namespace

int main() {
  GH2D matrix = makeMatrix();

  std::unique_ptr<GH1D> px(matrix.ProjectionX(0.25, 1.75));
  require(px != nullptr, "ProjectionX should return a histogram");
  require(px->GetParent() == &matrix, "ProjectionX should remember parent");
  require(px->TestBit(GH1D::kProjectionX), "ProjectionX should mark axis bit");
  requireNear(px->GetBinContent(1), 230.0, 1e-12, "ProjectionX bin 1");
  requireNear(px->GetBinContent(2), 430.0, 1e-12, "ProjectionX bin 2");
  requireNear(px->GetBinContent(3), 630.0, 1e-12, "ProjectionX bin 3");

  std::unique_ptr<GH1D> py(matrix.ProjectionY(1.25, 2.75));
  require(py != nullptr, "ProjectionY should return a histogram");
  require(py->GetParent() == &matrix, "ProjectionY should remember parent");
  require(!py->TestBit(GH1D::kProjectionX), "ProjectionY should clear axis bit");
  requireNear(py->GetBinContent(1), 520.0, 1e-12, "ProjectionY bin 1");
  requireNear(py->GetBinContent(2), 540.0, 1e-12, "ProjectionY bin 2");
  requireNear(py->GetBinContent(3), 560.0, 1e-12, "ProjectionY bin 3");

  std::unique_ptr<GH1D> bgsub(matrix.ProjectionX(0.25, 1.75, 2.25, 2.75));
  require(bgsub != nullptr, "background-subtracted ProjectionX should return a histogram");
  requireNear(bgsub->GetBinContent(1), 100.0, 1e-12, "ProjectionX bgsub bin 1");
  requireNear(bgsub->GetBinContent(2), 200.0, 1e-12, "ProjectionX bgsub bin 2");
  requireNear(bgsub->GetBinContent(3), 300.0, 1e-12, "ProjectionX bgsub bin 3");
  return 0;
}
