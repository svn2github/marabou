#ifndef __CINT__
#include "TList.h"
#include "TF1.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TString.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TKey.h"
#include "TFile.h"
#include "TRegexp.h"
#include "TFrame.h"
#include "TPaveText.h"

#include "iostream.h"
#include "fstream.h"
#endif
#include "iomanip.h"


//
// This macro fits a gaussian + linear bg to histograms provided in file 
// input.list given as argument. Results are written to output.result. 
// Its name defaults to input.result. 
//
// The first entry in input.list is the name of the root file followed
// by the names of the histgrams which may contain wildcards (*).
//
// The program runs through the list of histograms, displays it and waits
// for 2 inputs by mouse Button 1, which are taken as lower and upper 
// limit for the fit range.
// The following action buttons are provided: 
// Repeat: Ignore fit for current histogram and expect again 2 mouse clicks
// Next:   Move on to next histogram
// Quit:   Skip the rest of list
//
// Author: Otto Schaile
//_________________________________________________________________________
//

class TH1;

//  globals

Int_t gWait;

Axis_t gLowX, gUpX;
Double_t gBgConstant, gBgSlope,
         gContent, gMean, gSigma, 
         gBinW, gChi2pNDF;
//____________________________________________________________________________________ 
  
Double_t gaus_lbg(Double_t *x, Double_t *par)
{
/*
  par[0]   background constant
  par[1]   background slope
  par[2]   gauss width
  par[3]   gauss0 constant
  par[4]   gauss0 mean
  par[5]   gauss1 constant
  par[6]   gauss1 mean
  par[7]   gauss2 constant
  par[8]   gauss2 mean
*/
   static Float_t sqrt2pi = TMath::Sqrt(2*TMath::Pi()), sqrt2 = TMath::Sqrt(2.);
   Double_t arg;
//    gBinW = 0.08;
   if (par[2] == 0)par[2]=1;                 //  force widths /= 0
   arg = (x[0] - par[4])/(sqrt2*par[2]);
   Double_t fitval = par[0] + x[0]*par[1] 
              + gBinW/(sqrt2pi*par[2]) * par[3] * exp(-arg*arg);
   return fitval;
}
//____________________________________________________________________________________ 

TH1 * GetHist(TVirtualPad * pad)
{
   TList * l = pad->GetListOfPrimitives();
   TIter next(l);
   TObject * o;
   while( (o = next()) ){
      if(o->InheritsFrom("TH1")){
         TH1* h = (TH1*)o;
         return h;
      }
   }
   return NULL;
}
//____________________________________________________________________________________ 

void handle_mouse()
{
   static Int_t npress = 0;
//   static Bool_t done = kFALSE;
   static Axis_t x[2];
   TF1 fitfunc("fitfunc", gaus_lbg, 0, 1, 5);
//   Double_t sq2pi = TMath::Sqrt(2 * TMath::Pi());

   Int_t event = gPad->GetEvent();
   if (event != kButton1Down) return;
   TVirtualPad * pad;
   TObject *select = gPad->GetSelected();
   if(!select) return;
   if(select->IsA() == TPaveText::Class()){
      if(!strncmp(select->GetName(), "Repeat", 6)) {
         npress = 0;
//         done = kFALSE;
      }
      if(!strncmp(select->GetName(), "Quit", 4)){
         gWait = -1;
      }
      if(!strncmp(select->GetName(), "Next", 4)){
         gWait = 0;
         npress = 0;
 //        done = kTRUE;
      }
      return;
   }

   if(npress >= 2)return;

   pad = gPad;
//  make sure we are inside the box to allow still zooming 
   if(select->IsA() == TFrame::Class() ||
      select->IsA()->InheritsFrom("TH1")){ 
//      if(done){
//         gWait  = 0; 
//         npress = 0;
//         done = kFALSE;
 //        return;
 //     }
      Int_t px = gPad->GetEventX();
      Float_t xx = gPad->AbsPixeltoX(px);
      x[npress] = gPad->PadtoX(xx);
      cout << "x[" << npress << "] " << x[npress] << endl;
      npress++;
      if(npress == 2){
         TH1 * hist = GetHist(pad);
         if(!hist){
            cout << "hist not found" << endl;
            npress = 0;
            return;
         }
         gLowX = x[0];
         gUpX  = x[1];
         if(gLowX == gUpX){
            cout << "LowX == UpX, please try again" << endl;
            npress = 0;
            return;
         }
         if(gLowX > gUpX){gLowX = x[1]; gUpX  = x[0];}
         gContent = hist->Integral(hist->FindBin(gLowX),hist->FindBin(gUpX)); 
         gMean    = 0.5 * ( gLowX + gUpX);  
         gSigma   = 0.3 * ( gUpX  - gLowX); 
         gBinW = hist->GetBinWidth(1);
//         gContent /= (gSigma * sq2pi);
         cout << "gContent " << gContent << " gMean " 
               << gMean<< " gSigma " << gSigma<< " gBinW  " << gBinW<< endl;
         fitfunc.SetParameters(0, 0, gSigma, gContent, gMean); 
         fitfunc.SetRange(gLowX, gUpX);

         fitfunc.SetParName(0,"BgConstant");
         fitfunc.SetParName(1,"BgSlope   ");
         fitfunc.SetParName(2,"Sigma     ");
         fitfunc.SetParName(3,"Content   ");
         fitfunc.SetParName(4,"Mean      ");

         hist->Fit("fitfunc", "R", "SAME");

         gBgConstant = fitfunc.GetParameter(0);
         gBgSlope    = fitfunc.GetParameter(1);
         gSigma      = fitfunc.GetParameter(2);
         gContent    = fitfunc.GetParameter(3);
         gMean       = fitfunc.GetParameter(4);
         gChi2pNDF   = fitfunc.GetChisquare() / fitfunc.GetNDF();

         pad->Modified(kTRUE);
         pad->Update();
//         done = kTRUE;
      }
   } else {
      return;
   }
};

void treathist(TH1 * hist, ofstream * outfile){
   TCanvas * c;
   TString cname;
   cname = "C_";
   cname += hist->GetName();
   c = new TCanvas(cname, cname, 750, 20, 800, 600);
   hist->Draw();

   TPaveText repeat(0, 0, 0.1, 0.04, "NDC");
   repeat.SetBorderSize(0);
   repeat.AddText("Repeat");
   repeat.SetName("Repeat");
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

   gWait = 1;
   c->AddExec("handle_mouse", "handle_mouse()");
//      char ccc[10];  
//      cin >> ccc;
   while(gWait > 0){gSystem->Sleep(10); gSystem->ProcessEvents();};
   if(gWait == -1)return;
/*
   outfile << setw(15) << hist->GetName()
           << setw(10) << gBgConstant
           << setw(10) << gBgSlope   
           << setw(10) << gSigma     
           << setw(15) << gContent   
           << setw(10) << gMean      
           << setw(10) << gChi2pNDF
           << setw(10) << endl; 
*/
    *outfile << setw(15) << hist->GetName()
           << "  "<< gBgConstant
           << "  "<< gBgSlope   
           << "  "<< gSigma     
           << "  "<< gContent   
           << "  "<< gMean      
           << "  "<< gChi2pNDF
           << "  "<< endl; 
    cout << setw(15) << hist->GetName()
           << "  "<< gBgConstant
           << "  "<< gBgSlope   
           << "  "<< gSigma     
           << "  "<< gContent   
           << "  "<< gMean      
           << "  "<< gChi2pNDF
           << "  "<< endl; 
   delete hist;
   delete c;
}
//_________________________________________________________________________

Int_t fit(const Char_t * InFileName = "hs.list", const Char_t * OutFileName = 0)
{

#ifdef __CINT__
   cout << " def __CINT__ " <<endl;
#endif

   gStyle->SetOptFit(1);
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
   outfile.open(outname.Data(), ios::out);
   if(!outfile.good()){
      cout << "cant open: " << outname << endl;
      return -1;
   }
   
   TString wline;
	wline.ReadLine(infile, kFALSE);
	if (infile.eof()) {
      cout << "empty file: " << InFileName << endl;
      return -2;
   }
   TFile * histfile = new TFile(wline);
   if(!histfile || !histfile->IsOpen()){
      cout << "cant open histogram file: " << wline << endl;
      return -3;
   }
   TH1 * hist;
   TString cname;
	for (;;) {
	   wline.ReadLine(infile, kFALSE);
	   if (infile.eof()) {
			infile.close();
			break;
		}
      if(wline.Contains("*")){
         TRegexp pat(wline.Data(), kTRUE);
         cout << wline << endl;
         gDirectory->ls();
         TIter iterator(gDirectory->GetListOfKeys());
         TH1 *h;
         TKey * key; 
         const char * hname;
//         key = iterator();
//         cout << key  << endl;
//         if(key){h= (TH1*)key;  h->Print();}
         cout << wline << endl;
         while( (key = (TKey*)iterator()) ){
            if(!strncmp(key->GetClassName(),"TH1",3)){
               hname = key->GetName();
               h = (TH1*)histfile->Get(hname);
               if(!h){
                  cout << hname << " not found" << endl;
                  continue;
               }
               TString hn(h->GetName());
               cout << h->GetName() << endl;
               if(hn.Index(pat) >= 0) treathist(h, &outfile);
               if( gWait == -1 )return -1; 
            }
         }
      } else {  
      	hist = (TH1 *)histfile->Get(wline);
      	if(!hist){
         	cout << "Hist not found: " << wline <<endl;
         	continue;
      	}
      	treathist(hist, &outfile);
         if( gWait == -1 )return -1; 
   	}
   }
   return 1; 
}




