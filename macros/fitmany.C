//////////////////////////////////////////////////////////////////////////////////
//
// This macro fits a gaussian in a given range to a histogram read from
// serveral files. 
// The name of the histogram is hardcoded in TString hname("hE4");
//
// The file name must contain a run number RUN e.g. re1-033.root
// It is the concatination of: fname_prefixRUNfname_suffix
//
// the run nmubers and fit ranges are read from an ASCII file 
// InFileName  = "hs.list"
// formatted as: run lower_limit upper_limit
//
// results (run, meanvalue) are written to:
// OutFileName = "hs.result"
//
// After each fit the histogram + fitcurves are drawn.
// Press button Next or Quit to continue or finish.
// Pressing Loop  treats the rest of the Input file without further
// interaction
// as default fit results are printed on the terminal,
// Using option = "Q" suppresses this output.
//
// Author: Otto Schaile
//____________________________________________________________________________________ 

//  globals

Int_t gWait;
enum actions { kQuit, kWaitForMouse, kNext, kLoop }; 
//____________________________________________________________________________________ 

void handle_mouse()
{
   Int_t event = gPad->GetEvent();
   if (event != kButton1Down) return;
   TVirtualPad * pad;
   TObject *select = gPad->GetSelected();
   if(!select) return;
   if(select->IsA() == TPaveText::Class()){
      if(!strncmp(select->GetName(), "Loop", 6)) {
         gWait = kLoop;
      }
      if(!strncmp(select->GetName(), "Quit", 4)){
         gWait = kQuit;
      }
      if(!strncmp(select->GetName(), "Next", 4)){
         gWait = kNext;
 //        done = kTRUE;
      }
      return;
   }
}
//____________________________________________________________________________________ 

Int_t fitmany(const Char_t * InFileName  = "hs.list", 
              const Char_t * OutFileName = 0 )
{
   TString fname_prefix("re1-");
   TString fname_suffix(".root");
   TString fname;
   TString hname("hE4");
   TString option("");
//   TString option("Q");

   TFile * histfile;
   TH1 * hist;

   gStyle->SetOptFit(1);     // display fit results

   ifstream infile;
   infile.open(InFileName, ios::in);
   if(!infile.good()){
      cout << "cant open: " << InFileName << endl;
      return -1;
   }
   TString outname;
   if(OutFileName){
      outname = OutFileName;
   } else {
      outname = InFileName;
      Int_t dotpos = outname.Last('.');
      if(dotpos > 0)outname.Remove(dotpos);
      outname += ".result";
   }
   ofstream outfile;
   outfile.open(outname, ios::out);
   if(!outfile.good()){
      cout << "cant open: " << outname << endl;
      return -1;
   }

   Int_t run;
   Double_t xmin, xmax;
   TF1 * fitf = new TF1("fitf", "gaus");
   TCanvas * c1 = new TCanvas("c1", "c1", 10, 10, 700, 500);
   c1->AddExec("handle_mouse", "handle_mouse()");

   gWait = kNext;
	for (;;) {
	   infile >> run >> xmin >> xmax;
	   if (infile.eof()) {
			infile.close();
			break;
		}
//     generate file name
      fname = Form("%03d",run);
      fname.Prepend(fname_prefix);
      fname.Append(fname_suffix);
      cout << fname << " " << xmin << " " << xmax << endl;
      histfile = new TFile(fname);
      if (!histfile->IsOpen()) {
         cout << "Cant open " << fname << endl;
         delete histfile;
         continue;
      }
      hist = (TH1*)histfile->Get(hname);
      if (!hist) {      
         cout << "Cant find " << hname << endl;
         continue;
      }
      hist->Draw();
      fitf->SetLineColor(2);
      fitf->SetLineWidth(6);

      hist->Fit(fitf, option.Data(), "same", xmin, xmax);

   	TPaveText repeat(0, 0, 0.1, 0.04, "NDC");
   	repeat.SetBorderSize(0);
   	repeat.AddText("Loop");
   	repeat.SetName("Loop");
   	repeat.Draw();

   	TPaveText next(0.11, 0, 0.21, 0.04, "NDC");
   	next.SetBorderSize(0);
   	next.AddText("Next");
   	next.SetName("Next");
   	next.Draw();

   	TPaveText quit(0.22, 0, 0.32, 0.04, "NDC");
   	quit.SetBorderSize(0);
   	quit.AddText("Quit");
   	quit.SetName("Quit");
   	quit.Draw();
      c1->Update();

      if(gWait == kNext)
      {
         gWait = kWaitForMouse;
         while(gWait == kWaitForMouse)
         {
            gSystem->Sleep(10); 
            gSystem->ProcessEvents();
         }
         if(gWait == kQuit)return;
      }
      outfile << setw(5) << run << setw(15) << fitf->GetParameter(1) << endl;
      histfile->Close();
   }
}
