#include <fstream.h>
#include <iomanip.h>
#include <iostream.h>
#include <strstream.h>

#include "TROOT.h"
#include "TArray.h"
#include "TArrow.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TH2.h"
#include "TObjString.h"
#include "TGMsgBox.h"

#include "FitHist.h"
#include "TGMrbTableFrame.h"
#include "support.h"
#include "TGMrbInputDialog.h"

// Find peaks
Int_t FoldSqareWave(Int_t i, Int_t j,Int_t m){
   if      (i <= j-1 && i >= j-m)     return -1;
   else if (i <= j+m-1 && i >= j)     return  2;
   else if (i <= j+2*m-1 && i >= j+m) return -1; 
   else return 0;
}
  
Int_t FitHist::FindPeaks(){
   Int_t npeaks=0;
   if(is2dim(fSelHist)){
      cout << "FindPeaks in 2 dim not yet supported " << endl;
      return -1;
   }
   Int_t Mwidth=11;
   Int_t bin, i, L, start=0, wid;
   Stat_t fold, var;
   Float_t threshold=4.;
   Int_t nbins   = fSelHist->GetNbinsX();
//   TAxis *yaxis = fSelHist->GetYaxis();
//   Float_t ymin  = fSelHist->GetMinimum();
//   Float_t ymax  = fSelHist->GetMaximum();
   Float_t ymin  = cHist->GetUymin();
   Float_t ymax  = cHist->GetUymax();
 
//   if(fOpfac > 1. && fOpfac < 10) threshold = fOpfac;
   TH1F* h_width = new TH1F("width","Widths",30,0,30);
   for (bin = 1;bin<=nbins-1;bin++) {
      if(bin < Mwidth+1 || bin >= nbins-2*Mwidth-1) {
//         hnew->SetBinContent(bin,0);
      } else {
         fold=0.; var = 0.;
         for(i=bin-Mwidth; i <= bin+2*Mwidth-1; i++){
            L = FoldSqareWave(i,bin,Mwidth);
            fold += L*fSelHist->GetBinContent(i);
            var += L * L * fSelHist->GetBinContent(i);
         }
         if(var) fold = fold /sqrt(var);
         else    fold = 0.;
         if(fold < threshold) {
            fold=0.;
            if(start){
               wid=bin-start;
               h_width->Fill(wid);
//               if(wid > 12)cout << " bin " << bin << " width " << wid << endl;
               start += Mwidth;
               if(start > 0){
                  Float_t x = fSelHist->GetBinCenter(start);
                  Float_t y = ymax*fSelHist->GetBinContent(start)/(ymax-ymin);
                  y += 0.02 * (ymax-ymin);
                  Float_t y1 =  y + 0.04 * (ymax-ymin);
                  TArrow *arr = new TArrow(x,y1,x,y, 0.025);
                  fSelPad->cd();
                  arr->Draw();
                  peaks->Add(arr);
                  npeaks++;
               }
               start = 0;
            }
         } else {
             if(!start) start = bin;
         }
//         hnew->SetBinContent(bin,fold);
      }
   }
   cHist->Modified(kTRUE);
   cHist->Update();
   TCanvas* cc=new TCanvas("wid","Peak widths",300,200);
   h_width->Draw();
   cc->Modified(kTRUE);
   cc->Update();
   return npeaks;
}
//_____________________________________________________________________

Int_t FitHist::ShowPeaks(){
   Int_t npeaks=0;
   cHist->cd();
   Axis_t xmin  = fSelHist->GetXaxis()->GetXmin();
   Axis_t xmax  = fSelHist->GetXaxis()->GetXmax();
   Float_t ymin  = cHist->GetUymin();
   Float_t ymax  = cHist->GetUymax();
   TArrow *arr;
   TIter next(peaks);
//   markers->Sort();
   while ( (arr= (TArrow*)next()) ) {
      Coord_t x=arr->GetX1();
      if(x >= xmin && x <= xmax){
         Int_t start=XBinNumber(fOrigHist,x);
         Coord_t y = ymax*fOrigHist->GetBinContent(start)/(ymax-ymin);
                 y += 0.02 * (ymax-ymin);
         Coord_t y1 =  y + 0.04 * (ymax-ymin);
         arr->DrawArrow(x,y1,x,y,0.025);
         npeaks++;
      }
   }
   return npeaks;
   cHist->Modified(kTRUE);
   cHist->Update();
}
//_____________________________________________________________________

void FitHist::PrintCalib(){
   TIter next(fPeaks);
   FhPeak * peak;
   if(fPeaks->GetSize() > 0) ((FhPeak*)fPeaks->At(0))->PrintHeadLine();
   else    WarnBox("No peaks selected");
   while ( (peak = (FhPeak*)next()) ) {
      peak->Print();  
   }
}
//_____________________________________________________________________

Bool_t FitHist::Calibrate(){
   PrintCalib();
   if(fSetRange){
//       cout << "range was set" << endl;
       WarnBox("Calibration already applied,
use Clear calibration and redisplay");
       return kFALSE;
   }
   Int_t npeaks_def = fPeaks->GetSize();
   Int_t npeaks = npeaks_def;
   Bool_t ok;
   if(npeaks < 2){
      if(npeaks == 0)cout << "No peaks defined yet" << endl;
      else           cout << "Only one peak defined" << endl;
      npeaks = GetInteger("How many peaks to use", 2, &ok, mycanvas);
      if(npeaks <= 0 )return kFALSE;
   }
//   if(npeaks != 2) {
//      WarnBox("Need 2 peaks to calibrate");
//      return kFALSE;
//   }
//   Double_t *xyvals = new Double_t[3*npeaks];
   TArrayD xyvals(3*npeaks);
   TOrdCollection *col_lab = new TOrdCollection(); 
   TOrdCollection *row_lab = new TOrdCollection(); 
   col_lab->Add(new TObjString("Mean"));
   col_lab->Add(new TObjString("Sigma"));
   col_lab->Add(new TObjString("Nom Value"));
   col_lab->Add(new TObjString("Use it"));
   for(Int_t i=0; i<npeaks; i++)row_lab->Add(new TObjString("Values"));

   for(Int_t counter=0; counter<npeaks; counter++){
      xyvals[counter] = 0;
      xyvals[counter+npeaks] = 0;
      xyvals[counter+2*npeaks] = 0;
   }
   if(npeaks_def > 0){
      fPeaks->Sort();
      TIter next(fPeaks);
      FhPeak * peak;
      Int_t counter = 0;
      while ( (peak = (FhPeak*)next()) ) {
         xyvals[counter] = peak->GetMean();
         xyvals[counter+npeaks] = peak->GetWidth();
         counter ++;
      }
   } else {
      xyvals[1] = 1;               // mean
      xyvals[1 + 2*npeaks ] = 2;   // nom value
   }
   TArrayI useflag(npeaks);
   for(Int_t i=0; i<npeaks; i++)useflag[i] = 1;
   Int_t ret, ncols = 3, itemwidth=120, precission = 5; 
   TGMrbTableOfDoubles((TGWindow*)mycanvas, &ret, "Calibration values", 
                         itemwidth, ncols, npeaks, xyvals, precission,
                        col_lab, row_lab, &useflag);
   ok = kFALSE;
   if(ret >= 0){
      Int_t used = 0;
      for(Int_t i=0; i<npeaks; i++)used += useflag[i];
      cout << "used " << used << endl;
//      if(used != 2) {
//         WarnBox("Currently only linear (2 values used) supported");
//         return kFALSE;
//      }
      Axis_t x0 = 1e9, y0 = 0, x1 = -1e9, y1 = 0, a, b;
      used = 0;
      for(Int_t i=0; i<npeaks; i++) if(useflag[i] == 1)used++;
      Float_t *xv = new Float_t[used];
      Float_t *xe = new Float_t[used];
      Float_t *yv = new Float_t[used];
      Float_t *ye = new Float_t[used];
      Int_t j=0;
      for(Int_t i=0; i<npeaks; i++){ 
         if(useflag[i] == 1){
             yv[j] = xyvals[i + npeaks*2];
             ye[j] = 0.0001 * xyvals[i + npeaks*2];
             xv[j] = xyvals[i];
             xe[j] = xyvals[i + npeaks];
//              use first and last point as start 
             if(xv[j] < x0){x0 = xv[j]; y0 = yv[j];};
             if(xv[j] > x1){x1 = xv[j]; y1 = yv[j];};
             j++;
         }
      }

      cout << "x0 " << x0<< " y0 " << y0<< " x1 " << x1<< " y1 " << y1<< endl;
      if(x0 == x1 || y0 == y1){
         WarnBox("x0 == x1 or y0 == y1");
      } else {
         TAxis * xaxis = fSelHist->GetXaxis();
         Int_t lowbin =xaxis->GetFirst();
         Int_t upbin =xaxis->GetLast();
         Axis_t oldlow = xaxis->GetXmin();
         Axis_t oldup  = xaxis->GetXmax();
         Bool_t ok;
         TString formula("pol1");
tryagain:
         formula=GetString("Formula",(const char *)formula, &ok, mycanvas);
         TF1 * calfunc = new TF1("calfunc",formula.Data(), oldlow, oldup);
         if(calfunc->GetNpar() < 2){
            cout << "Need at least pol1" << endl;
            delete calfunc;
            goto tryagain;
         }       
         
         TGraphErrors * gr = new TGraphErrors(used,xv,yv, xe, ye);
         b = (y1 - y0)/(x1 - x0);
         a = y1 - b * x1;
         cout << "def a,b " << a << " " << b << endl;
         calfunc->SetParameters(a,b);
         gr->Fit("calfunc");
         a = calfunc->GetParameter(0);
         b = calfunc->GetParameter(1);
         cout << "fit a,b " << a << " " << b << endl;
         TCanvas * cc = new TCanvas("cc","cc");
         gr->Draw("ALP");
         calfunc->Draw("SAME"); 
         cc->Update(); 
         Axis_t newlow = a + b * oldlow;
         Axis_t newup  = a + b * oldup;
         xaxis->Set(fSelHist->GetNbinsX(), newlow, newup);
         xaxis->SetRange(lowbin,upbin);
         fXtitle = "Energy[keV]";
         xaxis->SetTitle(fXtitle.Data());
         cout << "In this hist: nlow " << newlow << " nup "<< newup << endl;
         Float_t epb = (newup  - newlow) / 
                       (Float_t)fOrigHist->GetNbinsX();
         ostrstream buf;
         buf << "Events/" << epb <<  " keV" << '\0';
         fYtitle = buf.str();
         fSelHist->GetYaxis()->SetTitle(buf.str());
         buf.rdbuf()->freeze(0);
         if(fSelHist != fOrigHist){
            newlow = a + b * fOrigHist->GetXaxis()->GetXmin();
            newup  = a + b * fOrigHist->GetXaxis()->GetXmax();
            fOrigHist->GetXaxis()->SetTitle(fXtitle.Data());
            fOrigHist->GetYaxis()->SetTitle(fYtitle.Data());
            cout << "In orig hist: nlow " << newlow << " nup "<< newup << endl;
         }
         fRangeLowX = newlow;
         fRangeUpX  = newup;
         fSetRange = kTRUE;
         ok = kTRUE;
         fSelHist->GetListOfFunctions()->Delete();
         cHist->cd();
         fSelHist->Draw();
         cHist->Modified(kTRUE);
         cHist->Update();
         SaveDefaults();
   		TString question= "Write function to workfile?";
   		int buttons= kMBYes | kMBNo, retval=0;
   		EMsgBoxIcon icontype = kMBIconQuestion;
   		new TGMsgBox(gClient->GetRoot(), mycanvas,
   		"Qustion",(const char *)question,
   		icontype, buttons, &retval);
   		if(retval == kMBYes){
            TString funcname(calfunc->GetName());
            funcname=GetString("Function name",(const char *)funcname, &ok, mycanvas);
            calfunc->SetName((const char *)funcname);
       		if(OpenWorkFile()){
         		calfunc->Write();
         		CloseWorkFile();
      		}
   		}
     
      }
      if(xv) {delete [] xv; delete [] yv;delete [] xe; delete []  ye;};
   }
//   if (xyvals) delete [] xyvals;
   if(col_lab){ col_lab->Delete(); delete col_lab;}
   return ok;
}
