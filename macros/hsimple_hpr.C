{
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*
//*-*  This program creates :
//*-*    - a one dimensional histogram
//*-*    - a two dimensional histogram
//*-*    - a profile histogram
//*-*    - a memory-resident ntuple
//*-*
//*-*  These objects are filled with some random numbers and saved on a file.
//*-*
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

  gROOT->Reset();

// Create a new canvas.
  c1 = new TCanvas("c1","Dynamic Filling Example",250,10,700,500);
  c1->SetFillColor(42);
  c1->GetFrame()->SetFillColor(21);
  c1->GetFrame()->SetBorderSize(6);
  c1->GetFrame()->SetBorderMode(-1);

// Create a new ROOT binary machine independent file.
// Note that this file may contain any kind of ROOT objects, histograms,
// pictures, graphics objects, detector geometries, tracks, events, etc..
// This file is now becoming the current directory.

  TFile *hfile = (TFile*)gROOT->FindObject("hsimple_hpr.root"); if (hfile) hfile->Close();
  hfile = new TFile("hsimple_hpr.root","RECREATE","Demo ROOT file with histograms");

// Create some histograms, a profile histogram and an ntuple
  hpx    = new TH1F("hpx","This is the px distribution",100,-4,4);
  hpy2   = new TH1F("hpy2","This is the py distribution * 2",100,-4,4);
  hpxpy  = new TH2F("hpxpy","py vs px",100,-4,4,100,-4,4);
  hpxpy2  = new TH2F("hpxpy2","py vs px",100,-4,4,100,-4,4);
  hprof  = new TProfile("hprof","Profile of pz versus px",100,-4,4,0,20);
  ntuple = new TNtuple("ntuple","Demo ntuple","xvalue:yvalue:zvalue:random:event_number");

//  Set canvas/frame attributes (save old attributes)
  hpx->SetFillColor(48);

  gBenchmark->Start("hsimple");

// Fill histograms randomly
  gRandom->SetSeed();
  Float_t px, py, pz;
  Float_t px1, py1;
  const Int_t kUPDATE = 1000;
  for (Int_t i = 0; i < 25000; i++) {
     gRandom->Rannor(px,py);
     gRandom->Rannor(px1,py1);
     pz = px*px + py*py;
     Float_t random = gRandom->Rndm(1);
     hpx->Fill(px);
     hpy2->Fill(2 * py);
     hpxpy->Fill(0.3 * px + 1, 0.7 * py+0.5);
     hpxpy->Fill(0.5 * px - 1, 0.5 * py-0.5);

     hpxpy2->Fill(px, px * 0.5 + 0.5 * py);
     hprof->Fill(px,pz);
     ntuple->Fill(px,py,pz,random,(Float_t)i);
     if (i && (i%kUPDATE) == 0) {
        if (i == kUPDATE) hpx->Draw();
        c1->Modified();
        c1->Update();
        if (gSystem->ProcessEvents())
           break;
     }
  }
  gBenchmark->Show("hsimple");

// Save all objects in this file
  hpx->SetFillColor(0);
  hfile->Write();
  hpx->SetFillColor(48);
  c1->Modified();
  gDirectory = gROOT;
  
// Note that the file is automatically close when application terminates
// or when the file destructor is called.
}