
#ifdef __ROOTCLING__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace GFunctions;
#pragma link C++ namespace GGuiHistory;

#pragma link C++ global gHist;
#pragma link C++ global gHistomatic;

#pragma link C++ function GausFit;
#pragma link C++ function AutoFitPeaks;
#pragma link C++ function PhotoPeakFit;
#pragma link C++ function GrabHist;
#pragma link C++ function GrabFit;
#pragma link C++ function GrabPlottable;

#pragma link C++ function ResidualHist;
#pragma link C++ function DrawResiduals;

#pragma link C++ function ls;

#pragma link C++ function SaveAllCuts;

#pragma link C++ function GetSignals;

#pragma link C++ function CloseAllCanvases;

#pragma link C++ function GRootInteract;

#pragma link C++ function GGuiHistory::IsEnabled;
#pragma link C++ function GGuiHistory::SetEnabled;
#pragma link C++ function GGuiHistory::Path;
#pragma link C++ function GGuiHistory::SetPath;
#pragma link C++ function GGuiHistory::Clear;
#pragma link C++ function GGuiHistory::Recent;
#pragma link C++ function GGuiHistory::Summary;
#pragma link C++ function GGuiHistory::Print;
#pragma link C++ function gui_history;

#pragma link C++ class GCanvas+;
#pragma link C++ class GMarker+;
#pragma link C++ class GH1D+;
#pragma link C++ class GH2D+;

#pragma link C++ class GF1+;
#pragma link C++ class GGaus+;
#pragma link C++ class GDoubleGaus+;
#pragma link C++ class GPeak+;
#pragma link C++ class GPeak2+;
#pragma link C++ class GROI+;
#pragma link C++ class GCalibrator+;

#endif
