#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "TROOT.h"
#include "TEnv.h"
#include "TSystem.h"
#include "TDirectory.h"
#include "TFormula.h"
#include "TF1.h"
#include "TCutG.h"
#include "TFile.h"
#include "TLine.h"
#include "TH1.h"
#include "TH2.h"
#include "TObject.h"
#include "TGObject.h"
#include "TObjectTable.h"
#include "TProfile.h"
#include "TPad.h"
#include "TPaveText.h"
#include "TPolyLine.h"
#include "TPaveLabel.h"
#include "TVirtualPad.h"
#include "TApplication.h"
#include "Buttons.h"
#include "TButton.h"
#include "TDiamond.h"
#include "HTCanvas.h"
#include "TContextMenu.h"
#include "TArray.h"
//#include "TArrayF.h"
#include "TString.h"
#include "TRegexp.h"
#include "TObjString.h"
#include "TList.h"
#include "TRint.h"
#include "TList.h"
#include "TStyle.h"
#include "TFrame.h"
#include "TArrayC.h"
#include "TArrayI.h"

#include "TGMsgBox.h"
#include "TGWidget.h"
#include "TGaxis.h"
#include "FHCommands.h"

#include "FitHist.h"
#include "FitHist_Help.h"
//#include "HprFitMacros.cxx"
#include "CmdListEntry.h"
#include "HistPresent.h"
//#include "MH1F.h"
//#include "MH2S.h"

// #include "TPInputs.h"
#include "FhMarker.h"
#include "support.h"
#include "TGMrbTableFrame.h"
#include "TGMrbInputDialog.h"
#include "SetColor.h"
#include "TMrbWdw.h"
#include "TMrbVarWdwCommon.h"
//  #include "TMrbVarWdw.h"

extern HistPresent *hp;
extern TFile *fWorkfile;
extern const char *fWorkname;
extern Int_t nHists;
extern Int_t nPeaks;
extern Double_t gTailSide;      // in fit with tail determines side: 1 left(low), -1 high(right)
extern Float_t gBinW;

enum dowhat { expand, projectx, projecty, statonly, projectf,
       projectboth };

//____________________________________________________________________________________ 
/* *INDENT-OFF* */
//____________________________________________________________________________________ 
const char Gauss2d[]=
"Double_t gauss2d( Double_t *x, Double_t *par)\n\
{\n\
   Double_t sqrt2pi = sqrt(2*TMath::Pi()), sqrt2 = sqrt(2.);\n\
   Double_t cx = par[0];    // const x\n\
   Double_t mx = par[1];    // mean x\n\
   if (par[2] == 0) par[2] = 1;\n\
   Double_t sx = par[2];    // sigma x\n\
   Double_t cy = par[3];    // const y\n\
   Double_t my = par[4];    // mean y\n\
   if (par[5] == 0) par[5] = 1;\n\
   Double_t sy = par[5];    // sigma \n\
   Double_t xij = x[0] - mx;  \n\
   Double_t argx = xij/(sqrt2*sx);\n\
   Double_t xval = cx * exp(-argx * argx)/(sqrt2pi*sx);\n\
   Double_t yij = x[1] - my;  \n\
   Double_t argy = yij/(sqrt2*sy);\n\
   Double_t yval = cy * exp(-argy * argy)/(sqrt2pi*sy);\n\
   return xval * yval;\n\
};\n\
\n\
fit_user_function(const char *hname)\n\
// This is a template macro to fit a user defined 2d function\n\
// As an example a 2d gauss is provided\n\
{\n\
//   gROOT->Reset();\n\
   TH2* hist = (TH1*)gROOT->FindObject(hname);\n\
   if(!hist){\n\
     cout << \"histogram not found\" << endl;\n\
     return 0;\n\
   }\n\
//  try to use same canvas dimensions for fitted function as used for histogram \n\
   TCanvas * mc  = (TCanvas *) gPad;\n\
   Int_t wtopx = 100, wtopy = 100;\n\
   UInt_t ww = 750, wh = 750;\n\
   TString cname(\"canvas_2d\");\n\
   if (mc) {\n\
      mc->GetCanvasPar(wtopx, wtopy, ww, wh);\n\
      cname = mc->GetName();\n\
      cname += \"_2df\";\n\
      wtopx = TMath::Max(0,wtopx - 100);\n\
      wtopy = TMath::Max(wh, wtopy + 100);\n\
 //      cout << wtopx<< " " << wtopy << " " <<  ww<< " " <<  wh << endl;\n\
   }\n\
\n\
//  nb: fit range may be different from histo range\n\
   Float_t x_from = -2; \n\
   Float_t x_to   =  2; \n\
   Float_t y_from = -4; \n\
   Float_t y_to   =  4; \n\
   TF2 * f2 = new TF2(\"gauss2d\", gauss2d, x_from, x_to, y_from, y_to , 6);\n\
   f2->SetParName(0, \"const_x\");\n\
   f2->SetParName(1, \"mean_x\");\n\
   f2->SetParName(2, \"sigma_x\");\n\
   f2->SetParName(3, \"const_y\");\n\
   f2->SetParName(4, \"mean_y\");\n\
   f2->SetParName(5, \"sigma_y\");\n\
\n\
   f2->SetParameters(100, 0, 1, 100, 0, 1);\n\
 //  f2->Draw();\n\
   hist->Fit(\"gauss2d\",\"R0\",\"same\");  // dont in same picture \n\
   TCanvas *cc = new TCanvas(cname , cname, wtopx, wtopy, ww, wh);\n\
//  draw in same range as histogram with same drawing options\n\
   f2->SetRange(hist->GetXaxis()->GetXmin(), hist->GetYaxis()->GetXmin(),\n\
                hist->GetXaxis()->GetXmax(), hist->GetYaxis()->GetXmax()); \n\
   f2->Draw(hist->GetOption()); \n\
}\n\
";
//____________________________________________________________________________________

const char ExpGauss[]=
"/*Exponential + Gauss (formula)*/\n\
fit_user_function(const char *hname)\n\
//\n\
// example with fit function given as formula\n\
//\n\
// This is a template macro to fit a user defined function\n\
// As an example an exponential + a gauss function is provided\n\
// The function is defined  as a formula\n\
{\n\
//   gROOT->Reset();\n\
   TH1* hist = (TH1*)gROOT->FindObject(hname);\n\
   if(!hist){\n\
     cout << \"histogram not found\" << endl;\n\
     return 0;\n\
   }\n\
   Float_t from =  0; \n\
   Float_t to   = 10;\n\
   TF1* f = new TF1(\"exp_gaus\",\"[0] + [1]*exp([2]*x) + gaus(3)\",from,to);\n\
   f->SetParameters(50, 200, -1, 200, 4, 1);\n\
   f->SetParName(0,\"lin_const \");\n\
   f->SetParName(1,\"exp_const \");\n\
   f->SetParName(2,\"exp_slope \");\n\
   f->SetParName(3,\"gaus_const\");\n\
   f->SetParName(4,\"gaus_mean \");\n\
   f->SetParName(5,\"gaus_sigma\");\n\
   hist->Fit(\"exp_gaus\",\"R+\",\"same\");\n\
}\n\
";
//____________________________________________________________________________________

const char TwoGauss[]=
"/*Two Gaussians with common mean*/\n\
Double_t TwoGauss     ( Double_t *x, Double_t *par) \n\
{\n\
// two gaussians with common mean\n\
  Double_t sqrt2pi = sqrt(2*TMath::Pi()), sqrt2 = sqrt(2.);\n\
  Double_t BinW = 0.25;\n\
  Double_t mean = par[2];\n\
//  force widths /= 0\n\
   if (par[6] == 0)par[6]=1;\n\
   if (par[4] == 0)par[4]=1;\n\
   Double_t xij = x[0] - par[2];   \n\
   Double_t arg0 = xij/(sqrt2*par[4]);\n\
   Double_t arg1 = xij/(sqrt2*par[6]);\n\
   Double_t fitval = par[0] + x[0]*par[1]\n\
           + par[3] * exp(-arg0*arg0)/(sqrt2pi*par[4])\n\
           + par[5] * exp(-arg1*arg1)/(sqrt2pi*par[6]);\n\
  return BinW * fitval;\n\
}\n\
\n\
fit_user_function(const char *hname)\n\
// Fit 2 gaussians with common mean\n\
{\n\
//   gROOT->Reset();\n\
   TH1* hist = (TH1*)gROOT->FindObject(hname);\n\
   if(!hist){\n\
     cout << \"histogram not found\" << endl;\n\
     return 0;\n\
   }\n\
   Double_t par[7];\n\
   Double_t from =  0; \n\
   Double_t to   = 50;\n\
\n\
   Double_t lin_const  = 0;\n\
   Double_t lin_slope  = 0;\n\
   Double_t gaus_mean  = 20;\n\
   Double_t gaus0_const= 10000;\n\
   Double_t gaus0_sigma= 4;\n\
   Double_t gaus1_const= 10000;\n\
   Double_t gaus1_sigma= 12;\n\
   TF1* f = new TF1(\"two_gaus\",TwoGauss,from,to, 7);\n\
   f->SetParameters(lin_const  ,\n\
                    lin_slope  ,\n\
                    gaus_mean  ,\n\
                    gaus0_const,\n\
                    gaus0_sigma,\n\
                    gaus1_const,\n\
                    gaus1_sigma);\n\
   f->SetParName(0,\"lin_const  \");\n\
   f->SetParName(1,\"lin_slope  \");\n\
   f->SetParName(2,\"gaus_mean  \");\n\
   f->SetParName(3,\"gaus0_const\");\n\
   f->SetParName(4,\"gaus0_sigma\");\n\
   f->SetParName(5,\"gaus1_const\");\n\
   f->SetParName(6,\"gaus1_sigma\");\n\
   f->SetLineColor(4);\n\
   f->SetLineWidth(3);\n\
//  Fill a histogram randomly using function\n\
//   Int_t entries = gaus0_const  + gaus1_const ;\n\
//   TH1F * twogran = new TH1F(\"twogran\",\n\
//                               \"Two gaussians with common mean\",\n\
//                              400, 0, 100);\n\
//   twogran->FillRandom(\"two_gaus\", entries);\n\
//   twogran->Draw();\n\
//   f->Draw(\"same\");   // only draw function, dont do fit\n\
   hist->Fit(\"two_gaus\",\"R+\",\"same\");\n\
\n\
   f->GetParameters(&par[0]);\n\
//  draw first gauss\n\
   TF1* f1 = new TF1(\"gaus1\",TwoGauss,from,to, 7);\n\
   f1->SetParameters(0, 0, par[2], par[3], par[4], 0, par[6]);\n\
   f1->SetLineColor(2);\n\
   f1->SetLineWidth(2);\n\
   f1->Draw(\"same\");\n\
\n\
//  draw second gauss\n\
   TF1* f2 = new TF1(\"gaus2\",TwoGauss,from,to, 7);\n\
   f2->SetParameters(0, 0, par[2], 0, par[4], par[5], par[6]);\n\
   f2->SetLineColor(6);\n\
   f2->SetLineWidth(2);\n\
   f2->Draw(\"same\");\n\
}\n\
";
//____________________________________________________________________________________

const char BreitWigner[]=
"/*Breit Wigner*/\n\
Double_t BreitWigner  ( Double_t *x, Double_t *par) \n\
{\n\
//\n\
// example with fit function defined as function \n\
//  \n\
  // Breit- Wigner function\n\
  Double_t mw = par[0], gw = par[1], mw2, gw2, eb2;\n\
  mw2 = mw*mw;\n\
  gw2 = gw*gw;\n\
  eb2 = x[0]*x[0];\n\
  return( par[2]*gw2*mw2 / ( pow( eb2 - mw2, 2 ) + mw2 * gw2 ) );\n\
}\n\
 \n\
fit_user_function(const char *hname)\n\
// This is a template macro to fit a user defined function\n\
// As an example a Breit Wigner is provided\n\
// The function is defined  as a call\n\
{\n\
//   gROOT->Reset();\n\
//  ------- comment out this piece if only a sample histogram should be produced ---\n\
   TH1* hist = (TH1*)gROOT->FindObject(hname);\n\
   if(!hist){\n\
     cout << \"histogram not found\" << endl;\n\
     return 0;\n\
   }\n\
//  ----------------------------------------------------------------------------   \n\
   Float_t from = -4; \n\
   Float_t to   = 4;\n\
   TF1* f = new TF1(\"BreitWigner\",BreitWigner,from,to, 3);\n\
   f->SetParameters(0, 1, 1000);\n\
   f->SetParName(0,\"mean \");\n\
   f->SetParName(1,\"width \");\n\
   f->SetParName(2,\"cont \");\n\
//  ------- activate  this peace if only a sample histogram should be produced --- \n\
\n\
//  Fill a histogram randomly using function\n\
//   TH1F * breitw = new TH1F(\"breitw\", \"A BreitWigner\",\n\
//                              400, -10, 10);\n\
//   breitw->FillRandom(\"BreitWigner\", 10000);\n\
//  ----------------------------------------------------------------------------   \n\
//   f-Draw,\"same\");   // only draw function, dont do fit\n\
   hist->Fit(\"BreitWigner\",\"R+\",\"same\");\n\
}\n\
";
//____________________________________________________________________________________

const char Landau[]=
"/*Three landaus*/\n\
Double_t Landau_f ( Double_t *x, Double_t *par) \n\
{\n\
    Double_t BinW = 9;\n\
//   linear background + 3 landau functions\n\
//   tail may be on high or low energy side\n\
//\n\
   double x0 = 2 * par[3] - x[0];     // low energy tail\n\
   double x1 = 2 * par[6] - x[0];     // low energy tail\n\
   double x2 = 2 * par[9] - x[0];     // low energy tail\n\
//   double x0 = x[0];             // high  energy tail\n\
//   double x1 = x[0];             // high  energy tail\n\
//   double x2 = x[0];             // high  energy tail\n\
   if(par[4] == 0)  par[4] = 1;\n\
   if(par[7] == 0)  par[7] = 1;\n\
   if(par[10] == 0) par[10]= 1;\n\
   double val =  par[0] + x[0] * par[1]\n\
   + par[2] / par[4]  * TMath::Landau(x0, par[3], par[4])\n\
   + par[5] / par[7] * TMath::Landau(x1, par[6], par[7])\n\
   + par[8] / par[10]* TMath::Landau(x2, par[9], par[10]);\n\
   return BinW * val;\n\
}\n\
\n\
fit_user_function(const char *hname){\n\
\n\
//   gROOT->Reset();\n\
//  ------- comment out this piece if only a sample histogram should be produced ---\n\
   TH1* hist = (TH1*)gROOT->FindObject(hname);\n\
   if(!hist){\n\
     cout << \"histogram not found\" << endl;\n\
     return 0;\n\
   }\n\
//                                                                             ^   \n\
//  ----------------------------------------------------------------------------   \n\
   Double_t from = 1600; \n\
   Double_t to   = 2150;\n\
   Int_t npar    = 11;\n\
   TF1 * f    = new TF1(\"landf\",Landau_f, from, to, npar);\n\
\n\
   Double_t backconst = 120;\n\
   Double_t backslope = - 0.05;\n\
\n\
   Double_t cont0  = 2800;\n\
   Double_t mean0  = 1750;\n\
   Double_t width0 = 5;\n\
   Double_t cont1  = 650;\n\
   Double_t mean1  = 2010;\n\
   Double_t width1 = 5;\n\
   Double_t cont2  = 200;\n\
   Double_t mean2  = 2055;\n\
   Double_t width2 = 5;\n\
  \n\
   f->SetParameters(backconst, backslope,\n\
                    cont0, mean0, width0, \n\
                    cont1, mean1, width1); \n\
   f->SetParameter(8,  cont2);        // SetParameters allows only 9 arguments\n\
   f->SetParameter(9,  mean2);\n\
   f->SetParameter(10, width2);\n\
   f->Draw(\"same\");                        //  draw function with start parameters\n\
   f->SetLineColor(2);    \n\
   f->SetLineWidth(3);\n\
\n\
   f->SetParName(0,\"const\");\n\
   f->SetParName(1,\"slope\");\n\
   f->SetParName(2,\"cont_0 \");\n\
   f->SetParName(3,\"mean_0 \");\n\
   f->SetParName(4,\"width_0\");\n\
   f->SetParName(5,\"cont_1 \");\n\
   f->SetParName(6,\"mean_1 \");\n\
   f->SetParName(7,\"width_1\");\n\
   f->SetParName(8,\"cont_2 \");\n\
   f->SetParName(9,\"mean_2 \");\n\
   f->SetParName(10,\"width_2\");\n\
//   f-Draw,\"same\");   // only draw function, dont do fit\n\
\n\
   hist->Fit(\"landf\",\"R+\",\"same\");   // the real fit\n\
\n\
//  ------- activate  this peace if only a sample histogram should be produced --- \n\
\n\
//   TFile * fout = new TFile(\"h_land_6.root\", \"RECREATE\");\n\
//   TH1F * hland = new TH1F(\"hland\",\"Landau functions\", 100, 1600, 2200);\n\
//   hland->FillRandom(\"landf\", 100000);\n\
//   fout->Write(); fout->Close();  \n\
                                                                            \n\
//  -------------------------------------------------------------------------------   \n\
\n\
}\n\
";
//____________________________________________________________________________________

const char Pol2Sine[]=
"/*Pol2 + Sine, (formula)*/ \n\
fit_user_function(const char *hname) \n\
// \n\
// example with fit function given as formula \n\
// \n\
// This is a template macro to fit a user defined function \n\
// As an example a sine with an amplitude modulated by a polynomial \n\
// of degree 2 (parabola) is provided \n\
// The function is defined  as a formula \n\
{ \n\
   TH1* hist = (TH1*)gROOT->FindObject(hname); \n\
   if(!hist){ \n\
     cout << \"histogram not found\" << endl; \n\
     return 0; \n\
   } \n\
   Float_t from = 500; \n\
   Float_t to   = 3500;\n\
   TF1* f = new TF1(\"sinf\",\"pol2 * sin([3]*x+[4])\",from,to); \n\
   f->SetParameters(0.366, 0, 0, 0.087, -.7); \n\
      f->SetLineColor(4); \n\
   f->SetLineWidth(1); \n\
   f->SetNpx(500); \n\
//   f->Draw(\"same\");                // dont fit, draw only \n\
   hist->Fit(\"sinf\",\"R\",\"same\"); \n\
}\n\
";

//____________________________________________________________________________________
/* *INDENT-ON* */
const Int_t nFitTemplates = 5;
const char *FitMacroTemplates[nFitTemplates] = {
   &TwoGauss[0],
   &ExpGauss[0],
   &BreitWigner[0],
   &Landau[0],
   &Pol2Sine[0]
};
//____________________________________________________________________________________ 

Int_t GetGaussEstimate(TH1 * h1, Double_t bg, Int_t centbin, TArrayD & par)
{
   Double_t halfmax = 0.5 * (h1->GetBinContent(centbin) + bg);
   par[1] = h1->GetBinCenter(centbin);
   Int_t NbinX = h1->GetNbinsX();
   Double_t sigma = 5. * h1->GetBinWidth(centbin);
   if (halfmax <= 0.) {
      cout << "halfmax<0" << endl;
      par[0] = h1->GetBinContent(centbin);
      par[2] = sigma;
      return -1;
   };
   Double_t sum = 0., sumr = 0.;
   Int_t binl = centbin, dbinl = 0 , dbin = 0;
   while (binl > 0) {
      Double_t cont = (Double_t) h1->GetBinContent(binl);
      if (cont < halfmax && cont > bg) {
         dbinl = centbin - binl;
         break;
      }
      sum += (cont - bg);
      binl--;
   };
   Int_t binr = centbin, dbinr = 1;
   while (binr <= NbinX) {
      Double_t cont = (Double_t) h1->GetBinContent(binr);
      if (cont < halfmax && cont > bg) {
         dbinr = binr - centbin;
         break;
      }
      sumr += (cont - bg);
      binr++;
   };
   if (!dbinl && !dbinr)
      return -1;

   if (dbinr && dbinr) {
      if (dbinr > dbinl)
         dbin = dbinl;
      else {
         dbin = dbinr;
         sum = sumr;
      };
   } else {
      if (dbinr) {
         dbin = dbinr;
         sum = sumr;
      };
      if (dbinr) {
         dbin = dbinr;
         sum = sumr;
      };
   };
   sigma = dbin * h1->GetBinWidth(centbin) / 1.18;
//  par[0]=2.* 1.25 * sum*h1->GetBinWidth(centbin)/(sigma*sq2pi);
   par[0] = 2. * 1.25 * sum;
   par[2] = sigma;
   return 1;
//  cout << "centbin, dbin" << centbin << dbin << endl;      
};

//____________________________________________________________________________________ 

Int_t GetMaxBin(TH1 * h1, Int_t binl, Int_t binu)
{
   Double_t maxcont = 0.;
   Int_t bin = (binu - binl) / 2;
   if (bin <= 0) {
      bin = -bin;
      return bin;
   }
   for (Int_t i = binl; i <= binu; i++) {
      Double_t cont = (Double_t) h1->GetBinContent(i);
      if (cont > maxcont) {
         maxcont = cont;
         bin = i;
      }
   };
//  cout << "MaxBin " << bin  << endl;
   return bin;
};

//____________________________________________________________________________________ 

Double_t gaus_cbg(Double_t * x, Double_t * par)
{
/*
  par[0]   background constant
//  par[1]   background slope
  par[1]   gauss width
  par[3]   gauss0 constant
  par[4]   gauss0 mean
  par[5]   gauss1 constant
  par[6]   gauss1 mean
  par[7]   gauss2 constant
  par[8]   gauss2 mean
*/
   static Float_t sqrt2pi = sqrt(2 * M_PI), sqrt2 = sqrt(2.);
   Double_t arg;

   if (par[2] == 0)
      par[2] = 1;               //  force widths /= 0
   arg = (x[0] - par[4]) / (sqrt2 * par[2]);
   Double_t fitval = par[0] + x[0] * par[1]
       + gBinW / (sqrt2pi * par[2]) * par[3] * exp(-arg * arg);
   if (nPeaks > 1) {
      arg = (x[0] - par[6]) / (sqrt2 * par[2]);
      fitval += gBinW / (sqrt2pi * par[2]) * par[5] * exp(-arg * arg);
   }
   if (nPeaks > 2) {
      arg = (x[0] - par[8]) / (sqrt2 * par[2]);
      fitval += gBinW / (sqrt2pi * par[2]) * par[7] * exp(-arg * arg);
   }
   return fitval;
}

//____________________________________________________________________________________ 

Double_t gaus_lbg(Double_t * x, Double_t * par)
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
   static Float_t sqrt2pi = sqrt(2 * M_PI), sqrt2 = sqrt(2.);
   Double_t arg;

   if (par[2] == 0)
      par[2] = 1;               //  force widths /= 0
   arg = (x[0] - par[4]) / (sqrt2 * par[2]);
   Double_t fitval = par[0] + x[0] * par[1]
       + gBinW / (sqrt2pi * par[2]) * par[3] * exp(-arg * arg);
   if (nPeaks > 1) {
      arg = (x[0] - par[6]) / (sqrt2 * par[2]);
      fitval += gBinW / (sqrt2pi * par[2]) * par[5] * exp(-arg * arg);
   }
   if (nPeaks > 2) {
      arg = (x[0] - par[8]) / (sqrt2 * par[2]);
      fitval += gBinW / (sqrt2pi * par[2]) * par[7] * exp(-arg * arg);
   }
   return fitval;
}

//____________________________________________________________________________________ 

Double_t gaus_tail(Double_t * x, Double_t * par)
{
/*
  par[0]   tail fraction
  par[1]   tail width
  par[2]   background constant
  par[3]   background slope
  par[4]   gauss width
  par[5]   gauss0 constant
  par[6]   gauss0 mean
  par[7]   gauss1 constant
  par[8]   gauss1 mean
  par[9]   gauss2 constant
  par[10]  gauss2 mean
*/
   static Float_t sqrt2pi = sqrt(2 * M_PI), sqrt2 = sqrt(2.);
//  force widths /= 0
   if (par[1] == 0)
      par[1] = 1;
   if (par[4] == 0)
      par[4] = 1;

// xij = (x - xpeak)
   Double_t xij = x[0] - par[6];
   Double_t arg = xij / (sqrt2 * par[4]);
   xij *= gTailSide;
   Double_t tail = exp(xij / par[1]) / par[1];
   Double_t g2b = 0.5 * par[4] / par[1];
   Double_t err = 0.5 * par[0] * par[5] * TMath::Erfc(xij / par[4] + g2b);
   Double_t norm = exp(0.25 * pow(par[4], 2) / pow(par[1], 2));
   Double_t fitval = par[2] + x[0] * par[3]
       + norm * err * tail + par[5] * exp(-arg * arg) / (sqrt2pi * par[4]);
   if (nPeaks > 1) {
      xij = x[0] - par[8];
      arg = xij / (sqrt2 * par[4]);
      xij *= gTailSide;
      tail = exp(xij / par[1]) / par[1];
      err = 0.5 * par[0] * par[7] * TMath::Erfc(xij / par[4] + g2b);
      fitval = fitval + norm * err * tail
          + par[7] * exp(-arg * arg) / (sqrt2pi * par[4]);
   }
   if (nPeaks > 2) {
      xij = x[0] - par[10];
      arg = xij / (sqrt2 * par[4]);
      xij *= gTailSide;
      tail = exp(xij / par[1]) / par[1];
      err = 0.5 * par[0] * par[9] * TMath::Erfc(xij / par[4] + g2b);
      fitval = fitval + norm * err * tail
          + par[9] * exp(-arg * arg) / (sqrt2pi * par[4]);
   }
   return gBinW * fitval;
}

//____________________________________________________________________________________ 

Double_t tailf(Double_t * x, Double_t * par)
{
//   extern Double_t erfc(Double_t);
//  force widths /= 0
   if (par[2] == 0)
      par[2] = 1;
   if (par[3] == 0)
      par[3] = 1;

// xij = (x - xpeak)
   Double_t xij = x[0] - par[1];
   xij *= gTailSide;

//   Double_t arggaus = xij/par[6];

   Double_t tail = exp(xij / par[3]) / par[3];
   Double_t g2b = 0.5 * par[2] / par[3];
   Double_t err = 0.5 * par[0] * TMath::Erfc(xij / par[2] + g2b);
   Double_t norm = exp(0.25 * pow(par[2], 2) / pow(par[3], 2));
   Double_t fitval = norm * err * tail;
//                     par[0] + x[0]*par[1] 
//                   + er * tail
//                   + par[5] * exp(-arggaus*arggaus);
   return gBinW * fitval;
}

//____________________________________________________________________________________ 

Double_t backf(Double_t * x, Double_t * par)
{
   Double_t fitval = par[0] + x[0] * par[1];
   return fitval;
//   return gBinW*fitval;
}

//____________________________________________________________________________________ 

Double_t gausf(Double_t * x, Double_t * par)
{
   static Float_t sqrt2pi = sqrt(2 * M_PI), sqrt2 = sqrt(2.);
   if (par[2] == 0)
      par[2] = 1;

//   xij = (x - xpeak)
   Double_t xij = x[0] - par[1];

   Double_t arggaus = xij / (sqrt2 * par[2]);
   Double_t fitval = par[0] * exp(-arggaus * arggaus) / (sqrt2pi * par[2]);
   return gBinW * fitval;
}

// Fit a pol1 + 1 or 2 or 3 gaus + optionally tails
//____________________________________________________________________________________ 

void FitHist::FitGBg(Int_t with_tail, Int_t force_zero_bg)
{

//   Int_t GetGaussEstimate(TH1*, Double_t, Int_t, Double_t *);
   if (!fSelHist) {
      cout << "No hist selected" << endl;
      return;
   }

   if (is2dim(fSelHist)) {
      cout << "Cant fit 2-dim hist (yet)" << endl;
      return;
   }
//   UInt_t xp, yp;
//   Int_t ret=GetPosition(gPad,&xp,&yp);
//   SetMark(kFALSE);

   const char *funcname;
   TString sfunc;
   if (with_tail > 0)
      sfunc = "Bg+Gauss_Tail_";
   else
      sfunc = "Bg+Gauss_";
   if (func_numb < 10) sfunc += "0";
   sfunc += func_numb;
   Bool_t ok;
   sfunc = GetString("Function name", (const char *) sfunc, &ok, mycanvas);
   if (!ok)
      return;
   funcname = (const char *) sfunc;
   func_numb++;

   TOrdCollection *row_lab = new TOrdCollection();
   if (with_tail > 0) {
      row_lab->Add(new TObjString("Ta_Fract"));
      row_lab->Add(new TObjString("Ta_Width"));
   }
   row_lab->Add(new TObjString("Bg_Const"));
   row_lab->Add(new TObjString("Bg_Slope"));
   row_lab->Add(new TObjString("Ga_Width"));
   row_lab->Add(new TObjString("Ga_Cont"));
   row_lab->Add(new TObjString("Ga_Mean "));
   row_lab->Add(new TObjString("Ga_Cont"));
   row_lab->Add(new TObjString("Ga_Mean "));
   row_lab->Add(new TObjString("Ga_Cont"));
   row_lab->Add(new TObjString("Ga_Mean "));

   int ncols = 3;
   TOrdCollection *col_lab = new TOrdCollection();
   col_lab->Add(new TObjString("StartVal"));
   col_lab->Add(new TObjString("Low Lim "));
   col_lab->Add(new TObjString("Up Lim  "));

//
   int *inp;
   TF1 *func;
   static Int_t npar;
   static Int_t nval;
   static const Int_t len_upar = 33;
   static Double_t upar[len_upar];
//   static Double_t fpar[11];
//   static Double_t gpar[3];
   TArrayD gpar(3);
   static Axis_t edgelx;
   static Axis_t edgeux;
   //
   if (hp->fFitOptKeepParameters && fOldMode != -1) {
      if (fOldMode == 0) {      //  old mode no tail
         if (with_tail > 0) {
            npar += 2;
            for (Int_t i = npar - 1; i > 1; i--)
               upar[i] = upar[i - 2];
            upar[0] = upar[1] = 0;
         } else {
            npar -= 2;
            for (int i = 0; i < npar; i++)
               upar[i] = upar[i + 2];
         }
      }
   } else {
      nval = GetMarks(fSelHist);
      if (nval < 2) {
         WarnBox("Need at least 2 selected points");
         return;
      } else {
         cout << "Fit Gaussian ----------------" << endl;
         markers->Print();
         FhMarker *p;
         TIter next(markers);
         inp = new int[nval];
         int i = 0;
         while ( (p = (FhMarker *) next()) ) {
            inp[i] = XBinNumber(fSelHist, p->GetX());
            if (inp[i] > 0)
               i++;
         }
      }
      edgelx = fSelHist->GetBinCenter(inp[0]);
      edgeux = fSelHist->GetBinCenter(inp[nval - 1]);
      Double_t lowval = fSelHist->GetBinContent(inp[0]);
      Double_t upval = fSelHist->GetBinContent(inp[nval - 1]);
//      Double_t bwidth = fSelHist->GetBinWidth(inp[0]);
      Int_t binh;
      if (nval <= 3) {
         npar = 7;
         nPeaks = 1;
         //      func = new TF1(funcname,"pol1(0)+gaus(2)",edgelx, edgeux);
//         upar[2] = 0.5 * (lowval + upval);
//         upar[3] = 0.;
//         if (nval == 3) {
//            centbin = inp[1];
//         } else {
 //           centbin = GetMaxBin(fSelHist, inp[0], inp[1]);
//         }
 //        binh = GetGaussEstimate(fSelHist, lowval, centbin, gpar);

//         upar[4] = gpar[2];
//         upar[5] = gpar[0];
 //        upar[6] = gpar[1];
         Double_t dx = edgeux - edgelx;
         Double_t dy = upval - lowval;
         if (force_zero_bg == 0) {
         	if (dx > 0) {
            	upar[3] = dy / dx;
            	upar[2] = upval - upar[3] * edgeux;
         	} else {            
            	upar[2] = 0.5 * (lowval + upval);
            	upar[3] = 0;
         	}
         } else {
            upar[2] = 0;
            upar[3] = 0;
         }
         upar[5] = 0;
         upar[4] = 0.25 * (edgeux - edgelx);
         for (Int_t i = inp[0]; i <= inp[nval - 1]; i++){
            upar[5] = upar[5] + fSelHist->GetBinContent(i) -
                      (upar[2] + upar[3] * fSelHist->GetBinCenter(i));
         }
//         upar[5] *= fSelHist->GetBinWidth(inp[0]);

         upar[6] = 0.5 * (edgeux + edgelx);

      } else {
         if (nval > 5) {
            WarnBox("Warning: Using not all inputs");
         }
         if (nval == 4) {
            npar = 9;
            nPeaks = 2;
            //         func = new TF1(funcname,"pol1(0)+gaus(2)+gaus(5)",edgelx, edgeux);
         } else {
            npar = 11;
            nPeaks = 3;
            //         func = new TF1(funcname,"pol1(0)+gaus(2)+gaus(5)+gaus(8)",edgelx, edgeux);
            binh = GetGaussEstimate(fSelHist, lowval, inp[3], gpar);
            upar[9] = gpar[0];
            upar[10] = gpar[1];
         }
         upar[2] = 0.5 * (lowval + upval);
         upar[3] = 0.;
         binh = GetGaussEstimate(fSelHist, lowval, inp[1], gpar);
         upar[4] = gpar[2];
         upar[5] = gpar[0];
         upar[6] = gpar[1];
         binh = GetGaussEstimate(fSelHist, lowval, inp[2], gpar);
         upar[7] = gpar[0];
         upar[8] = gpar[1];
      }
      if (with_tail > 0) {          // with tail
         upar[0] = 1.;
         upar[npar] = -.00001;
         upar[2 * npar] = 0.5;
         upar[1] = upar[4];
      } else {
         npar -= 2;
         for (int i = 0; i < npar; i++) {
            upar[i] = upar[i + 2];
         }
         upar[npar] = 0;
         upar[npar + 1] = 0;
      }
   }
   fOldMode = with_tail;
// ---

   func = (TF1 *) gROOT->FindObject(funcname);
   if (func)
      delete func;
   for (int i = 0; i < npar; i++) {
      upar[i + npar] = 0;
      upar[i + 2 * npar] = 0;
//      upar[i+npar]  = 0.1 * upar[i];
//      upar[i+2*npar]= 10. * upar[i];
   }
   if (with_tail > 0) {             // with tail
      func = new TF1(funcname, gaus_tail, edgelx, edgeux, npar);
      gTailSide = 1;
      if (with_tail == 2)
         gTailSide = -1;

//      upar[0] = 0.;
//      upar[npar]=-.00001;
//     upar[2*npar]=0.5;
//      func->SetParLimits(0,0.,2.);      
//      upar[1] = gpar[2];
      func->SetParName(0, "Ta_Fract");
      func->SetParName(1, "Ta_Width");
      func->SetParName(2, "Bg_Const");
      func->SetParName(3, "Bg_Slope");
      func->SetParName(4, "Ga_Width");

   } else {
//      npar -=2;
//      for (int i=0; i < npar; i++){
//         upar[i]=upar[i+2];
//      }
//      upar[npar]   = 0;
//      upar[npar+1] = 0;
      func = new TF1(funcname, gaus_lbg, edgelx, edgeux, npar);
      func->SetParName(0, "Bg_Const");
      func->SetParName(1, "Bg_Slope");
      func->SetParName(2, "Ga_Width");
   }

//  set errors + step size

   int nn = 0, istart = 3;
   if (with_tail > 0)
      istart = 5;
   for (int i = istart; i < npar; i += 2) {
      TString buf("Ga_Cont"); buf += nn;
      func->SetParName(i, buf.Data());
      buf = "Ga_Mean"; buf += nn ;
      func->SetParName(i + 1, buf.Data());
      nn++;
   }
   TArrayI fixflag(2 * npar);
   fixflag[0] = 0;
   fixflag[1] = 0;

   Bool_t bound = kFALSE;
   for (Int_t i = 2; i < npar; i++)
      fixflag[i] = 0;
   col_lab->Add(new TObjString("fix it"));
   col_lab->Add(new TObjString("bound"));
// show values to caller and let edit
   TString title("Start parameters");
//  if fixed linear background  requested fiz const + slope
   if (hp->fFitOptUseLinBg || force_zero_bg > 0) {
      cout << "Fixed linear background selected" << endl;
      if (with_tail > 0) {          // with tail
         upar[2] = fLinBgConst;
         upar[3] = fLinBgSlope;
         fixflag[2] = 1;
         fixflag[3] = 1;
      } else {
         if (with_tail == 0) {
            upar[0] = fLinBgConst;
            upar[1] = fLinBgSlope;
         } else {
            upar[0] = 0;
            upar[1] = 0;
         }
         fixflag[0] = 1;
         fixflag[1] = 1;
      }
   }

   TArrayD xyvals(len_upar);
   for (Int_t i = 0; i < len_upar; i++)
      xyvals[i] = upar[i];
   Int_t ret, itemwidth = 120, precission = 5;
   TGMrbTableOfDoubles(mycanvas, &ret, title.Data(), itemwidth,
                       ncols, npar, xyvals, precission,
                       col_lab, row_lab, &fixflag, 0,
                       Help_Fit_1dim_text, "Do fit", "Draw only");
   for (Int_t i = 0; i < len_upar; i++)
      upar[i] = xyvals[i];
   if (col_lab)
      col_lab->Delete();
   if (row_lab)
      row_lab->Delete();
   if (col_lab)
      delete col_lab;
   if (row_lab)
      delete row_lab;
   if (ret < 0)
      return;

   cout <<
       "-------------------------------------------------------------"
       << endl;
   cout << "Fitting Gauss + linear BG to Histogram:" << fSelHist->GetName()
       << " from " << edgelx << " to " << edgeux << endl;
   cout <<
       "-------------------------------------------------------------"
       << endl;
   cout << "Start parameters " << endl;

   for (Int_t i = 0; i < npar; i++) {
      bound = kFALSE;
      cout << "par[" << i << "] " << upar[i]
          << "\t lowlim " << upar[i + npar]
          << "\t uplim " << upar[i + 2 * npar];
      if (fixflag[i]) {
         cout << "\t fixed " << endl;
         upar[i + 2 * npar] = upar[i + npar] = 1;
         bound = kTRUE;
      } else {
//         if(upar[i+npar] != 0 || upar[i+2*npar] != 0 ) {
         if (fixflag[i + npar]) {
            cout << "\t bound" << endl;
            bound = kTRUE;
            if (upar[i + 2 * npar] == upar[i + npar]) {
               cout << "warning upper = lower bound" << endl;
            }
         } else
            cout << "\t variable " << endl;
      }
//      cout << endl;
      if (bound)
         func->SetParLimits(i, upar[i + npar], upar[i + 2 * npar]);
   }
   cout <<
       "-------------------------------------------------------------"
       << endl;
   cout <<
       "-------------------------------------------------------------"
       << endl;

//   func->SetLineWidth(4);
//   func->SetLineColor(4);
   func->SetParameters(upar);

//  now fit it 
   fSelPad->cd();
   if (ret == 0) {
//      gROOT->Reset();
      TString fitopt = "R";
      if (hp->fFitOptMinos)
         fitopt += "E";
      if (hp->fFitOptQuiet)
         fitopt += "Q";
      if (hp->fFitOptVerbose)
         fitopt += "V";
      if (hp->fFitOptErrors1)
         fitopt += "W";
      if (hp->fFitOptIntegral)
         fitopt += "I";
      if (hp->fFitOptAddAll)
         fitopt += "+";
      if (hp->fFitOptNoDraw)
         fitopt += "0";
      cout << "fitopt.Data() " << fitopt.Data() << endl;
      fSelHist->Fit(funcname, fitopt.Data());	//  here fitting is done
      if (hp->fFitOptAddAll) {
         TList *lof = fSelHist->GetListOfFunctions();
         if (lof->GetSize() > 1) {
            TObject *last = lof->Last();
            lof->Remove(last);
            lof->AddFirst(last);
         }
      }
      func->SetLineStyle(1);
      func->SetFillStyle(0);
      if (fOrigHist != fSelHist) {
         TF1 *f1 = new TF1();
         func->Copy(*f1);
         fOrigHist->GetListOfFunctions()->Add(f1);
         f1->SetParent(fOrigHist);
         f1->Save(edgelx, edgeux, 0, 0, 0, 0);
//         TCanvas * cc = new TCanvas("cc", "cvc");
         fOrigHist->GetXaxis()->SetRange(1, fSelHist->GetNbinsX());
//         fOrigHist->Draw();
      }

   } else {
      fSelPad->cd();
      func->SetLineColor(2);
      func->Draw("same");
      func->SetLineStyle(2);
      func->SetFillStyle(0);
      cout << setred << "No fit done, function drawn with start parameters"
          << setblack << endl;
   }
   func->GetParameters(upar);
   Double_t * errors = func->GetParErrors();
//   fSelHist->SetFillColor(hp->f1DimFillColor);
//   fSelPad->cd();
//   fSelPad->Modified(kTRUE);
//   fSelPad->Update();

//   Bool_t enable_calib = kFALSE;
//                                                                                                                                                                                                                                                             TEnv env(".rootrc");                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          // inspect ROOT's environment
//                                                                                                                                                                                                                                                             enable_calib = (Bool_t) env.GetValue("HistPresent.EnableCalibration", kFALSE);
   if (nPeaks == 1 && hp->GetEnableCalibration()) {
      TString question = "Add parameters to calibration list?";
      int buttons = kMBYes | kMBNo, retval = 0;
      EMsgBoxIcon icontype = kMBIconQuestion;
      new TGMsgBox(gClient->GetRoot(), mycanvas,
                   "Question", (const char *) question,
                   icontype, buttons, &retval);
      if (retval == kMBYes) {
         Int_t woff = 0;
         if (with_tail > 0)
            woff = 2;
         FhPeak *peak = new FhPeak(upar[4 + woff]);
//         peak->SetWidth(upar[2 + woff]);
         peak->SetWidth(errors[4 + woff]);
         peak->SetContent(upar[3 + woff]);
         fPeaks->Add(peak);
      }
   }
//  draw components of fit, skip simple gaus without bg
   if (hp->GetShowFittedCurves() && force_zero_bg == 0) {
      double fdpar[4];
      if (with_tail > 0) {
         fdpar[0] = upar[2];
         fdpar[1] = upar[3];
      } else {
         fdpar[0] = upar[0];
         fdpar[1] = upar[1];
      }
      if (force_zero_bg == 0) {
         TF1 *back = new TF1("backf", backf, edgelx, edgeux, 2);
         back->SetParameters(fdpar);
         back->Save(edgelx, edgeux, 0, 0, 0, 0);
         back->SetLineColor(2);
         back->SetLineStyle(3);
   //      back->Draw("same");
         fSelHist->GetListOfFunctions()->Add(back);
         back->SetParent(fSelHist);
      }
//      if(fOrigHist != fSelHist)fOrigHist->GetListOfFunctions()->Add(back);
//
      for (Int_t j = 0; j < nPeaks; j++) {
         if (with_tail > 0) {
            fdpar[0] = upar[5 + 2 * j];
            fdpar[1] = upar[6 + 2 * j];
            fdpar[2] = upar[4];
         } else {
            fdpar[0] = upar[3 + 2 * j];
            fdpar[1] = upar[4 + 2 * j];
            fdpar[2] = upar[2];
         }
         TF1 *gaus = new TF1("gausf", gausf, edgelx, edgeux, 3);
         gaus->SetParameters(fdpar);
         TF1 *g1 = new TF1();
         gaus->Copy(*g1);
         g1->SetLineColor(6);
         g1->SetLineStyle(4);
//         g1->Draw("same");
         fSelHist->GetListOfFunctions()->Add(g1);
         g1->SetParent(fSelHist);
         g1->Save(edgelx, edgeux, 0, 0, 0, 0);
/*
         TF1 *g2 = new TF1();
         gaus->Copy(*g2);
         g2->SetLineColor(6);
         g2->SetLineWidth(3);
         if(fOrigHist != fSelHist)fOrigHist->GetListOfFunctions()->Add(g2);
         g2->SetParent(fOrigHist);
         g2->Save(edgelx, edgeux);
*/
         if (with_tail > 0) {
            fdpar[0] = upar[0] * upar[5 + 2 * j];	// const
            fdpar[1] = upar[6 + 2 * j];	// position
            fdpar[2] = upar[4]; // gaus width
            fdpar[3] = upar[1]; // tail width
            TF1 *tail = new TF1("tailf", tailf, edgelx, edgeux, 4);
            tail->SetParameters(fdpar);
            tail->Save(edgelx, edgeux, 0, 0, 0, 0);
            tail->SetLineColor(7);
            tail->SetLineStyle(2);
//            tail->Draw("same");
            fSelHist->GetListOfFunctions()->Add(tail);
            tail->SetParent(fSelHist);
//            if(fOrigHist != fSelHist)fOrigHist->GetListOfFunctions()->Add(tail);
         }
      }
          
   }
   ClearMarks();
//   if (hp->fShowErrors) fSelHist->Draw("e1");
   UpdateDrawOptions();
/*
   TString drawopt;
   if (hp->fShowContour)
      drawopt = "";
   if (hp->fShowErrors)
      drawopt += "e1";
   if (hp->fFill1Dim) {
      fSelHist->SetFillStyle(1001);
      fSelHist->SetFillColor(hp->f1DimFillColor);
   } else
      fSelHist->SetFillStyle(0);
   fSelHist->Draw(drawopt.Data());

   fSelPad->cd();
*/
   fSelPad->Modified(kTRUE);
   fSelPad->Update();
   fSelPad->GetFrame()->SetBit(TBox::kCannotMove);
};

//____________________________________________________________________________

Int_t FitHist::FitPolyHist(Int_t degree)
{
   if (is2dim(fSelHist))
      return Fit2dim(0, degree);
   else
      return Fit1dim(0, degree);
}

//____________________________________________________________________________

Int_t FitHist::FitPolyMarks(Int_t degree)
{
   if (is2dim(fSelHist))
      return Fit2dim(1, degree);
   else
      return Fit1dim(1, degree);
}

//____________________________________________________________________________

Int_t FitHist::Fit1dim(Int_t what, Int_t ndim)
{

   if (what != 0) {
      WarnBox("Fit marks not (yet) possible for 1-dim");
      return -1;
   }
   if (!fSelPad || !fOrigHist) {
      cout << "Cant find pad or hist" << endl;
      return -1;
   }

   if (ndim < 0 || ndim > 8) {
      WarnBox("wrong dim, must be 0-8");
      return -1;
   }

   int nval = GetMarks(fSelHist);
   if (nval < 2) {
      WarnBox("Not enough marks");
      return -1;
   }
   cout << "Fit1dim ----------------" << endl;
   markers->Print();
//   double par[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
//   double fpar[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

//   par[0] = mean;
   const char *funcname;
   TString sfunc = fHname;
/*
   TString sfunc = fSelHist->GetName();
   Int_t us = sfunc.Index("E_");
   if(us >= 0)sfunc.Remove(0,2);
   us = sfunc.Index("_");
   if(us >= 0)sfunc.Remove(0,us+1);
*/
//   sfunc="poly_";
   func_numb++;
   sfunc.Prepend(Form("%d_", func_numb));
   sfunc.Prepend("f");
   Bool_t ok;
   sfunc = GetString("Function name", (const char *) sfunc, &ok, mycanvas);
   if (!ok)
      return 0;
   funcname = (const char *) sfunc;
   TFormula *func;
   switch (ndim) {
   case 0:
      func = new TFormula("func", "[0]");
      break;
   case 1:
      func = new TFormula("func", "[0]+[1]*x");
      break;
   case 2:
      func = new TFormula("func", "[0]+[1]*x+[2]*x^2");
      break;
   case 3:
      func = new TFormula("func", "[0]+[1]*x+[2]*x^2+[3]*x^3");
      break;
   case 4:
      func = new TFormula("func", "[0]+[1]*x+[2]*x^2+[3]*x^3+[4]*x^4");
      break;
   case 5:
      func = new TFormula("func",
                          "[0]+[1]*x+[2]*x^2+[3]*x^3+[4]*x^4+[5]*x^5");
      break;
   case 6:
      func = new TFormula("func",
                          "[0]+[1]*x+[2]*x^2+[3]*x^3+[4]*x^4+[5]*x^5+[6]*x^6");
      break;
   case 7:
      func = new TFormula("func",
                          "[0]+[1]*x+[2]*x^2+[3]*x^3+[4]*x^4+[5]*x^5+[6]*x^6+[7]*x^7");
      break;
   case 8:
      func = new TFormula("func",
                          "[0]+[1]*x+[2]*x^2+[3]*x^3+[4]*x^4+[5]*x^5+[6]*x^6+[7]*x^7+[8]*x^8");
      break;
   }
   FhMarker *pfirst = (FhMarker *) markers->First();
   FhMarker *plast = (FhMarker *) markers->Last();
   cout <<
       "-------------------------------------------------------------"
       << endl;

   cout << "Fitting polynomial of degree " << ndim << " to ";
   if (what == 0)
      cout << fSelHist->GetName();
   if (what == 1)
      cout << " user supplied marks";
   cout << " from " << pfirst->GetX() << " to " << plast->GetX() << endl;
   cout <<
       "-------------------------------------------------------------"
       << endl;
//   Int_t binlx = XBinNumber(fSelHist, pfirst->GetX());
//   Int_t binux = XBinNumber(fSelHist, plast->GetX());
   cHist->cd();
   TF1 *pol =
       new TF1(funcname, "func", (float) pfirst->GetX(),
               (float) plast->GetX());
   TString fitopt = "R";
   if (hp->fFitOptMinos)
      fitopt += "E";
   if (hp->fFitOptQuiet)
      fitopt += "Q";
   if (hp->fFitOptVerbose)
      fitopt += "V";
   if (hp->fFitOptErrors1)
      fitopt += "W";
   if (hp->fFitOptIntegral)
      fitopt += "I";
   if (hp->fFitOptAddAll)
      fitopt += "+";
   if (hp->fFitOptNoDraw)
      fitopt += "0";
   cout << "fitopt.Data() " << fitopt.Data() << endl;
   fSelHist->Fit(funcname, fitopt.Data());	//  here fitting is done
   if (hp->fFitOptAddAll) {
      TList *lof = fSelHist->GetListOfFunctions();
      if (lof->GetSize() > 1) {
         TObject *last = lof->Last();
         lof->Remove(last);
         lof->AddFirst(last);
      }
   }
   UpdateDrawOptions();
   fSelPad->Update();
   fSelPad->GetFrame()->SetBit(TBox::kCannotMove);

   TString question = "Write function to workfile?";
   int buttons = kMBYes | kMBNo, retval = 0;
   EMsgBoxIcon icontype = kMBIconQuestion;
   new TGMsgBox(gClient->GetRoot(), mycanvas,
                "Qustion", (const char *) question,
                icontype, buttons, &retval);
   if (retval == kMBYes) {
      if (OpenWorkFile()) {
         pol->Write();
         CloseWorkFile();
      } else
         func_numb--;
   }
   return 1;
}

//____________________________________________________________________________

Int_t FitHist::Fit2dim(Int_t what, Int_t ndim)
{

   if (!fSelPad || !fOrigHist) {
      cout << "Cant find pad or hist" << endl;
      return -1;
   }
//  now find limits

   if (!is2dim(fSelHist)) {
      WarnBox("Not 2 dim");
      return -1;
   }
   if (ndim < -1 || ndim > 8) {
      WarnBox("wrong dim, must be 0-8");
      return -1;
   }

   int nval = GetMarks(fSelHist);
   if (nval < 2) {
      WarnBox("Not enough marks");
      return -1;
   }
   cout << "Fit2dim ----------------" << endl;
   markers->Print();
//     get values in orig hist      
   TAxis *xaxis = fOrigHist->GetXaxis();
   Axis_t xmin = xaxis->GetXmin();
   Axis_t xmax = xaxis->GetXmax();
   Int_t nxb = fOrigHist->GetNbinsX();
   Axis_t xbinwidth = (xmax - xmin) / nxb;

   TAxis *yaxis = fOrigHist->GetYaxis();
   Axis_t ymin = yaxis->GetXmin();
   Axis_t ymax = yaxis->GetXmax();
   Int_t nyb = fOrigHist->GetNbinsY();
   Axis_t ybinwidth = (ymax - ymin) / nyb;

//     get values in expanded hist      

   FhMarker *pfirst = (FhMarker *) markers->First();
   FhMarker *plast = (FhMarker *) markers->Last();
   Float_t xlow = pfirst->GetX();
   Float_t xup = plast->GetX();

   UpdateCut();
//   CheckList(fActiveCuts);
//   if(Ncuts() > 0){
//
//   }  
   Int_t binlx = XBinNumber(fSelHist, xlow);
   Int_t binux = XBinNumber(fSelHist, xup);
   if (binlx > binux) {
      Int_t temp = binux;
      binux = binlx;
      binlx = temp;
   }
   Int_t binly = YBinNumber(fSelHist, pfirst->GetY());
   Int_t binuy = YBinNumber(fSelHist, plast->GetY());
   if (binly > binuy) {
      Int_t temp = binuy;
      binuy = binly;
      binly = temp;
   }

   TAxis *xaxis_new = fSelHist->GetXaxis();
   Axis_t edgelx = xaxis_new->GetBinLowEdge(binlx);
   Axis_t edgeux = xaxis_new->GetBinLowEdge(binux) +
       xaxis_new->GetBinWidth(binlx);
   TAxis *yaxis_new = fSelHist->GetYaxis();
   Axis_t edgely = yaxis_new->GetBinLowEdge(binly);
   Axis_t edgeuy = yaxis_new->GetBinLowEdge(binuy) +
       yaxis_new->GetBinWidth(binly);
// recalculate bin numbers

   binlx = (Int_t) ((edgelx - xmin) / xbinwidth + 1.5);
   binux = (Int_t) ((edgeux - xmin) / xbinwidth + 0.5);
   Int_t nxbins = binux - binlx + 1;

   binly = (Int_t) ((edgely - ymin) / ybinwidth + 1.5);
   binuy = (Int_t) ((edgeuy - ymin) / ybinwidth + 0.5);
   Int_t nybins = binuy - binly + 1;
   if (nxbins < 2 || nybins < 1) {
      WarnBox("nxbins < 2 || nybins < 1");
      return -1;
   }
//   TCanvas *ccc = new TCanvas("Fit2dimHist_temp","Fit2dimHist"
//                   ,10,500, 500,500);
//   hp->GetCanvasList()->Add(ccc);
   HTCanvas *ccc =
       new HTCanvas("Fit2dimHist_temp", "Fit2dimHist", 10, 500, 500, 500,
                    hp, 0);
   hp->GetCanvasList()->Add(ccc);
   ccc->cd();
   double par[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
   double fpar[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
   TH1D *fithist = new TH1D("fithist_temp", "fithist_temp",
                            nxbins, edgelx, edgeux);
// case fit to histogram

   if (what == 0) {
      UpdateCut();
//      Double_t cont = 0;
      double *sum = new double[nxbins];
      double *sumx = new double[nxbins];
      double *sumx2 = new double[nxbins];
      double *mean = new double[nxbins];
      double *sigma = new double[nxbins];
      //     is there a cut active?
      for (Int_t i = binlx; i <= binux; i++) {
         Int_t indx = i - binlx;
         sum[indx] = 0.;
         sumx[indx] = 0.;
         sumx2[indx] = 0.;
         Axis_t xcent = xaxis->GetBinCenter(i);
         for (Int_t j = binly; j <= binuy; j++) {
            Axis_t ycent = yaxis->GetBinCenter(j);
            if (Ncuts()) {
               if (!InsideCut((float) xcent, (float) ycent))
                  continue;
            }
            Stat_t cont = fOrigHist->GetCellContent(i, j);
            sum[indx] += cont;
            sumx[indx] += ycent * cont;
            sumx2[indx] += ycent * ycent * cont;
         }
      }
      int nonzero = 0, nx1 = 0, nx2 = 0;
      double mean1 = 0., mean2 = 0., sigma_max = 0.;
      int nxhalf = nxbins / 2;
      for (Int_t i = 0; i < nxbins; i++) {
         if (sum[i] > 0) {
            mean[i] = sumx[i] / sum[i];
            if (sum[i] > 1.1) {
               double s2 = sumx2[i] / sum[i] - mean[i] * mean[i];
               if (s2 > 0)
                  sigma[i] = sqrt(s2) / sqrt(sum[i]);
               else
                  sigma[i] = 1;
//               sigma[i]= sqrt(sumx2[i]/sum[i]-mean[i]*mean[i])/sqrt(sum[i]);
            } else
               sigma[i] = 1;
            if (sigma[i] > sigma_max)
               sigma_max = sigma[i];
            if (i < nxhalf) {
               mean1 += mean[i];
               nx1++;
            } else {
               mean2 += mean[i];
               nx2++;
            }
            nonzero++;
         } else {
            mean[i] = 0.;
            sigma[i] = 0.;
         }
//         cout << i << " " << sum[i]<< " "<< sumx[i] <<" " << sumx2[i] << " " << mean[i] << " " << sigma[i] << endl;
      }
//     if nentries =1 replace error by max 
      for (Int_t i = 0; i < nxbins; i++) {
         if (sum[i] > 0 && sum[i] < 1.1)
            sigma[i] = sigma_max;
      }

      fithist->SetContent((Stat_t *) mean);
      fithist->SetError((Stat_t *) sigma);

      if (nx1 > 0)
         mean1 /= nx1;
      if (nx2 > 0)
         mean2 /= nx2;
      int nx14 = binlx + nxhalf / 2;
      int nx34 = binlx + nxhalf + nxhalf / 2;
      if (nx34 <= nx14) {
         cout << "Fit2dim: wrong range, nx14, nx34"
             << nx14 << ", " << nx34 << endl;
         return -1;
      }
      Axis_t x14 = xaxis->GetBinCenter(nx14);
      Axis_t x34 = xaxis->GetBinCenter(nx34);
      if (ndim <= 0) {
         par[0] = 0.5 * (mean2 - mean1);
      } else {
         par[1] = (mean2 - mean1) / (x34 - x14);
         par[0] = mean1 - x14 * par[1];
      }
      delete sum;
      delete sumx;
      delete sumx2;
      delete mean;
      delete sigma;
   } else {
      FhMarker *p;
      TIter next(markers);
      double mean = 0., entries = 0.;
      TAxis *xaxis = fSelHist->GetXaxis();
      xmin = xaxis->GetXmin();
      xmax = xaxis->GetXmax();
      while ( (p = (FhMarker *) next()) ) {
         cout << p->GetX() << " " << p->GetY() << endl;
//         dont take default marks at corners     
         if (p->GetX() > xmin && p->GetX() < xmax) {
            fithist->Fill(p->GetX(), p->GetY());
            mean += p->GetY();
            entries++;
         }
         if (entries > 0)
            mean /= entries;
      }
      if (entries > 0) {
         if (ndim >= 0 && entries < ndim + 1)
            WarnBox("Too many fit parameters");
         mean /= entries;
      } else {
         WarnBox("No marks set");
         return -1;
      }
      par[0] = mean;
   }
   const char *funcname;
   TString sfunc = fHname;
   func_numb++;
   sfunc.Prepend(Form("%d_", func_numb));
   sfunc.Prepend("f");
   Bool_t ok;
   sfunc = GetString("Function name", (const char *) sfunc, &ok, mycanvas);
   if (!ok)
      return 0;
   funcname = (const char *) sfunc;
   TFormula *func;
   if (ndim < 0) {
      TString uformula("[0]+[1]*x+[2]*x^2");
      uformula =
          GetString("Formula", (const char *) uformula, &ok, mycanvas);
      if (!ok)
         return 0;
      func = new TFormula("func", uformula.Data());
      cout << "Fitting user formula to ";
   } else {
      cout << "Fitting polynomial of degree " << ndim << " to ";
      switch (ndim) {
      case 0:
         func = new TFormula("func", "[0]");
         break;
      case 1:
         func = new TFormula("func", "[0]+[1]*x");
         break;
      case 2:
         func = new TFormula("func", "[0]+[1]*x+[2]*x^2");
         break;
      case 3:
         func = new TFormula("func", "[0]+[1]*x+[2]*x^2+[3]*x^3");
         break;
      case 4:
         func = new TFormula("func", "[0]+[1]*x+[2]*x^2+[3]*x^3+[4]*x^4");
         break;
      case 5:
         func = new TFormula("func",
                             "[0]+[1]*x+[2]*x^2+[3]*x^3+[4]*x^4+[5]*x^5");
         break;
      case 6:
         func = new TFormula("func",
                             "[0]+[1]*x+[2]*x^2+[3]*x^3+[4]*x^4+[5]*x^5+[6]*x^6");
         break;
      case 7:
         func = new TFormula("func",
                             "[0]+[1]*x+[2]*x^2+[3]*x^3+[4]*x^4+[5]*x^5+[6]*x^6+[7]*x^7");
         break;
      case 8:
         func = new TFormula("func",
                             "[0]+[1]*x+[2]*x^2+[3]*x^3+[4]*x^4+[5]*x^5+[6]*x^6+[7]*x^7+[8]*x^8");
         break;
      }
   }
   cout <<
       "-------------------------------------------------------------"
       << endl;

//   cout << "Fitting: " << funcname << " (Pol " << ndim << ") to ";
   if (what == 0)
      cout << fSelHist->GetName();
   if (what == 1)
      cout << " user supplied marks";
   cout << " from " << edgelx << " to " << edgeux << endl;
   cout <<
       "-------------------------------------------------------------"
       << endl;

   TF1 *pol = new TF1(funcname, "func", (float) edgelx, (float) edgeux);
//   DisplayHist(fithist);
   pol->SetLineColor(ndim + 1);
   pol->SetLineWidth(2);

   fithist->Fit(funcname, "R");
   if (isnan(pol->GetChisquare()) != 0) {
      cout << "Something went wrong, FCN=Chisquare is infinite " << endl;
      return -1;
   }
   fithist->Draw("E");
   ccc->Update();
   if (fOrigHist != fSelHist)
      fOrigHist->GetListOfFunctions()->Add(pol);
   fSelHist->GetListOfFunctions()->Add(pol);
//   fithist->SetMinimum(edgely);
//--------------------------------------
   pol->GetParameters(fpar);

   TString scorr = "$1/(";
   for (int i = 0; i <= ndim; i++) {
      if (i > 0) {
         if (fpar[i] > 0) scorr += "+";
          scorr += Form("%f", fpar[i]);
          scorr += "*$2";
         if (i > 1) {
            scorr += "^"; 
            scorr += i;
         }
      } else {
         scorr += Form("%f", fpar[i]);
      }
   }
   scorr += "):$2";
   cout << scorr << endl;
   fSelPad->cd();
   pol->Draw("same");
   fSelPad->Update();
   TString question = "Write function to workfile?";
   int buttons = kMBYes | kMBNo, retval = 0;
   EMsgBoxIcon icontype = kMBIconQuestion;
   new TGMsgBox(gClient->GetRoot(), mycanvas,
                "Qustion", (const char *) question,
                icontype, buttons, &retval);
   if (retval == kMBYes) {
      if (OpenWorkFile()) {
         pol->Write();
         CloseWorkFile();
      } else
         func_numb--;
   }
   if (what == 0)
      ClearMarks();
   if (what == 1)
      ClearMarks();
   return 1;
};

//____________________________________________________________________________

void FitHist::ExecFitMacro()
{
//   cout << fFitMacroName.Data()<< endl;
//   static TString name(fFitMacroName.Data());
   if (fFirstUse) {
      Bool_t ok;
      fFitMacroName =
          GetString("Use Macro:", fFitMacroName.Data(), &ok, mycanvas);
      if (!ok)
         return;
      fFirstUse = 0;
   }
   if (!gSystem->AccessPathName(fFitMacroName.Data(), kFileExists)) {
      gROOT->LoadMacro(fFitMacroName.Data());
      TString cmd = "fit_user_function";
//      Int_t p = cmd.Index(".");
//      cmd.Remove(p,2);
      cmd = cmd + "(\"" + fSelHist->GetName() + "\")";
      cout << cmd << endl;
      cHist->cd();
      gROOT->ProcessLine((const char *) cmd);
//      fSelPad->Modified(kTRUE);
//      fSelPad->Update();
      cHist->Modified(kTRUE);
      cHist->Update();
   } else
      WarnBox("Macro not found");
}

//____________________________________________________________________________________ 


void FitHist::SetTemplateMacro(const char *name)
{
   fTemplateMacro = name;
}

//____________________________________________________________________________________ 


void FitHist::EditFitMacro()
{
   static TString name(fFitMacroName.Data());
   Bool_t ok;
   name = GetString("Name of Fit Macro", name.Data(), &ok, mycanvas);
   if (!ok)
      return;
   if (gSystem->AccessPathName(name.Data())) {
      cout << "Try to open: " << name.Data() << endl;

      ofstream tmpfile(name.Data(), ios::out);
      if (!tmpfile.good()) {
         cerr << "EditFitMacro: "
             << gSystem->GetError() << " - " << name.Data()
             << endl;
         return;
      }
      if (!tmpfile) {
         cout << "Cant open new " << name.Data() << endl;
         return;
      }
      if (fSelHist->GetDimension() == 1) {
      	TOrdCollection *svalues = new TOrdCollection();
      	for (Int_t i = 0; i < nFitTemplates; i++) {
         	TString temp(FitMacroTemplates[i]);
         	Int_t cstart = temp.Index("/*");
         	Int_t cend = -1;
         	if (cstart >= 0) cend = temp.Index("*/");
         	TString mname("NoName");
         	if (cstart >= 0 && cend > cstart) 
            	mname = temp(cstart + 2, cend - cstart - 2);

         	svalues->Add(new TObjString(mname.Data()));
      	}

      	TArrayI flags(nFitTemplates);
      	TString title("Template Fit Macros");
      	Int_t retval;
      	Int_t itemwidth = 240;
      	new TGMrbTableFrame(mycanvas, &retval,
      							  title.Data(),
      							  itemwidth, 1,
      							  nFitTemplates,
      							  svalues, 0, 0, &flags,
      							  nFitTemplates);
      	if (retval < 0) {
         	return;
      	}
      	Bool_t ok = kFALSE;
      	for (Int_t i = 0; i < nFitTemplates; i++) {
         	if (flags[i] == 1) {
            	tmpfile << FitMacroTemplates[i];
            	ok = kTRUE;
            	break;
         	}
      	}
      	if (!ok) {
         	cout << "no selection" << endl;
         	return;
      	}
      } else if (fSelHist->GetDimension() == 2) {
          tmpfile << Gauss2d;
       }
      tmpfile.close();
   }
   TString EditCmd = "nedit ";
   EditCmd += name.Data();
   EditCmd += "&";
   gSystem->Exec(EditCmd.Data());
   fFitMacroName = name;
}

//__________________________________________________________________________________

Bool_t FitHist::SetLinBg()
{
   fLinBgConst = 0;
   fLinBgSlope = 0;
   int nval = GetMarks(fSelHist);
   if (nval != 4) {
      WarnBox("Please define exactly 4 marks");
      return kFALSE;
   }
   markers->Print();
   markers->Sort();
   Double_t xlow = ((FhMarker *) markers->At(0))->GetX();
   Double_t xup = ((FhMarker *) markers->At(3))->GetX();
   Int_t binlow1 =
       XBinNumber(fSelHist, ((FhMarker *) markers->At(0))->GetX());
   Int_t binup1 =
       XBinNumber(fSelHist, ((FhMarker *) markers->At(1))->GetX());
   Int_t binlow2 =
       XBinNumber(fSelHist, ((FhMarker *) markers->At(2))->GetX());
   Int_t binup2 =
       XBinNumber(fSelHist, ((FhMarker *) markers->At(3))->GetX());

   if (binlow1 >= binup1 || binlow2 >= binup2) {
      WarnBox("Illegal bins");
      cout << "binlow1, binup1, binlow2, binup2 " <<
          binlow1 << " " << binup1 << " " << binlow2 << " " << binup2 <<
          endl;
      return kFALSE;
   }

   Double_t meany1 = 0, meany2 = 0;
   for (Int_t i = binlow1; i <= binup1; i++)
      meany1 += fSelHist->GetBinContent(i);
   for (Int_t i = binlow2; i <= binup2; i++)
      meany2 += fSelHist->GetBinContent(i);
   meany1 /= (Double_t) (binup1 - binlow1 + 1);
   meany2 /= (Double_t) (binup2 - binlow2 + 1);
   Double_t meanx1 = 0.5 * (fSelHist->GetBinCenter(binup1)
                            + fSelHist->GetBinCenter(binlow1));
   Double_t meanx2 = 0.5 * (fSelHist->GetBinCenter(binup2)
                            + fSelHist->GetBinCenter(binlow2));
   fLinBgSlope = (meany2 - meany1) / (meanx2 - meanx1);
   fLinBgConst = meany1 - fLinBgSlope * meanx1;
   TF1 *lin_bg = new TF1("lin_bg", "pol1", xlow, xup);
   lin_bg->SetParameters(fLinBgConst, fLinBgSlope);
   cHist->cd();
   lin_bg->Draw("same");
   ClearMarks();
   cHist->Update();
   hp->fFitOptUseLinBg = 1;
   return kTRUE;
}
