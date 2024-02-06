
#ifdef __ROOTCLING__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace GFunctions;

#pragma link C++ global gHist;

#pragma link C++ function GausFit;
#pragma link C++ function PhotoPeakFit;
#pragma link C++ function GrabHist;
#pragma link C++ function GrabFit;

#pragma link C++ function GetSignals;

#pragma link C++ class GCanvas+;
#pragma link C++ class GMarker+;
#pragma link C++ class GH1D+;
#pragma link C++ class GH2D+;

#pragma link C++ class GGaus+;
#pragma link C++ class GPeak+;

#pragma link C++ class GEventTimer;

#endif
