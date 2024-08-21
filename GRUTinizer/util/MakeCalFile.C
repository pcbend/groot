

{
  //int holes[14] = {12,14,17,19,21,22,23,24,25,26,27,28,29,30};
  int holes[12] = {12,14,17,19,21,22,24,25,26,27,28,29};
  
  for(int x=0;x<12;x++) {
    int hole = holes[x];
    for(int y=0;y<4;y++) {
      int xtal = hole*4+y;
      int board_id = hole << 8;   
      board_id += y << 6;
      board_id += 9;
      unsigned int address = (1<<24) + board_id;

      cout << hex << address << endl;
      TChannel *c = new TChannel;
      TString name;    
      name.Form("xtal%03i",xtal);
      c->SetName(name);
      c->SetAddress(address);
      c->SetNumber(xtal);
      //c->SetSystem("gretina");
      c->Print();

      TChannel::AddChannel(c);


    }
  }

  TChannel::WriteCalFile("gretina.cal");

}


