

void graph1(const char *fname="cal3.dat") {

  gStyle->SetEndErrorSize(2);
  gStyle->SetErrorX(.05);

  TGraphErrors *gr = new TGraphErrors(fname,"%lg %lg %lg %lg");
  gr->SetMarkerStyle(2);
  gr->SetLineWidth(2);

  double xmin = 0;
  double xmax =110;

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
  gr->GetXaxis()->SetRangeUser(xmin,xmax);

  //gr->GetXaxis()->SetNdivisions(0);

  gr->SetTitle("");

  gr->GetYaxis()->SetTitle("F concentration (arb units)");
  gr->GetYaxis()->CenterTitle();

  int n = 5;
  double xlow = xmin;
  double xhigh = xmax;
  double y = 0.410;
  double err = 0.043;
  double xSample[5] = {xlow, xhigh, xhigh, xlow,};
  double ySample[5] = {y+err,y+err,y-err,y-err,};


  TGraph *samp = new TGraph(n,xSample,ySample);
  samp->SetLineColor(4);
  samp->SetLineWidth(-2002);
  samp->SetFillStyle(3004);
  samp->SetFillColor(4);



  gr->Draw("AP");
  samp->Draw("same");
  //ppm1->Draw("same");

  gr->Fit("pol1");


}



