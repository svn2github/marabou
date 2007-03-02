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
#include "Save2FileDialog.h"
#include "FhPeak.h"

#include <iostream>

using std::cout;
using std::endl;

static const Int_t kSc_Npeaks = 15;
Char_t *Sc_Name[kSc_Npeaks] =
      {"152Eu",    "152Eu",     "152Eu",     "152Eu", 
       "152Eu",    "152Eu",     "152Eu",     "88Y ",   "152Eu", 
       "152Eu",    "152Eu",     "60Co",     "152Eu", 
       "60Co",    "88Y"};
Double_t Sc_Energy[kSc_Npeaks] = 
     {121.780,  244.664,  344.306, 411.114,
      443.968,  778.880,  867.330, 898.010, 964.013,
      1085.817, 1112.050, 1173.231, 1408.030,
      1332.506, 1836.111};
Double_t Sc_EnError[kSc_Npeaks] =
     {0.040,    0.024,    0.024,   0.038, 
      0.038,    0.041,    0.050,   0.0334, 0.041,
      0.053,    0.041,    0.026,   0.046,
      0.028,    0.058};
Double_t Sc_Intensity[kSc_Npeaks]=
     {0.286,   0.074,   0.267,   0.022,
      0.031,   0.129,   0.041,   0.920, 0.146,
      0.099,   0.135,   0.999,    0.208,
      1.000,   0.997}; 
//____________________________________________________________________________________ 

CalibrationDialog::CalibrationDialog(TH1 * hist, Int_t maxPeaks)
{

static const Char_t helptext[] =
"This widget is used to calibrate a 1-dim histogram \n\
The procedure to use previously fitted peaks is as follows:\n\
  - Use the \"Fit Gaussian (with tail)\" dialog to perform \n\
    the required fits (Activate the Option \"Add all functions\n\
  - If this is done before the \"CalibrationDialog\" is invoked\n\
    the table in  the \"CalibrationDialog\" will have as many\n\
    rows as peaks are defined. The default number is 3\n\
  - Invoke this widget (\"CalibrationDialog\") \n\
    Execute (Click) \"Update Peaklist\"\n\
  - Fields \"Mean\" and \"Errors\" of the table should now\n\
    be filled. Complete / modify the table at least  with\n\
    \"Nom Val\" (Nominal values)\n\
    This can be done from \"Set Nominal Vals\", this menu\n\
    presents the lines of the calibration sources 60Co, 88Eu\n\
    and 88Y. THe selected values must correspond to the fitted\n\
\n\
  - Execute \"Calculate Function\", the function is drawn\n\
    in a seperate canvas.\n\
  - If the result is reasonable possibly modify the parameters\n\
    for the \"Calibrated Hist\"\n\
  - Execute \"Fill Calibrated Hist\". A new histogram will be created\n\
    and filled according to the calibration function.\n\
    Note: Each entry in the original histogram is randomly \n\
          shifted within its bin before the calibration is\n\
          applied to avoid bining effects.\n\
\n\
- The Calibration function may be any legal \"TFormala\"\n\
  in practice polynomials of 1. or 2. degree (pol1, pol2)\n\
  are used\n\
- Calibration functions may be saved to and restored from\n\
  a root file. Use \"Save Cal function\" to store it\n\
  and \"Get function from file\" to read from file.\n\
";
   if (maxPeaks <= 0) {
      fMaxPeaks = 3;
   } else if (maxPeaks <= MAXPEAKS) {
      fMaxPeaks = maxPeaks;
   } else {
      fMaxPeaks = MAXPEAKS;
      cout << "Setting fMaxPeaks = " << fMaxPeaks << endl;
   }
#ifdef MARABOUVERS
   fHistPresent = (HistPresent*)gROOT->GetList()->FindObject("mypres");
#endif
   static Int_t fFuncNumber = 0;
//   ClearLocalPeakList();
   fSelHist = hist;
   fCalHist = NULL;
   fCalFunc = NULL;
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
   fFuncName.Prepend("CalF_");
   const char hist_file[] = {"caldialog_hist.txt"};
   fFuncFromFile="workfile.root|TF1|ff";
   RestoreDefaults();

   TList *row_lab = new TList(); 
   static void *valp[100];
   static Int_t dummy = 0;
   Int_t ind = 0;
   static TString cacmd("CalculateFunction()");
   static TString svcmd("SaveFunction()");
   static TString gfcmd("GetFunction()");
   static TString excmd("FillCalibratedHist()");
   static TString udcmd("UpdatePeakList()");
   static TString nvcmd("SetNominalValues()");
//   static TString clcmd("ClearLocalPeakList()");
//   static TString chcmd("ClearHistPeakList()");

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
   row_lab->Add(new TObjString("DoubleValue-Xlow"));
   valp[ind++] = &fXlow;
   row_lab->Add(new TObjString("DoubleValue-Xup"));
   valp[ind++] = &fXup;

   row_lab->Add(new TObjString("StringValue_CalFunction name"));
   valp[ind++] = &fFuncName;

   row_lab->Add(new TObjString("CommandButt_Update Peaklist"));
   valp[ind++] = &udcmd;
   row_lab->Add(new TObjString("CommandButt+SetNominal Vals"));
   valp[ind++] = &nvcmd;
   row_lab->Add(new TObjString("CommandButt+Calculate Function"));
   valp[ind++] = &cacmd;
   row_lab->Add(new TObjString("CommandButt_Fill Calibrated Hist"));
   valp[ind++] = &excmd;
   row_lab->Add(new TObjString("CommandButt+Save Cal Func to File"));
   valp[ind++] = &svcmd;
   row_lab->Add(new TObjString("CommandButt+Get Cal Func from File"));
   valp[ind++] = &gfcmd;
//   row_lab->Add(new TObjString("CommandButt+Clear Peaklist"));
//   valp[ind++] = &clcmd;
 //  row_lab->Add(new TObjString("CommandButt+Clear Hists Plist"));
//   valp[ind++] = &chcmd;

   Int_t itemwidth = 480;
   Int_t ok = 0;
   fDialog =
   new TGMrbValuesAndText ("Calibration function formula", &fFormula, &ok, itemwidth,
                      fParentWindow, hist_file, NULL, row_lab, valp,
                      NULL, NULL, helptext, this, this->ClassName());

}
//________________________________________________________________________
 
void CalibrationDialog::SetNominalValues()
{
static const Char_t helptext[] =
"This widget allows selection of nominal values\n\
from frequently used calibration sources like Europium\n\
";   
   TString label;
   TList *row_lab = new TList(); 
   static TString nvcmd("SetValues()");
   static void *valp[100];
   Int_t ind = 0;
   for (Int_t i = 0; i < kSc_Npeaks; i++) {
      label = "CheckButton_";
      label += Sc_Name[i];
      label += " E=";
      Int_t e = (Int_t)Sc_Energy[i];
      label += e;
      label += " I=";
      label += Sc_Intensity[i];
      row_lab->Add(new TObjString(label));
      valp[ind++] = &fSetFlag[i];
   }
   row_lab->Add(new TObjString("CommandButt_Set Values"));
   valp[ind++] = &nvcmd;
   Int_t itemwidth = 200;
   Int_t ok = 0;
   new TGMrbValuesAndText ("Choose nominal values", NULL, &ok, -itemwidth,
                      fParentWindow, NULL, NULL, row_lab, valp,
                      NULL, NULL, helptext, this, this->ClassName());
//   nvdialog->SetDontCallCloseDown();
}
//________________________________________________________________________
 
void CalibrationDialog::SetValues()
{
   Int_t nset = 0;
   for (Int_t i = 0; i < kSc_Npeaks; i++) {
      if (fSetFlag[i] != 0) {
         if (nset >= fNpeaks) {
            cout << "Maximum " << fNpeaks << " selections possible" << endl;
            return;
         }
         fY[nset]=  Sc_Energy[i];
         cout << "fY[" << nset << "] = " << fY[nset] << endl;
         fYE[nset] = Sc_EnError[i];
         nset++;
      }
   }
   fDialog->ReloadValues();
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
   gr->SetMarkerStyle(4);
   gr->SetMarkerSize(2);
   for (Int_t i = 0; i < fNpeaks; i++) {
      if (fUse[i] == 0) gr->RemovePoint(i);
   }
/*
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
*/
   if (fCalFunc->GetNpar() > 2) {
      for (Int_t i = 2; i < fCalFunc->GetNpar(); i++) {
         fCalFunc->SetParameter(i, 0);
      }
   }
   gr->Fit(fFuncName);
   cout << endl << "Fitted values a: " 
        <<  fCalFunc->GetParameter(0)<< " +- " << fCalFunc->GetParError(0) << " b: " 
        << fCalFunc->GetParameter(1)<< " +- " << fCalFunc->GetParError(1) << endl;

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
   cout << "fNbinsX ,xl, xu " << fNbinsX << " xl" << xl<< "xu " << xu<< endl;
/*
   Int_t nbins;
   Double_t BinWidth;
   THLimitsFinder::Optimize(xl, xu, fNbinsX , fXlow, fXup, 
                      nbins, BinWidth, "");
   cout << "nbins , BinWidth " << nbins << " " << BinWidth << endl;
   fDialog->ReloadValues();
   fNbinsX = nbins;
*/
}

//________________________________________________________________________________

void CalibrationDialog::GetFunction()
{
static const Char_t helptext[] =
"Select file name with the browser (use arrow)\n\
Click on the required function name to select it\n\
Then use \"Read function\" to read it in.\n\
Note: As default the last entry is selected\n\
";
   TList *row_lab = new TList(); 
   static TString nvcmd("SetValues()");
   static void *valp[100];
   Int_t ind = 0;
   static TString gfcmd("ExecuteGetFunction()");

   row_lab->Add(new TObjString("FileContReq_File name"));
   valp[ind++] = &fFuncFromFile;
   row_lab->Add(new TObjString("CommandButt_Read function"));
   valp[ind++] = &gfcmd;
   Int_t itemwidth = 300;
   Int_t ok = 0;
   new TGMrbValuesAndText ("Get Function from file", NULL, &ok, -itemwidth,
                      fParentWindow, NULL, NULL, row_lab, valp,
                      NULL, NULL, helptext, this, this->ClassName());
}
//________________________________________________________________________________

void CalibrationDialog::ExecuteGetFunction()
{
   TString fname;
   TString cname;
   TString oname;

	TObjArray * oa = fFuncFromFile.Tokenize("|");
	Int_t nent = oa->GetEntries();
   if (nent < 3) {
      cout << "fFuncFromFile not enough (3) fields" << endl;
      return;
   }
	fname =((TObjString*)oa->At(0))->String();
	cname =((TObjString*)oa->At(1))->String();
   if (cname != "TF1") {
      cout << "fFuncFromFile " << oname << " is not a function " << endl;
      return;
   }
	oname =((TObjString*)oa->At(2))->String();
   TFile f(fname);
   fCalFunc = (TF1*)f.Get(oname);
   if (fCalFunc == NULL) {
      cout << "No function found / selected" << endl;
      return;
   }
   fCalFunc->Print();
   Double_t xl = fCalFunc->Eval(fSelHist->GetXaxis()->GetBinLowEdge(1));
   Double_t xu = fCalFunc->Eval(fSelHist->GetXaxis()
                  ->GetBinUpEdge(fSelHist->GetNbinsX()));
//   Int_t nbins;
//   Double_t BinWidth;
   cout << "fNbinsX ,xl, xu " << fNbinsX << " xl" << xl<< "xu " << xu<< endl;
//   THLimitsFinder::Optimize(xl, xu, fNbinsX , fXlow, fXup, 
 //                     nbins, BinWidth, "");
//   cout << "nbins , BinWidth " << nbins << " " << BinWidth << endl;
 //  fDialog->ReloadValues();
 //  fNbinsX = nbins;
}
//____________________________________________________________________________________ 

void CalibrationDialog::SaveFunction()
{
   if (!fCalFunc) {
      cout << "No function defined" << endl;
      return;
   } 
   if (!fCalHist) {
      cout << "Execute Fill histogram first" <<  endl;
      return;
   } 
   fCalFunc->SetParent(fCalHist);
   fSelHist->GetListOfFunctions()->Add(fCalFunc);
   new Save2FileDialog(fCalFunc);
}
//____________________________________________________________________________________ 

void CalibrationDialog::FillCalibratedHist()
{
   if (fCalFunc == NULL) {
      cout << "Calculate function first" << endl;
      return;
   }
   TString hname_cal;
   hname_cal = fSelHist->GetName();
   hname_cal += "_cal";
   TString title_cal(fSelHist->GetTitle());
   title_cal += "_calibrated";
   title_cal += ";Energy[KeV];Events[";
   title_cal += Form("%4.2f", (fXup-fXlow)/(Double_t)fNbinsX);
   title_cal += " KeV]";
   cout << "title_cal: " << title_cal << endl;
   TH1* hh = (TH1*)gROOT->GetList()->FindObject(hname_cal);
   if (hh) delete hh;
   if      (!strcmp(fSelHist->ClassName(), "TH1F"))
   	fCalHist = new TH1F(hname_cal, title_cal, fNbinsX, fXlow, fXup);
   else if (!strcmp(fSelHist->ClassName(), "TH1D"))
   	fCalHist = new TH1D(hname_cal, title_cal, fNbinsX, fXlow, fXup);
   else if (!strcmp(fSelHist->ClassName(), "TH1S"))
   	fCalHist = new TH1S(hname_cal, title_cal, fNbinsX, fXlow, fXup);
   else
   	fCalHist = new TH1C(hname_cal, title_cal, fNbinsX, fXlow, fXup);

//   under - overflows of origin hist are taken as they are
   fCalHist->SetBinContent(0, fSelHist->GetBinContent(0));
   fCalHist->SetBinContent(fCalHist->GetNbinsX()+1, 
                           fSelHist->GetBinContent(fSelHist->GetNbinsX()+1));
//  update number of entries
   fCalHist->SetEntries(fSelHist->GetBinContent(0) + 
                        fSelHist->GetBinContent(fSelHist->GetNbinsX()+1));
// shuffle bins
   for (Int_t bin = 1; bin <= fSelHist->GetNbinsX(); bin++) {
      Axis_t binw = fSelHist->GetBinWidth(bin);
      Axis_t bcent = fSelHist->GetBinCenter(bin);
      for (Int_t cnt = 0; cnt < fSelHist->GetBinContent(bin); cnt++) {
         Axis_t bcent_r = bcent + binw  * (gRandom->Rndm() - 0.5);
         Axis_t bcent_cal = fCalFunc->Eval(bcent_r);
         fCalHist->Fill(bcent_cal);
      }
   }
#ifdef MARABOUVERS
   if (fHistPresent) {
      fHistPresent->ShowHist(fCalHist);
//   in case a title was set to a stored value
      fCalHist->SetTitle(title_cal);
      gPad->Modified();
   } else {
#endif
      TString title(hname_cal);
      title.Prepend("C_");
      TCanvas *ch = new TCanvas("ccal", title, 500, 100, 600,400);
      fCalHist->Draw("E");
      ch->Update();
#ifdef MARABOUVERS
   }
#endif
}
//____________________________________________________________________________________ 
/*
void CalibrationDialog::ClearLocalPeakList()
{
   for (Int_t i =0; i < fMaxPeaks; i++) {
      fX[i] = fXE[i] = fY[i] = fYE[i] = 0;
      fUse[i] = 0;
   }
   fNpeaks = 0;
}
*/
//____________________________________________________________________________________ 
/*
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
*/
//____________________________________________________________________________________ 

void CalibrationDialog::UpdatePeakList()
{
//   Int_t npeaks = 0;
   TIter next(fSelHist->GetListOfFunctions());
   TObject *obj;
   TF1 *f;
   TString pname;
   TList *plist = new TList();
   Double_t cont, mean, error;
   while ( (obj = next()) ) {
      if (obj->IsA() == TF1::Class()) {
         f = (TF1*)obj;
         for (Int_t i = 0; i < f->GetNpar(); i++) {
            pname = f->GetParName(i);
            if (pname.BeginsWith("Ga_Mean")) {
               mean =  f->GetParameter(i);
               error = f->GetParError(i);
               cont= 0;
               if (i > 0) cont= f->GetParameter(i-1);
               cout << "fNpeaks: " << fNpeaks<<  " Mean: " << mean  << " Error: " << error << endl;
	//          look if already stored
            	Bool_t store_it = kTRUE;
            	if (fNpeaks > 0) {
               	for (Int_t k = 0; k < fNpeaks; k++) { 
                  	if (TMath::Abs((fX[k] - mean) / fX[k]) < 0.0001) store_it = kFALSE;
               	}
            	}
            	if (store_it && fNpeaks < fMaxPeaks) {
                  FhPeak *p = new FhPeak(mean);
                  p->SetWidth(error);
                  p->SetContent(cont);
                  plist->Add(p);
               	fNpeaks++;
            	} else {
               	if (!store_it)
                  	cout << mean << " already in list" << endl;
               	else 
                  	cout << "More than " << fMaxPeaks << " peaks" << endl;
            	}
            }
         }
      }
   }   
   if (fNpeaks < 1) {
      plist->Delete();
      delete plist;
      cout << " Not enough peaks defined: " << fNpeaks << endl;
      return;
   }
   plist->Sort();
   TIter next1(plist);
   FhPeak *p;
   fNpeaks = 0;
   while ( (p = (FhPeak*)next1()) ) {
	   fX [fNpeaks] = p->GetMean();
    	fXE[fNpeaks] = p->GetWidth();
	   fY [fNpeaks] = 0;
	   fYE[fNpeaks] = 1;
	   fUse[fNpeaks] = 1;
      fCont[fNpeaks] = p->GetContent();
      fNpeaks++;
   }
   plist->Delete();
   delete plist;
/*
   // try association with Europium peaks
   Double_t ratio;
   Double_t ratio_fl = fX[fNpeaks-1] / fX[0];
   Double_t limit = 0.01 * Sc_Energy[kSc_Npeaks-1] / Sc_Energy[0];
   cout << "ratio_fl " << ratio_fl << " limit " << limit<< endl; 
   for (Int_t i = 0; i < kSc_Npeaks; i++) {
      for (Int_t k = i+1; k < kSc_Npeaks; k++) {
         ratio = Sc_Energy[k] / Sc_Energy[i];
         cout << "i, k, E[i] ratio " << i << " " << k 
              << " " << Sc_Energy[i] << " " << ratio << endl;
         if (TMath::Abs(ratio - ratio_fl) < limit) {
            fY[0] = Sc_Energy[i];
            fY[fNpeaks-1] = Sc_Energy[k];
         }
      }
   }      
   cout << "Peak list" << endl;
   for (Int_t i=0; i < fNpeaks; i++) {
      cout << fCont[i] << " " << fX[i] << " "<< fXE[i] << " " << fY[i] << " " << fYE[i] << endl;
   }
*/
}
//_______________________________________________________________________

void CalibrationDialog::RestoreDefaults()
{
   TEnv env(".rootrc");
   fNbinsX  = env.GetValue("CalibrationDialog.BinsX", 4000);
   fXlow    = env.GetValue("CalibrationDialog.fXlow", 0.);
   fXup     = env.GetValue("CalibrationDialog.fXup", 4000.);
   fFormula = env.GetValue("CalibrationDialog.Formula", "pol1");
   fFuncFromFile = env.GetValue("CalibrationDialog.fFuncFromFile", "workfile.root|TF1|calf");
}
//_______________________________________________________________________

void CalibrationDialog::SaveDefaults()
{
   cout << "CalibrationDialog:: SaveDefaults() kEnvLocal" << endl;
   TEnv env(".rootrc");
   env.SetValue("CalibrationDialog.BinsX", fNbinsX);
   env.SetValue("CalibrationDialog.fXlow", fXlow);
   env.SetValue("CalibrationDialog.fXup", fXup);
   env.SetValue("CalibrationDialog.Formula", fFormula.Data());
   env.SetValue("CalibrationDialog.fFuncFromFile", fFuncFromFile);
   env.SaveLevel(kEnvLocal);
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
