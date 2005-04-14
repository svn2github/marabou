Int_t addup_all_hists(const char * filelist, const char * outfile){
////////////////////////////////////////////////////////////////////////////////////
// 
// loop on root files in filelist, add up all histograms  and write to outfile
//
////////////////////////////////////////////////////////////////////////////////////

   gROOT->Reset();
//  list with input file names
   if(gSystem->AccessPathName(filelist)){
      cout << filelist << " not found" << endl;
      return -1;
   }
   if(!gSystem->AccessPathName(outfile)){
      cout << outfile << " already existing" << endl;
      return -1;
   }
   Int_t nfiles = 0;
   ifstream wstream;
   wstream.open(filelist, ios::in);
   while(1){
      TString fname;
      fname.ReadLine(wstream, kFALSE);
      cout << fname  << endl;
	   if (wstream.eof()) {
			wstream.close();
			break;
		}
		fname.Strip();
		if (fname.Length() <= 0) continue;
		if (fname[0] == '#') continue;
      if(gSystem->AccessPathName(fname.Data())){
         cout << fname.Data() << " not found" << endl;
         continue;
      }
      cout << "file " << fname.Data() << endl;
      TFile* inhist = new TFile(fname.Data());
      TIter next(inhist->GetListOfKeys());
      TKey* key;
      const char * hname;
      while(key= (TKey*)next()){
         if(!strncmp(key->GetClassName(),"TH1",3) ||
            !strncmp(key->GetClassName(),"TH2",3)){
            hname = (TObject*)key->GetName(); 
            cout << "histogram " << hname << endl;
            hresult = (TH1*)gROOT->GetList()->FindObject(hname);
            if(hresult)hresult->Print();
            hist = (TH1*)inhist->Get(hname);            // get pointer to histogram
            hist->Print();
         } else continue;
         if(nfiles == 0) {
            gDirectory = gROOT;
            hresult = (TH1*)hist->Clone();   // first time copy it
//            hist->SetDirectory(gROOT);
            cout << "clone histogram " << hname << endl;
         } else {
//            hresult = (TH1*)gROOT->FindObject(hname);
            cout << "adding histogram " << hname << endl;
//            hresult->Print();
            if(hresult)hresult->Add(hist, 1.);          // then add it up
            else cout << "extra histogram "  << hname 
                      << " found on " << fname.Data() << endl;
            if(hresult)hresult->Print();
         }
      } 
      nfiles++;
      inhist->Close();
   } 
   if(nfiles > 0){
      cout << "================ result ====================" << endl;
      TFile* outhist = new TFile(outfile, "RECREATE");
      Int_t nhists = 0;
      TIter next(gROOT->GetList());
      TH1 *h;
      TObject* obj;
      while(obj= (TObject*)next()){
         if(obj->InheritsFrom(TH1::Class())){
            h = (TH1*)obj;
            h->Print();
            h->Write();
            nhists++;
         }
      }
      cout << nhists << " hists written to " <<  outfile << endl;
   }   
   return nfiles; 
}
