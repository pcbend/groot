int load_groot(const char* build_dir) {
  TString libdir = TString::Format("%s/lib", build_dir);
  gSystem->AddDynamicPath(libdir);

  if(gSystem->Load("libGTOOLS") < 0) return 1;
  if(gSystem->Load("libGROOTLIB") < 0) return 2;
  if(gSystem->Load("libGCORE") < 0) return 3;

  auto* h = reinterpret_cast<TH1*>(
    gROOT->ProcessLine("new GH1D(\"load_test\", \"load_test\", 16, 0.0, 16.0);")
  );
  if(!h) return 4;
  h->Fill(4.0);
  auto* c = new TCanvas("native_canvas", "native_canvas");
  h->Draw();
  c->AddExec("groot_interact", "GRootInteract()");
  c->Modified();
  c->Update();
  delete c;
  return 0;
}
