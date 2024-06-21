

void makeGraph(const char *fname="data2.dat") {

  gStyle->SetEndErrorSize(2);
  gStyle->SetErrorX(.05);

  TGraphErrors *gr = new TGraphErrors(fname,"%lg %lg %lg %lg");
  gr->SetMarkerStyle(20);
  gr->SetLineWidth(2);

  gr->GetXaxis()->SetBinLabel(gr->GetXaxis()->FindBin(1 + .1), "blank");
  gr->GetXaxis()->SetBinLabel(gr->GetXaxis()->FindBin(2 + .1), "# 1");
  gr->GetXaxis()->SetBinLabel(gr->GetXaxis()->FindBin(3 + .1), "# 2");
  gr->GetXaxis()->SetBinLabel(gr->GetXaxis()->FindBin(4 + .1), "# 3");
  gr->GetXaxis()->SetBinLabel(gr->GetXaxis()->FindBin(5 + .1), "# 4");
  gr->GetXaxis()->SetBinLabel(gr->GetXaxis()->FindBin(6 + .1), "# 5");
  gr->GetXaxis()->SetBinLabel(gr->GetXaxis()->FindBin(7 + .1), "# 6");
  gr->GetXaxis()->SetBinLabel(gr->GetXaxis()->FindBin(8 + .1), "# 7");
  gr->GetXaxis()->SetBinLabel(gr->GetXaxis()->FindBin(9 + .1), "# 8");
  gr->GetXaxis()->SetBinLabel(gr->GetXaxis()->FindBin(10 + .1), "# 9");
  gr->GetXaxis()->SetBinLabel(gr->GetXaxis()->FindBin(11 + .1), "# 10");
  gr->GetXaxis()->SetBinLabel(gr->GetXaxis()->FindBin(12 + .1), "# 11");

  gr->GetXaxis()->SetLabelSize(0.06);
  gr->GetXaxis()->LabelsOption("v");

  gr->SetTitle("");

  gr->GetYaxis()->SetTitle("F concentration (arb units)");
  gr->GetYaxis()->CenterTitle();

  int n = 5;
  double xZero[5] = {-1,           15,            15,           -1,          -1};
  double yZero[5] = {0.0219+.0028, 0.0219+.0028 , 0.0219-.0028, 0.0219-.0028, 0.0219+.0028};

  double x1ppm[5] = {-1,           15,            15,           -1,          -1};
  double y1ppm[5] = {0.0303+.0039, 0.0303+.0039, 0.0303-.0039, 0.0303-.0039, 0.0303+.0039};
  
  double x10ppm[5] = {-1,           15,            15,           -1,          -1};
  double y10ppm[5] = {0.0914+.0096, 0.0914+.0096, 0.0914-.0096, 0.0914-.0096, 0.0914+.0096};

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

  TGraph *ppm10 = new TGraph(n,x10ppm,y10ppm);
  ppm10->SetLineColor(8);
  ppm10->SetLineWidth(-2002);
  ppm10->SetFillStyle(3004);
  ppm10->SetFillColor(8);



  gr->Draw("AP");
  zero->Draw("same F");
  ppm1->Draw("same F");
  ppm10->Draw("same F");
}



