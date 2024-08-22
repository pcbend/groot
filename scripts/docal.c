
//180 - 56Co
//184 - 152Eu

//003 - 56Co
//002 - 152Eu

void docal() {

  gROOT->SetBatch(kTRUE);
  
  //TFile *eu152 = TFile::Open("hist0184.root");
  //TFile *co56  = TFile::Open("hist0180.root");

  TFile *eu152 = TFile::Open("hist0002.root");
  TFile *co56  = TFile::Open("hist0003.root");
  
  //eu152->cd("raw");

  std::map<int,TCalibrator> calMap;

  for(int x=0;x<200;x++) {
    //TCalibrator c; 
    GH1D* ehist = (GH1D*)eu152->Get(Form("raw/Energy%03i_3",x));
    GH1D* chist = (GH1D*)co56->Get(Form("raw/Energy%03i_3",x));
    if(ehist) cout << "----- " << x << " -------" << endl;    
    if(ehist) ehist->Print();
    if(chist) chist->Print();
    if(!ehist || !chist) continue;
    ehist->GetXaxis()->SetRangeUser(150,4000);
    calMap[x].AddData(ehist,"152eu",4,.05);
    chist->GetXaxis()->SetRangeUser(500,2750);
    calMap[x].AddData(chist,"56Co",4,.05);
    chist->GetXaxis()->SetRangeUser(2750,10000);
    calMap[x].AddData(chist,"56Co",4,.05);
    calMap[x].Fit(1,1);
    calMap[x].Print();
    
    int address = (x/4) << 8;
    address += (x%4) << 6;
    address += 9;
    address += (1<<24);
    TChannel *c = TChannel::Get(address);
    if(c) {
      c->SetEnergyCoeff(calMap[x].GetParameters());
      c->Print();
      TChannel::AddChannel(c,"overwrite");
    } else {
      cout << x << "NO CHANNEL FOUND" << endl;
    }
    //if(x>50) break;

    if(ehist) cout << endl;
  } 

  std::map<int,TCalibrator>::iterator it;
  for(it=calMap.begin();it!=calMap.end();it++) {
    it->second.Print();
  }

  //TChannel::WriteCalFile("test.cal");
 
  return; 
}
