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































