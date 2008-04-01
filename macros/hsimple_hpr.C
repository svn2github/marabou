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
  TCanvas *c1 = new TCanvas("c1","Dynamic Filling Example",250,10,700,500);
  c1->SetFillColor(42);
  c1->GetFrame()->SetFillColor(21);
  c1->GetFrame()->SetBorderSize(6);
  c1->GetFrame()->SetBorderMode(-1);

// Create a new ROOT binary machine independent file.
// Note that this file may contain any kind of ROOT objects, histograms,
// pictures, graphics objects, detector geometries, tracks, events, etc..
// This file is now becoming the current directory.

  TFile *hfile = (TFile*)gROOT->FindObject("hsimple_hpr.root"); if (hfile) hfile->Close();
  TFile *hfile = new TFile("hsimple_hpr.root","RECREATE","Demo ROOT file with histograms");

// Create some histograms, a profile histogram and an ntuple
  TH1F *hpx    = new TH1F("hpx","This is the px distribution",100,-4,4);
  TH1F *hpx2   = new TH1F("hpx2","This is the py distribution * 2",100,-4,4);
  TH1F *hpmany   = new TH1F("hpmany","Some gauss peaks",1000,0,4000);
  TH2F *hpxpy  = new TH2F("hpxpy","py vs px",100,-4,4,100,-4,4);
  TH2F *hpxpy2  = new TH2F("hpxpy2","py vs px",100,-4,4,100,-4,4);
  TH2F *hpxpz  = new TH2F("hpxpz","2dim, pz=px*px+py*py",100,-4,4,100,-4,4);
  TProfile *hprof  = new TProfile("hprof","Profile of pz (mean(pz)) versus px",100,-4,4,0,20);
  TNtuple *ntuple = new TNtuple("ntuple","Demo ntuple","xvalue:yvalue:zvalue:random:event_number");
  TNtuple *nt4 = new TNtuple("nt4","FourTuple","xv:yv:zv:uv");
  TH3C *hp31 = new TH3C("hp31", "3dim ", 100,-4,4, 100,-4,4, 100,-4,4);
  hp31->GetXaxis()->SetTitle("2 * px1");
  hp31->GetYaxis()->SetTitle("px + py1");
  hp31->GetZaxis()->SetTitle("px + pz1");
  hp31->SetMarkerColor(2);
  hp31->SetMarkerSize(0.05);


  TH3C *hp32 = new TH3C("hp32", "3dim ", 100,-4,4, 100,-4,4, 100,-4,4);
  hp32->GetXaxis()->SetTitle("2 * px1");
  hp32->GetYaxis()->SetTitle("-px + py1");
  hp32->GetZaxis()->SetTitle("px + pz1");
  hp32->SetMarkerColor(3);
  hp32->SetMarkerSize(0.05);

  TH3C *hp33 = new TH3C("hp33", "3dim ", 100,-4,4, 100,-4,4, 100,-4,4);
  hp33->GetXaxis()->SetTitle("2 * px1");
  hp33->GetYaxis()->SetTitle("0.5 * px + py1");
  hp33->GetZaxis()->SetTitle("-px + pz1");
  hp33->SetMarkerColor(4);
  hp33->SetMarkerSize(0.05);

//  Set canvas/frame attributes (save old attributes)
  hpx->SetFillColor(48);

  gBenchmark->Start("hsimple");

// Fill histograms randomly
  gRandom->SetSeed();
  Float_t px, py, pz, pu;
  Float_t px1, py1, pz1;
  const Int_t kUPDATE = 1000;
  for (Int_t i = 0; i < 25000; i++) {
     gRandom->Rannor(px,py);
     gRandom->Rannor(px1,py1);
     pz1 = px;
     pz = px*px + py*py;
     Float_t random = gRandom->Rndm(1);
     hpx->Fill(px);
     hpx2->Fill(2 * py);
     hpxpy->Fill(0.3 * px + 1, 0.7 * py+0.5);
     hpxpy->Fill(0.5 * px - 1, 0.5 * py-0.5);

     hpxpy2->Fill(px, px * 0.5 + 0.5 * py);
     hpxpz->Fill(px,pz);
     hprof->Fill(px,pz);
     ntuple->Fill(px,py,pz,random,(Float_t)i);
//
     px = 2 * px1;
     py = px + py1;
     pz = px + pz1;
     hp31->Fill(px, py,pz);

     py = - px + py1;
     pz = px + pz1;
     hp32->Fill(px, py,pz);

     py = 0.5 * px + py1;
     pz = - px + pz1;
     hp33->Fill(px, py,pz);

     if (i && (i%kUPDATE) == 0) {
        if (i == kUPDATE) hp33->Draw();
        c1->Modified();
        c1->Update();
        if (gSystem->ProcessEvents())
           break;
     }
  }
  for (Int_t i = 0; i < 10000; i++) {
     gRandom->Rannor(px,py);
     gRandom->Rannor(pz,pu);
     px += 2;
     py += 4;
     pz += 6;
     pu += 10;
     nt4->Fill(px,py,pz,pu);
  }
  for (Int_t i = 0; i < 10000; i++) {
     gRandom->Rannor(px,py);
     gRandom->Rannor(pz,pu);
     px -= 2;
     py -= 4;
     pz -= 6;
     pu += 20;
     nt4->Fill(px,py,pz,pu);
  }
  for (Int_t i = 0; i < 10000; i++) {
     gRandom->Rannor(px,py);
     gRandom->Rannor(pz,pu);
     px += 2;
     py -= 4;
     pz -= 6;
     pu += 30;
     nt4->Fill(px,py,pz,pu);
  }
  const Int_t npeaks = 5;
  Int_t mult = 10000;
  Int_t content[5]  = { 3,      2,    5,    4,    3};
  Float_t mean[5]   = { 800, 1200, 2000, 3000, 3200};
  Float_t sigma[5]  = { 120,  200,   40,   60,  100};
  for (Int_t i = 0; i < npeaks; i++) {
     for (Int_t k = 0; k < mult * content[i]; k++) {
        gRandom->Rannor(px,py);
        px1 = px * sigma[i] + mean[i];
        hpmany->Fill(px1);
     }
  }
// add some linear background
  TF1 * lbg = new TF1("lbg", "pol1",0, 4000);
  lbg->SetParameters(400, - 300. / 4000);
  hpmany->FillRandom("lbg", 100000);
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
