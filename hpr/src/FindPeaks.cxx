#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

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
#include "TGMrbInputDialog.h"
#include "CalibrationDialog.h"
#include "support.h"

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
   Int_t Mwidth = 11;
   if (hp)  Mwidth = hp->fPeakMwidth;
   Int_t bin, i, L, start=0, wid;
   Stat_t fold, var;
   Float_t threshold = 3.;
   if (hp) threshold = hp->fPeakThreshold;
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
               start += (Int_t)(0.5 * Mwidth);
               if(start > 0){
                  Float_t x = fSelHist->GetBinCenter(start);

//                  Float_t y = ymax*fSelHist->GetBinContent(start)/(ymax-ymin);
//                  y += 0.02 * (ymax-ymin);
 //                 Float_t y1 =  y + 0.04 * (ymax-ymin);
//                  TArrow *arr = new TArrow(x,y1,x,y, 0.025);
                  TLine *arr = new TLine(x,ymin,x, ymax);
                  arr->SetLineColor(2);
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
   cHist->Modified(kTRUE);
   cHist->Update();
   return npeaks;
   
}
//_____________________________________________________________________

void FitHist::PrintCalib(){
   TIter next(fPeaks);
   FhPeak * peak;
   if(fPeaks->GetSize() > 0) {
   cout << endl << "Data used for calibration" << endl;
   cout         << "-------------------------" << endl;
      ((FhPeak*)fPeaks->At(0))->PrintHeadLine();
       while ( (peak = (FhPeak*)next()) ) {
         peak->Print();  
      }
   }
//  else    WarnBox("No peaks selected");
}
//_____________________________________________________________________

void FitHist::ClearCalibration(){
   if (expHist) {
      expHist->GetListOfFunctions()->Clear("nodelete");
      expHist->Delete();
      expHist = NULL;
      fSelHist = fOrigHist;
      Draw1Dim();
   }
   fSetRange = kFALSE;
   fSelHist->GetXaxis()->SetTitle("");
   fSelHist->GetYaxis()->SetTitle("");
   fSelHist->GetXaxis()->Set(fOrigHist->GetNbinsX(), fOrigLowX, fOrigUpX);
   SaveDefaults();
   cHist->Modified(kTRUE);
   cHist->Update();
}

//_____________________________________________________________________

Bool_t FitHist::Calibrate(Int_t flag){
//
// flag: 0 change scale of current histogram only
//       1 generate a new histogram wit new binning

   if (flag == 2) {
      new CalibrationDialog(fSelHist);
      return kTRUE;
   }
   if(fSetRange){
       WarnBox("Calibration already applied,\n\
use Clear calibration and redisplay");
       return kFALSE;
   }
   Int_t npeaks_def = fPeaks->GetSize();
   Int_t npeaks = npeaks_def;
   Bool_t ok;
   TGWindow * window_for_cal = 0;
   if(npeaks < 2){
      if(npeaks == 0)cout << "No peaks defined yet" << endl;
      else           cout << "Only one peak defined" << endl;
      npeaks = GetInteger("How many peaks to use", 2, &ok, mycanvas);
      if (npeaks < 2)return kFALSE;
   }
//   if(npeaks != 2) {
//      WarnBox("Need 2 peaks to calibrate");
//      return kFALSE;
//   }
//   Double_t *xyvals = new Double_t[3*npeaks];
   TArrayD xyvals(4*npeaks);
   TOrdCollection *col_lab = new TOrdCollection(); 
   TOrdCollection *row_lab = new TOrdCollection(); 
   col_lab->Add(new TObjString("Mean"));
   col_lab->Add(new TObjString("Error"));
   col_lab->Add(new TObjString("Nom Value"));
   col_lab->Add(new TObjString("Error "));
   col_lab->Add(new TObjString("Use it"));
   for(Int_t i=0; i<npeaks; i++)row_lab->Add(new TObjString("Values"));

   for(Int_t counter=0; counter<npeaks; counter++){
      xyvals[counter] = counter;
      xyvals[counter+npeaks] = 1;
      xyvals[counter+2*npeaks] = counter;
      xyvals[counter+3*npeaks] = 1;
   }
   if (npeaks_def > 0) {
      fPeaks->Sort();
      TIter next(fPeaks);
      FhPeak * peak;
      Int_t counter = 0;
      while ( (peak = (FhPeak*)next()) ) {
         xyvals[counter] = peak->GetMean();
         xyvals[counter+npeaks] = peak->GetWidth();
          xyvals[counter+ 2 * npeaks] = peak->GetNominalEnergy();
        counter ++;
      }
   } 
/*
else {
      xyvals[0] = 0;               // mean
      xyvals[1] = 1;               // mean
      xyvals[2] = 1;               // err
      xyvals[1 + 2*npeaks ] = 2;   // nom value
      xyvals[2 + 2*npeaks ] = 1;   // err
   }
*/
   TArrayI useflag(npeaks);
   for(Int_t i=0; i<npeaks; i++)useflag[i] = 1;
   Int_t ret = 0, ncols = 4, itemwidth=120, precission = 5; 
   TGMrbTableOfDoubles((TGWindow*)mycanvas, &ret, "Calibration values", 
                         itemwidth, ncols, npeaks, xyvals, precission,
                        col_lab, row_lab, &useflag);
   if(npeaks_def > 0){
      TIter next(fPeaks);
      FhPeak * peak;
      Int_t counter = 0;
      while ( (peak = (FhPeak*)next()) ) {
         peak->SetNominalEnergy(xyvals[counter + 2 * npeaks]);
        counter ++;
      }
   }
   ok = kFALSE;
   if(ret >= 0){
     TGraphErrors * gr = 0;
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
      FhPeak * peak;
      
      for(Int_t i=0; i<npeaks; i++){ 
         if (i< fPeaks->GetSize()) {
           peak = (FhPeak *)fPeaks->At(i);
           peak->SetNominalEnergy(xyvals[i + npeaks*2]);
           peak->SetUsed(useflag[i] == 1);
         }
         if(useflag[i] == 1){
             yv[j] = xyvals[i + 2 * npeaks];
             ye[j] = xyvals[i + 3 * npeaks];
             xv[j] = xyvals[i];
             xe[j] = xyvals[i + npeaks];
//              use first and last point as start 
             if(xv[j] < x0){x0 = xv[j]; y0 = yv[j];};
             if(xv[j] > x1){x1 = xv[j]; y1 = yv[j];};
             j++;
         }
      }

//      cout << "x0 " << x0<< " y0 " << y0<< " x1 " << x1<< " y1 " << y1<< endl;
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
         TString funcname(fHname);
         funcname += "_calfunc";

tryagain:
         formula=GetString("Formula",(const char *)formula, &ok, mycanvas);
         if (fCalFunc) {delete fCalFunc; fCalFunc = NULL;}
         fCalFunc = new TF1(funcname,formula.Data(), oldlow, oldup);
         if (!ok) return ok;
         if(fCalFunc->GetNpar() < 2){
            WarnBox("Need at least pol1");
            delete fCalFunc; fCalFunc = NULL;
            goto tryagain;
         }       
         if(fCalFunc->GetNpar() > 2  && flag == 0)
            WarnBox("Only first 2 parameters\n will be used for calib");
        
         gr = new TGraphErrors(used,xv,yv, xe, ye);
         b = (y1 - y0)/(x1 - x0);
         a = y1 - b * x1;
         cout << "Number of points: " << used << " start a,b " << a << " " << b << endl;
         fCalFunc->SetParameters(a,b);
         if (fCalFunc->GetNpar() > 2) {
            for (Int_t i = 2; i < fCalFunc->GetNpar(); i++) {
               fCalFunc->SetParameter(i, 0);
            }
         }
         gr->Fit(funcname);
         a = fCalFunc->GetParameter(0);
         b = fCalFunc->GetParameter(1);
         cout << endl << "Fitted values a: " 
              << a << " +- " << fCalFunc->GetParError(0) << " b: " 
              << b << " +- " << fCalFunc->GetParError(1) << endl;

         TCanvas * cc = new TCanvas("cc","cc", 200, 200, 500, 500);
         gr->Draw("A*");
         PrintGraph(gr);
         fCalFunc->Draw("SAME"); 
         fCalFunc->SetLineWidth(1);
         fCalFunc->SetLineColor(7);
         cc->Update();

         if (flag == 0) {
         	Axis_t newlow = a + b * oldlow;
         	Axis_t newup  = a + b * oldup;
         	xaxis->Set(fSelHist->GetNbinsX(), newlow, newup);
         	xaxis->SetRange(lowbin,upbin);
         	fXtitle = "Energy[keV]";
         	xaxis->SetTitle(fXtitle.Data());
         	PrintCalib();
         	cout << "In this hist: nlow " << newlow << " nup "<< newup << endl;
         	Float_t epb = (newup  - newlow) / 
                     	  (Float_t)fOrigHist->GetNbinsX();
         	TString buf ("Events/");
         	buf += Form("%f", epb);
         	buf += " keV";
         	fYtitle = buf.Data();
         	fSelHist->GetYaxis()->SetTitle(buf.Data());
         	if(fSelHist != fOrigHist){

            	Axis_t newlowo = a + b * fOrigHist->GetXaxis()->GetXmin();
            	Axis_t newupo  = a + b * fOrigHist->GetXaxis()->GetXmax();
            	fOrigHist->GetXaxis()->SetTitle(fXtitle.Data());
            	fOrigHist->GetYaxis()->SetTitle(fYtitle.Data());
            	fOrigHist->GetXaxis()->Set(fOrigHist->GetNbinsX(), newlowo, newupo);

            	cout << "In orig hist: nbin: " << fOrigHist->GetNbinsX()
               	  << " nlow " << newlowo 
               	  << " nup "<< newupo << endl;
         	}
         	fRangeLowX = newlow;
         	fRangeUpX  = newup;
         	fSetRange = kTRUE;
         	ok = kTRUE;
         	cHist->Modified(kTRUE);
         	cHist->Update();
         } else if (flag == 1) {
//            if (fCalFitHist) {delete fCalFitHist; fCalFitHist = 0;}
   
            col_lab->Delete();
            row_lab->Delete();
            row_lab->Add(new TObjString("Nbins"));
            row_lab->Add(new TObjString("Low edge"));
            row_lab->Add(new TObjString("Up edge"));
            xyvals[0] = 1000;
            xyvals[1] = 0;
            xyvals[2] = fCalFunc->Eval(oldup);
            ncols = 1; 
            Int_t nrows = 3;
            ret = 0;
            TGMrbTableOfDoubles((TGWindow*)mycanvas, &ret, 
                                "Parameters for calibrated histogram", 
                                itemwidth, ncols, nrows, xyvals, precission,
                                0, row_lab);
            if (ret >= 0) {

   				TH1 * hist = fSelHist;
   				Int_t  nbin_cal = (Int_t)xyvals[0];
   				Axis_t low_cal  = (Axis_t)xyvals[1];
   				Axis_t up_cal = (Axis_t)xyvals[2];
               if (fCalHist) {delete fCalHist; fCalHist = 0;}
               fCalHist = calhist(hist, fCalFunc, nbin_cal, 
                                  low_cal, up_cal,(const char *)fHname);

   				if (hp) {
                  fCalFitHist = hp->ShowHist(fCalHist);
                  fCalFitHist->Entire();
                  window_for_cal = fCalFitHist->GetMyCanvas();
               }                  
            }
         }
         SaveDefaults();
//
         TGWindow * parent;
         if (window_for_cal) parent = window_for_cal;
         else                parent = mycanvas;
   		TString question= "Save function to workfile?";
   		int buttons= kMBYes | kMBNo, retval=0;
   		EMsgBoxIcon icontype = kMBIconQuestion;
   		new TGMsgBox(gClient->GetRoot(), parent,
   		"Qustion",(const char *)question,
   		icontype, buttons, &retval);
   		if (retval == kMBYes) {

            TString funcname(fCalFunc->GetName());
//            funcname=GetString("Function name",(const char *)funcname, &ok, mycanvas);
//            fCalFunc->SetName((const char *)funcname);
            funcname+="_graph";
            gr->SetName((const char *)funcname);
            gr->SetTitle((const char *)funcname);
           
       		if(OpenWorkFile(parent)){
         		fCalFunc->Write();
               gr->Write();
         		CloseWorkFile();
      		}
   		}
     
      }
      if(xv) {delete [] xv; delete [] yv;delete [] xe; delete []  ye;};
      if (gr) delete gr;
   }
//   if (xyvals) delete [] xyvals;
   if(col_lab){ col_lab->Delete(); delete col_lab;}
//   cout << fSelHist->GetXaxis()->GetTitle() << endl;

   return ok;
}
