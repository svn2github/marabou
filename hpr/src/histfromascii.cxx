#include "TArrayD.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TString.h"
#include "TSystem.h"
#include "TGWindow.h"
#include "TRegexp.h"
#include "HistPresent.h"
#include "TGMrbInputDialog.h"
#include "TGMrbTableFrame.h"
#include <iostream>
#include <fstream>

namespace std {} using namespace std;

//________________________________________________________________________________________

void HistPresent::HistFromASCII(TGWindow * win, HistPresent::EHfromASCIImode mode)
{
   const Char_t helpText[] = "Read values from ASCII file and fill histogram";
//   char temp[100];
   TArrayD xval(100), yval(100), zval(100), wval(100), eyl(100), eyh(100);

   TRegexp endwithasc("asc$");
   Bool_t ok = kTRUE;
   fGraphFile = GetString("Filename",fGraphFile.Data(), &ok, win,0,0,helpText);
//   fname = GetString("Filename",fname.Data(), &ok);
   if (!ok) {
      cout << " Canceled " << endl;
      return; 
   } 

   ifstream infile;
   infile.open(fGraphFile.Data(), ios::in);
	if (!infile.good()) {
	cerr	<< "HistFromASCII: "
			<< gSystem->GetError() << " - " << infile
			<< endl;
		return;
	}
   Int_t n = 0;
//   Double_t x, y, w;
   ok = kTRUE;
   Double_t x[6];
   Int_t nval;
   if      (mode == kSpectrum || mode == k1dimHist)  nval = 1;
   else if (mode == kSpectrumError || mode == k1dimHistWeight 
         || mode == k2dimHist || mode == kGraph)     nval = 2;
   else if (mode == k2dimHistWeight)                 nval = 3;
   else if (mode == kGraphError)                     nval = 4;
   else if (mode == kGraphAsymmError)                nval = 6;
   else {
      cout << "Invalid mode: " << mode << endl;
      return;
   }
  
   
   TString line;
   while ( 1 ) {
      for (Int_t i = 0; i < nval; i++) {
         infile >> x[i];
	      if (infile.eof()) {
             if (i != 0) {
               cout << "Warning: discard the " << i << " value(s) at end of file " << endl;
               cout << ": " << x[0] << endl;
               if (i == 2) cout << ": " << x[1] << endl;
             }
             ok = kFALSE;
			    break;
		   }
         if (!infile.good()) {
            infile.clear();
            infile >> line;
            cout << "Invalid double at or before: " << n+1 << ": " << line << endl;
            ok = kFALSE;
            break;
         }
         if      (i == 0) xval.AddAt(x[i], n);
         else if (i == 1) yval.AddAt(x[i], n);
         else if (i == 2) zval.AddAt(x[i], n);
         else if (i == 3) wval.AddAt(x[i], n);
         else if (i == 4) eyl.AddAt(x[i], n);
         else if (i == 5) eyh.AddAt(x[i], n);
      }
      if (!ok) break;
      n++;
      if (n >= xval.GetSize()){
         xval.Set(n+100);
         if (nval > 1) yval.Set(n+100);
         if (nval > 2) zval.Set(n+100);
         if (nval > 3) wval.Set(n+100);
         if (nval > 4) eyl.Set(n+100);
         if (nval > 5) eyh.Set(n+100);
      }
   }
   infile.close();

   cout << "entries " << n << endl;
   if (n < 1) return;

   Bool_t its_1dimhist = kFALSE;
   Bool_t its_2dimhist = kFALSE;
   Bool_t its_graph    = kFALSE;
   if (mode == kGraph || mode == kGraphError) its_graph= kTRUE;
   if (mode == kSpectrum || mode == kSpectrum 
     || mode == k1dimHist || mode == k1dimHistWeight) its_1dimhist = kTRUE;
   if (mode == k2dimHist || mode == k2dimHistWeight)  its_2dimhist = kTRUE;

   TString hname = fGraphFile;
   Int_t ip = hname.Index(".");
   if (ip > 0) hname.Resize(ip);
   TString htitle = hname;
   htitle.Prepend("Values from ");
   if (!its_graph) hname.Prepend("h_");

   Int_t nbinx, nbiny, nrows;
   Axis_t xlow, xup, ylow , yup;
   Double_t xmin, xmax, ymin, ymax;
   TMrbString temp;

   if (its_1dimhist || its_2dimhist ) {
      hname.Prepend("h_");
      TOrdCollection *row_lab = new TOrdCollection(); 
      TOrdCollection *values  = new TOrdCollection();
      row_lab->Add(new TObjString("Histogram name"));
      row_lab->Add(new TObjString("Histogram title"));
      row_lab->Add(new TObjString("Nbins X"));
      row_lab->Add(new TObjString("Low edge X"));
      row_lab->Add(new TObjString("Upper edge X"));

      values->Add(new TObjString(hname.Data()));
      values->Add(new TObjString(htitle.Data()));

      if (mode == kSpectrum) {
         nbinx = n;
         xlow = 0;
         xup = (Axis_t)n;
         nrows = 5;
      	values->Add(new TObjString(Form("%d",nbinx )));
      	values->Add(new TObjString(Form("%f", xlow)));
      	values->Add(new TObjString(Form("%f", xup)));
      }  else {
         nbinx = 100;
         xmin = xval[TMath::LocMin(n, xval.GetArray())]; 
         xmax = xval[TMath::LocMax(n, xval.GetArray())]; 
         xlow = xmin - 1;
         xup  = xmax + 1;
         nrows = 5;
      	values->Add(new TObjString(Form("%d",nbinx )));
      	values->Add(new TObjString(Form("%f", xlow)));
      	values->Add(new TObjString(Form("%f", xup)));
         if (its_2dimhist) {
         	nbiny = 100;
         	ymin = yval[TMath::LocMin(n, yval.GetArray())]; 
         	ymax = yval[TMath::LocMax(n, yval.GetArray())]; 
         	ylow = ymin - 1;
         	yup  = ymax + 1;
         	nrows += 3;
      		row_lab->Add(new TObjString("Nbins Y"));
      		row_lab->Add(new TObjString("Low edge Y"));
      		row_lab->Add(new TObjString("Upper edge Y"));
      		values->Add(new TObjString(Form("%d", nbiny )));
      		values->Add(new TObjString(Form("%f", ylow)));
      		values->Add(new TObjString(Form("%f", yup)));
         }
      }

      Int_t ret,  itemwidth=120; 
tryagain:
      new TGMrbTableFrame(win, &ret, "Define histogram parameters", 
                        itemwidth, 1, nrows, values,
                        0, row_lab);
//      delete row_lab;
      if (ret < 0) {
         return;
      }
      hname  = ((TObjString*)values->At(0))->GetString();
      htitle = ((TObjString*)values->At(1))->GetString();
      temp = ((TObjString*)values->At(2))->GetString();

      if (!temp.ToInteger(nbinx)) {
         cout << "Illegal integer: " << temp << endl;
         goto tryagain;      
      }
      temp = ((TObjString*)values->At(3))->GetString();
      if (!temp.ToDouble(xlow)) {
         cout << "Illegal double: " << temp << endl;
         goto tryagain;      
      }
      temp = ((TObjString*)values->At(4))->GetString();
      if (!temp.ToDouble(xup)) {
         cout << "Illegal double: " << temp << endl;
         goto tryagain;      
      }
      TH1 * hist = 0;
      if (its_1dimhist) {
         TH1D * hist1 = new TH1D(hname, htitle, nbinx, xlow, xup);
         hist = hist1;
         if (mode == kSpectrum) {
            for (Int_t i = 0; i < n; i++) {
               hist1->SetBinContent(i+1, xval[i]);
            }
         } else if (mode == kSpectrumError) {
            for (Int_t i = 0; i < n; i++) {
               hist1->SetBinContent(i+1, xval[i]);
               hist1->SetBinError(i+1,   yval[i]);
            }
         } else if (mode == k1dimHist) {
            for (Int_t i = 0; i < n; i++) {
               hist1->Fill(xval[i]);
            }
         } else if (mode == k1dimHistWeight) {
            for (Int_t i = 0; i < n; i++) {
               hist1->Fill(xval[i], yval[i]);
            }
         }
      }
      if (its_2dimhist) {
         temp = ((TObjString*)values->At(5))->GetString();
         if (!temp.ToInteger(nbiny)) {
            cout << "Illegal integer: " << temp << endl;
            goto tryagain;      
         }
         temp = ((TObjString*)values->At(6))->GetString();
         if (!temp.ToDouble(ylow)) {
            cout << "Illegal double: " << temp << endl;
            goto tryagain;      
         }
         temp = ((TObjString*)values->At(7))->GetString();
         if (!temp.ToDouble(yup)) {
            cout << "Illegal double: " << temp << endl;
            goto tryagain;      
         }
         TH2F * hist2 = new TH2F(hname, htitle, nbinx, xlow, xup, 
                                               nbiny, ylow, yup);
         hist = hist2;
         if (mode == k2dimHist) {
            for (Int_t i = 0; i < n; i++) {
               hist2->Fill(xval[i], yval[i]);
            }
         } else if (mode == k2dimHistWeight) {
            for (Int_t i = 0; i < n; i++) {
               hist2->Fill(xval[i], yval[i], zval[i]);
            }
         }
      }
      if (hist) ShowHist(hist);
      return;
   } 
   if (mode == kGraph || mode == kGraphError|| mode == kGraphAsymmError) {
      TGraph * graph = 0;
      if (mode == kGraph) {
         graph = new TGraph(n, xval.GetArray(), yval.GetArray()); 
         hname.Prepend("graph_");
      } else if (mode == kGraphError) {
         graph = new TGraphErrors(n, xval.GetArray(), yval.GetArray(),
                                     zval.GetArray(), wval.GetArray());
         hname.Prepend("graph_err_");
      } else if (mode == kGraphAsymmError) {
         graph = new TGraphAsymmErrors(n, xval.GetArray(), yval.GetArray(),
                                          zval.GetArray(), wval.GetArray(),
                                          eyl.GetArray(),  eyh.GetArray());
         hname.Prepend("graph_asymm_err_");
      }
      if (graph) {
         TString cname = hname;
         cname.Prepend("C_");
         if (fNwindows>0) {       // not the 1. time
            if (fWinshiftx != 0 && fNwindows%2 != 0) fWincurx += fWinshiftx;
            else   {fWincurx = fWintopx; fWincury += fWinshifty;}
         }
         fNwindows++;
         HTCanvas * cg = new HTCanvas(cname, htitle, fWincurx, fWincury,
                                    fWinwidx_1dim, fWinwidy_1dim, this, 0, 0, graph);
         fCanvasList->Add(cg);
         graph->SetName(hname);
         graph->SetTitle(htitle);
         graph->Draw(fDrawOptGraph);
         graph->GetHistogram()->SetStats(kFALSE);
      }
   }
   return;
}
;
//____________________________________________________________________________________

const char ExpGaussG[]=
"/*Exponential + Gauss (formula)*/\n\
fit_user_function(const char *gname)\n\
//\n\
// example with fit function given as formula\n\
//\n\
// This is a template macro to fit a user defined function\n\
// As an example an exponential + a gauss function is provided\n\
// The function is defined  as a formula\n\
{\n\
   TGraph * graph = 0;\n\
   if (gPad) {\n\
      graph = (TGraph*)gPad->GetListOfPrimitives()->FindObject(gname);\n\
   }\n\
   if(!graph){\n\
     cout << \"graph not found\" << endl;\n\
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
   graph->Fit(\"exp_gaus\",\"R+\",\"same\");\n\
}\n\
";
//____________________________________________________________________________________

const char LandauG[]=
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
fit_user_function(const char *gname){\n\
\n\
//  ----------------------------------------------------------------------------   \n\
   TGraph * graph = 0;\n\
   if (gPad) {\n\
      graph = (TGraph*)gPad->GetListOfPrimitives()->FindObject(gname);\n\
   }\n\
   if(!graph){\n\
     cout << \"graph not found\" << endl;\n\
     return 0;\n\
   }\n\
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
   graph->Fit(\"landf\",\"R+\",\"same\");   // the real fit\n\
\n\
\n\
}\n\
";
const Int_t nFitTemplatesG = 2;
const char *FitMacroTemplatesG[nFitTemplatesG] = {
   &ExpGaussG[0],
   &LandauG[0],
};

TString gFitMacroNameG;

//____________________________________________________________________________________ 

void EditFitMacroG(TGWindow * win)
{
   const char hist_file[] = {"hprFitMacrosG.txt"};
   const char * hf = hist_file;
   if (gROOT->GetVersionInt() < 40101) hf = NULL;
   static TString name(gFitMacroNameG.Data());
   Bool_t ok;
   name = GetString("Name of Fit Macro", name.Data(), &ok, win,
                     0,0,0,0,0, hf);
   if (!ok)
      return;
   if (gSystem->AccessPathName(name.Data())) {
      cout << "Try to open: " << name.Data() << endl;

      ofstream tmpfile(name.Data(), ios::out);
      if (!tmpfile.good()) {
         cerr << "EditFitMacroG: "
             << gSystem->GetError() << " - " << name.Data()
             << endl;
         return;
      }
      if (!tmpfile) {
         cout << "Cant open new " << name.Data() << endl;
         return;
      }
      TOrdCollection *svalues = new TOrdCollection();
      for (Int_t i = 0; i < nFitTemplatesG; i++) {
         TString temp(FitMacroTemplatesG[i]);
         Int_t cstart = temp.Index("/*");
         Int_t cend = -1;
         if (cstart >= 0) cend = temp.Index("*/");
         TString mname("NoName");
         if (cstart >= 0 && cend > cstart) 
            mname = temp(cstart + 2, cend - cstart - 2);

         svalues->Add(new TObjString(mname.Data()));
      }

      TArrayI flags(nFitTemplatesG);
      TString title("Template Fit Macros");
      Int_t retval;
      Int_t itemwidth = 240;
      new TGMrbTableFrame(win, &retval,
      						  title.Data(),
      						  itemwidth, 1,
      						  nFitTemplatesG,
      						  svalues, 0, 0, &flags,
      						  nFitTemplatesG);
      if (retval < 0) {
         return;
      }
      Bool_t ok = kFALSE;
      for (Int_t i = 0; i < nFitTemplatesG; i++) {
         if (flags[i] == 1) {
            tmpfile << FitMacroTemplatesG[i];
            ok = kTRUE;
            break;
         }
      }
      if (!ok) {
         cout << "no selection" << endl;
         return;
      }
      tmpfile.close();
   }
   TString EditCmd = "nedit ";
   EditCmd += name.Data();
   EditCmd += "&";
   gSystem->Exec(EditCmd.Data());
   gFitMacroNameG = name;
}

//____________________________________________________________________________

void ExecFitMacroG(TGraph * graph, TGWindow * win)
{
//   cout << gFitMacroName.Data()<< endl;
   if (!gPad || !graph) return;
   static TGWindow * winold = 0;
   const char hist_file[] = {"hprFitMacrosG.txt"};
   const char * hf = hist_file;
   if (gROOT->GetVersionInt() < 40101) hf = NULL;
   if (winold != win) {
      Bool_t ok;
      gFitMacroNameG =
          GetString("Use Macro:", gFitMacroNameG.Data(), &ok, win,
          0,0,0,0,0, hf);
      if (!ok)
         return;
      winold = win;
   }
   if (!gSystem->AccessPathName(gFitMacroNameG.Data(), kFileExists)) {
      gROOT->LoadMacro(gFitMacroNameG.Data());
      TString cmd = "fit_user_function";
//      Int_t p = cmd.Index(".");
//      cmd.Remove(p,2);
      cmd = cmd + "(\"" + graph->GetName() + "\")";
      cout << cmd << endl;
      gPad->cd();
      gROOT->ProcessLine((const char *) cmd);
      gPad->Modified(kTRUE);
      gPad->Update();
   } else
      cout << "Macro not found" << endl;
}






























