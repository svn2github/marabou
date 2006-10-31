#include "TCanvas.h"
#include "TH1D.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TEnv.h"
#include "TObjString.h"
#include "TList.h"
#include "TRandom.h"
#include "TGMsgBox.h"
#include "THLimitsFinder.h"
#include "CalibrationDialog.h"
#include "FhPeak.h"

#include <iostream>

using std::cout;
using std::endl;
//____________________________________________________________________________________ 

CalibrationDialog::CalibrationDialog(TH1 * hist, Int_t maxPeaks)
{

static const Char_t helptext[] =
"This widget is used to calibrate a 1-dim histogram \n\
";
   if (maxPeaks <= 0) {
      fMaxPeaks = 3;
   } else if (maxPeaks <= MAXPEAKS) {
      fMaxPeaks = maxPeaks;
   } else {
      fMaxPeaks = MAXPEAKS;
      cout << "Setting fMaxPeaks = " << fMaxPeaks << endl;
   }
   fHistPresent = (HistPresent*)gROOT->GetList()->FindObject("mypres");
   static Int_t fFuncNumber = 0;
   ClearLocalPeakList();
   fSelHist = hist;
   if (!fSelHist) {
      cout << "No hist selected" << endl;
      return;
   }

   if (fSelHist->GetDimension() != 1) {
      cout << "Can only be used with 1-dim hist" << endl;
      return;
   }
   fParentWindow =  NULL;
   fSelPad = gPad;
   if (fSelPad == NULL) {
     cout << "gPad = 0!!" <<  endl;
   } else {
      fParentWindow = (TRootCanvas*)fSelPad->GetCanvas()->GetCanvasImp();
   }
//  function name
   fFuncName = fSelHist->GetName();
   Int_t ip = fFuncName.Index(";");
	if (ip > 0)fFuncName.Resize(ip);
   fFuncNumber++;
   fFuncName.Prepend(Form("%d_", fFuncNumber));
   fFuncName.Prepend("f");
   const char hist_file[] = {"caldialog_hist.txt"};
   RestoreDefaults();

   TList *row_lab = new TList(); 
   static void *valp[100];
   static Int_t dummy = 0;
   Int_t ind = 0;
   static TString cacmd("CalculateFunction()");
   static TString excmd("FillCalibratedHist()");
   static TString udcmd("UpdatePeakList()");
   static TString clcmd("ClearLocalPeakList()");
   static TString chcmd("ClearHistPeakList()");

   row_lab->Add(new TObjString("CommentOnly_Parameters for Polynomial Fit"));
   valp[ind++] = &dummy;
   row_lab->Add(new TObjString("CommentOnly_Mean"));
   valp[ind++] = &dummy;
   row_lab->Add(new TObjString("CommentOnly+Error"));
   valp[ind++] = &dummy;
   row_lab->Add(new TObjString("CommentOnly+Nom Val"));
   valp[ind++] = &dummy;
   row_lab->Add(new TObjString("CommentOnly+Error"));
   valp[ind++] = &dummy;
   row_lab->Add(new TObjString("CommentRigh+Use it "));
   valp[ind++] = &dummy;
   for (Int_t i = 0; i < fMaxPeaks; i++) {
      row_lab->Add(new TObjString("DoubleValue_"));
      valp[ind++] = &fX[i];
      row_lab->Add(new TObjString("DoubleValue+"));
      valp[ind++] = &fXE[i];
      row_lab->Add(new TObjString("DoubleValue+"));
      valp[ind++] = &fY[i];
      row_lab->Add(new TObjString("DoubleValue+"));
      valp[ind++] = &fYE[i];
      row_lab->Add(new TObjString("CheckButton+"));
      valp[ind++] = &fUse[i];
   } 
   row_lab->Add(new TObjString("CommentOnly_Parameters for Calibrated Hist"));
   valp[ind++] = &dummy;
   row_lab->Add(new TObjString("PlainIntVal_Nbins"));
   valp[ind++] = &fNbinsX;
   row_lab->Add(new TObjString("DoubleValue+Xlow"));
   valp[ind++] = &fXlow;
   row_lab->Add(new TObjString("DoubleValue+Xup"));
   valp[ind++] = &fXup;
   row_lab->Add(new TObjString("StringValue_CalFunction name"));
   valp[ind++] = &fFuncName;

   row_lab->Add(new TObjString("CommandButt_Calculate Function"));
   valp[ind++] = &cacmd;
   row_lab->Add(new TObjString("CommandButt+Fill Calibrated Hist"));
   valp[ind++] = &excmd;
   row_lab->Add(new TObjString("CommandButt_Update Peaklist"));
   valp[ind++] = &udcmd;
   row_lab->Add(new TObjString("CommandButt+Clear Local Plist"));
   valp[ind++] = &clcmd;
   row_lab->Add(new TObjString("CommandButt+Clear Hists Plist"));
   valp[ind++] = &chcmd;

   Int_t itemwidth = 480;
   Int_t ok = 0;
   fDialog =
   new TGMrbValuesAndText ("Calibration function formula", &fFormula, &ok, itemwidth,
                      fParentWindow, hist_file, NULL, row_lab, valp,
                      NULL, NULL, helptext, this, this->ClassName());

}
//____________________________________________________________________________________ 

void CalibrationDialog::CalculateFunction()
{
   if (fCalFunc) {delete fCalFunc; fCalFunc = NULL;}
   fCalFunc = new TF1(fFuncName,fFormula);
   if(fCalFunc->GetNpar() < 2){
      cout << "Need at least pol1" << endl;
      delete fCalFunc; fCalFunc = NULL;
      return;
   }       
   TGraphErrors *gr = new TGraphErrors(fNpeaks, fX , fY, fXE, fYE);
   Int_t ip_xmin = fNpeaks;
   Int_t ip_xmax = -1;
   Double_t xmin = 1e30;
   Double_t xmax = -1e30;
   for (Int_t i = 0; i < fNpeaks; i++) {
      if (fX[i] < xmin) {
         xmin = fX[i];
         ip_xmin = i;
      }
      if (fX[i] > xmax) {
         xmax = fX[i];
         ip_xmax = i;
      }
   }
   Double_t b = (fY[ip_xmax] - fY[ip_xmin]) / (fX[ip_xmax] - fX[ip_xmin]);
   Double_t a =  fY[ip_xmax]- b * fX[ip_xmax];
   cout << "Number of points: " << fNpeaks << " start a,b " << a << " " << b << endl;
   fCalFunc->SetParameters(a,b);
   if (fCalFunc->GetNpar() > 2) {
      for (Int_t i = 2; i < fCalFunc->GetNpar(); i++) {
         fCalFunc->SetParameter(i, 0);
      }
   }
   gr->Fit(fFuncName);
   a = fCalFunc->GetParameter(0);
   b = fCalFunc->GetParameter(1);
   cout << endl << "Fitted values a: " 
        << a << " +- " << fCalFunc->GetParError(0) << " b: " 
        << b << " +- " << fCalFunc->GetParError(1) << endl;

   TCanvas * cc = new TCanvas("cc","cc", 200, 200, 500, 500);
   gr->Draw("AP");
//   PrintGraph(gr);
   fCalFunc->Draw("SAME"); 
   fCalFunc->SetLineWidth(1);
   fCalFunc->SetLineColor(7);
   cc->Update();
   Double_t xl = fCalFunc->Eval(fSelHist->GetXaxis()->GetBinLowEdge(1));
   Double_t xu = fCalFunc->Eval(fSelHist->GetXaxis()
                  ->GetBinUpEdge(fSelHist->GetNbinsX()));
   Int_t nbins;
   Double_t BinWidth;
   THLimitsFinder::Optimize(xl, xu, fNbinsX , fXlow, fXup, 
                      nbins, BinWidth, "");
   cout << "nbins , BinWidth " << nbins << " " << BinWidth << endl;
   fNbinsX = nbins;
}
//____________________________________________________________________________________ 

void CalibrationDialog::FillCalibratedHist()
{
   TString hname_cal;
   hname_cal = fSelHist->GetName();
   hname_cal += "_cal";
   TString title_cal(fSelHist->GetTitle());
   title_cal += "_calibrated";
   title_cal += ";Energy[KeV];Events[";
   title_cal += Form("%4.2f", (fXup-fXlow)/(Double_t)fNbinsX);
   title_cal += " KeV]";
   TH1 * hist_cal;
   if      (!strcmp(fSelHist->ClassName(), "TH1F"))
   	hist_cal = new TH1F(hname_cal, title_cal, fNbinsX, fXlow, fXup);
   else if (!strcmp(fSelHist->ClassName(), "TH1D"))
   	hist_cal = new TH1D(hname_cal, title_cal, fNbinsX, fXlow, fXup);
   else if (!strcmp(fSelHist->ClassName(), "TH1S"))
   	hist_cal = new TH1S(hname_cal, title_cal, fNbinsX, fXlow, fXup);
   else
   	hist_cal = new TH1C(hname_cal, title_cal, fNbinsX, fXlow, fXup);

//   under - overflows of origin hist are taken as they are
   hist_cal->SetBinContent(0, fSelHist->GetBinContent(0));
   hist_cal->SetBinContent(hist_cal->GetNbinsX()+1, 
                           fSelHist->GetBinContent(fSelHist->GetNbinsX()+1));
//  update number of entries
   hist_cal->SetEntries(fSelHist->GetBinContent(0) + 
                        fSelHist->GetBinContent(fSelHist->GetNbinsX()+1));
// shuffle bins
   for (Int_t bin = 1; bin <= fSelHist->GetNbinsX(); bin++) {
      Axis_t binw = fSelHist->GetBinWidth(bin);
      Axis_t bcent = fSelHist->GetBinCenter(bin);
      for (Int_t cnt = 0; cnt < fSelHist->GetBinContent(bin); cnt++) {
         Axis_t bcent_r = bcent + binw  * (gRandom->Rndm() - 0.5);
         Axis_t bcent_cal = fCalFunc->Eval(bcent_r);
         hist_cal->Fill(bcent_cal);
      }
   }
   if (fHistPresent) {
      fHistPresent->ShowHist(hist_cal);
   } else {
      TString title(hname_cal);
      title.Prepend("C_");
      TCanvas *ch = new TCanvas("ccal", title, 500, 100, 600,400);
      hist_cal->Draw("E");
      ch->Update();
   }
}
//____________________________________________________________________________________ 

void CalibrationDialog::ClearLocalPeakList()
{
   for (Int_t i =0; i < fMaxPeaks; i++) {
      fX[i] = fXE[i] = fY[i] = fYE[i] = 0;
      fUse[i] = 0;
   }
   fNpeaks = 0;
}
//____________________________________________________________________________________ 

void CalibrationDialog::ClearHistPeakList()
{
   TList *lof = fSelHist->GetListOfFunctions();
   TIter next(lof);
   TObject *obj;
   while (obj = next()) {
      if (obj->IsA() == FhPeak::Class()) {
         lof->Remove(obj);
      }
   }    
}
//____________________________________________________________________________________ 

void CalibrationDialog::UpdatePeakList()
{
   TIter next(fSelHist->GetListOfFunctions());
   TObject *obj;
   while (obj = next()) {
      if (obj->IsA() == FhPeak::Class()) {
         if (fNpeaks < fMaxPeaks) {
            FhPeak *p = (FhPeak*)obj;
//          look if already stored
            Bool_t store_it = kTRUE;
            if (fNpeaks > 0) {
//               Int_t im = (Int_t)(1000 * p->GetMean());
               
               for (Int_t i = 0; i < fNpeaks; i++) { 
//                  Int_t im1 = (Int_t)(1000 * fX[i]);

 //                 cout << hex << p->GetMean() << " " << hex <<  fX[i]<< endl;        
 //                 cout << "i, im, im1 "  << i << " " << im << " " << im1 << endl;        
 //                 if ((Int_t)(1000 * fX[i]) == im) store_it = kFALSE;
                  if ((fX[i] - p->GetMean()) / fX[i] < 0.0001) store_it = kFALSE;
               }
            }
            if (store_it) {
               fX [fNpeaks] = p->GetMean();
               fXE[fNpeaks] = p->GetWidth();
               fY [fNpeaks] = p->GetNominalEnergy();
               fYE[fNpeaks] = 1;
               fUse[fNpeaks] = 1;
   //            fYE[fNpeaks] = p->GetNominalEnergyError();
               fNpeaks++;
            } else {
               cout << p->GetMean() << " already in list" << endl;
            }
         } else {
            cout << "More than " << fMaxPeaks << " peaks" << endl;
         }
      }
   }        
}
//_______________________________________________________________________

void CalibrationDialog::RestoreDefaults()
{
   TEnv env(".rootrc");
   fNbinsX  = env.GetValue("CalibrationDialog.BinsX", 1000);
   fFormula = env.GetValue("CalibrationDialog.Formula", "pol1");
}
//_______________________________________________________________________

void CalibrationDialog::SaveDefaults()
{
   TEnv env(".rootrc");
   env.SetValue("CalibrationDialog.BinsX", fNbinsX);
   env.SetValue("CalibrationDialog.Formula", fFormula.Data());
   env.SaveLevel(kEnvUser);
}
//_______________________________________________________________________

void CalibrationDialog::CloseDialog()
{
//   cout << "CalibrationDialog::CloseDialog() " << endl;
   gROOT->GetListOfCleanups()->Remove(this);
   fDialog->CloseWindow();
}
//_______________________________________________________________________

void CalibrationDialog::CloseDown()
{
//   cout << "CalibrationDialog::CloseDown() " << endl;
   SaveDefaults();
   delete this;
}