

void calGraph(const char *fname="cal4.dat") {

  gStyle->SetEndErrorSize(2);
  gStyle->SetErrorX(.05);

  TGraphErrors *gr = new TGraphErrors(fname,"%lg %lg %lg %lg");
  gr->SetMarkerStyle(2);
  gr->SetLineWidth(2);

//  gr->GetXaxis()->SetBinLabel(gr->GetXaxis()->FindBin(1 + .1), "blank");
//  gr->GetXaxis()->SetBinLabel(gr->GetXaxis()->FindBin(2 + .1), "sample 1");
//  gr->GetXaxis()->SetBinLabel(gr->GetXaxis()->FindBin(3 + .1), "sample 2");
//  gr->GetXaxis()->SetBinLabel(gr->GetXaxis()->FindBin(4 + .1), "sample 3");
//  gr->GetXaxis()->SetBinLabel(gr->GetXaxis()->FindBin(5 + .1), "sample 4");

  //for(int i=1;i<12;i++) {
  //  gr->GetXaxis()->SetBinLabel(gr->GetXaxis()->FindBin(i + .1), Form("%i",i-1));
  //}
  //gr->GetXaxis()->SetLabelSize(0.06);
  //gr->GetXaxis()->LabelsOption("h");

  gr->GetXaxis()->SetTitle("F concentration (ppm)");
  gr->GetXaxis()->CenterTitle();

  gr->GetXaxis()->SetCanExtend(true);
  gr->GetXaxis()->SetRangeUser(-1,15);

  //gr->GetXaxis()->SetNdivisions(0);

  gr->SetTitle("");

  gr->GetYaxis()->SetTitle("F concentration (arb units)");
  gr->GetYaxis()->CenterTitle();
/*
  int n = 5;
  double xZero[5] = {-1,           10,            10,           -1,          -1};
  double yZero[5] = {0.0202+.0022, 0.0202+.0022 , 0.0202-.0022, 0.0202-.0022, 0.0202+.0022};

  double x1ppm[5] = {-1,           10,            10,           -1,          -1};
  double y1ppm[5] = {0.0276+.003, 0.0276+.003 , 0.0276-.003, 0.0276-.003, 0.0276+.003};

  TGraph *zero = new TGraph(n,xZero,yZero);
  zero->SetLineColor(4);
  zero->SetLineWidth(-2002);
  zero->SetFillStyle(3004);
  zero->SetFillColor(4);

  TGraph *ppm1 = new TGraph(n,x1ppm,y1ppm);
  ppm1->SetLineColor(2);
  ppm1->SetLineWidth(-2002);
  ppm1->SetFillStyle(3004);
  ppm1->SetFillColor(2);
*/


  gr->Draw("AP");
  //zero->Draw("same");
  //ppm1->Draw("same");

  gr->Fit("pol1");


}



