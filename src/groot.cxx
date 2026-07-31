
#include<cstdio>

#include<Gint.h>
#include<utils.h>

#include<TEnv.h>
#include<TStyle.h>

void loadEnv();
void loadStyle();

int main(int argc, char **argv) {
  loadEnv();
  loadStyle();
  Gint::Get(argc,argv)->Run(true);
  return 0;
}






void loadEnv() {
  // Set the GSYS variable based on the executable path.
  // If GSYS has already been defined, don't overwrite.
  setenv("GSYS", (programPath()+"/..").c_str(), 0);

  printf("GSYS has been set to: %s\n",getenv("GSYS"));

  // Load $GSYS/.gintrc
  std::string grut_path = Form("%s/.gintrc",getenv("GSYS"));
  gEnv->ReadFile(grut_path.c_str(),kEnvChange);
  // Load $HOME/.gintrc
  grut_path = Form("%s/.gintrc",getenv("HOME"));
  if(fileExists(grut_path.c_str())){
    gEnv->ReadFile(grut_path.c_str(),kEnvChange);
  }
  
  // Load $PWD/.gintrc
  grut_path = Form("%s/.gintrc",getenv("PWD"));
  if(fileExists(grut_path.c_str())){
    gEnv->ReadFile(grut_path.c_str(),kEnvChange);
  }
}

void loadStyle() {
  // Load the ROOT style file
  //gStyle->SetPalette(kVisibleSpectrum);
  gStyle->SetPalette(gEnv->GetValue("Gint.Style.Palette",kVisibleSpectrum));
  gStyle->SetNumberContours(gEnv->GetValue("Gint.Style.Contours",32));

  gStyle->SetHistLineWidth(2);
  gStyle->SetHistFillStyle(0);

  gStyle->SetFrameBorderMode(1);
  //gStyle->SetFrameFillColor(1);
  gStyle->SetCanvasBorderMode(1);
  gStyle->SetPadBorderMode(1);
  gStyle->SetPadBorderSize(5);
  //gStyle->SetPadColor(2);
  //canvas.SetBorderSize(6);
  //canvas.SetHighLightColor(kBlue); //or whatever color
  //Hists.Stats: "nemri"
  gStyle->SetOptStat(gEnv->GetValue("Hists.Stats","nemri"));
  //gEnv->Print();

  std::string theme = gEnv->GetValue("Gint.Style.Theme","light");
  if(theme.compare("dark")==0) {

    gEnv->SetValue("Gui.Style","modern-flat");
    //Zone 1 -> App frame, Bottom Bar, Checkbox 
    gEnv->SetValue("Gui.BackgroundColor", "#35363A");
    gEnv->SetValue("Gui.ForegroundColor", "#E8EAED");

    //Zone 2 -> action buttons, active highlight tones
    gEnv->SetValue("Gui.HighLightColor", "#202124");

    //Zone 3 -> file tree panel
    gEnv->SetValue("Gui.DocumentBackgroundColor", "#202124");
    gEnv->SetValue("Gui.DocumentForegroundColor", "#FFFFFF");
    
    gEnv->SetValue("Gui.SelectBackgroundColor", "#4A4D51");
    gEnv->SetValue("Gui.SelectForegroundColor", "#FFFFFF");

    //tooltips.
    gEnv->SetValue("Gui.TooltipBackgroundColor", "#35363A");
    gEnv->SetValue("Gui.TooltipForegroundColor", "#E8EAED");



    //gStyle->SetCanvasColor(923);  // kGrey +3 
    //gStyle->SetPadColor(gEnv->GetValue("Gint.Style.PadColor",0));
    //gStyle->SetFrameFillColor(gEnv->GetValue("Gint.Style.FrameColor",0));

    //gStyle->SetTextColor(gEnv->GetValue("Gint.Style.TextColor",1));
    //gStyle->SetLabelColor(gEnv->GetValue("Gint.Style.LabelColor",1),"XYZ");
    //gStyle->SetTitleColor(gEnv->GetValue("Gint.Style.TitleColor",1),"XYZ");
    //gStyle->SetAxisColor(gEnv->GetValue("Gint.Style.AxisColor",1),"XYZ");
  }


  //gROOT->ForceStyle();

}
