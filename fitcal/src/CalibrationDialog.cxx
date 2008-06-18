#include "TROOT.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TLine.h"
#include "TH1D.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TEnv.h"
#include "TObjString.h"
#include "TList.h"
#include "TMath.h"
#include "TRandom.h"
#include "TGMsgBox.h"
#include "THLimitsFinder.h"
#include "CalibrationDialog.h"
#include "Save2FileDialog.h"
#include "FhPeak.h"
#include "SetColor.h"
#include <fstream>
//#include "support.h"

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

CalibrationDialog::CalibrationDialog(TH1 * hist, Int_t interactive)
{

static const Char_t helptext[] =
"This widget is used to calibrate a 1-dim histogram \n\
The procedure to use previously fitted peaks is as follows:\n\
  - Use the \"Fit Gaussian (with tail)\" dialog to perform \n\
    the required fits. The Option \"Add all functions\"\n\
    must be activated.\n\
  - This must be  done before the \"CalibrationDialog\" is invoked.\n\
    In that case the table in the \"CalibrationDialog\" \n\
    will have as many rows as peaks are defined. \n\
    The default number is 3\n\
\n\
    Execute \"Update Peaklist\" in this dialog\n\
\n\
  - Fields \"Mean\" and \"Errors\" of the table should now\n\
    be filled. Complete / modify the table at least with\n\
    \"Nom Val\" (Nominal values)\n\
\n\
    This can be done from \"Set Nominal Vals\": This menu\n\
    presents the lines of the calibration sources 60Co, 192Eu\n\
    and 88Y or the custom provided ones according the\n\
    selection of \"Cal source\".\n\
\n\
  - An automatic procedure for this assignment is provided:\n\
    It uses a histogramming method to find the best assignments\n\
    between the calibration peaks and the measured ones \n\
    by varying offset and gain. Note: this procedure\n\
    assumes a linear calibration. For detailed info consult\n\
    the \"Help\" provided with the corresponding dialog widget\n\
\n\
  - Execute \"Calculate Function\": A TGraphErrors is created \n\
    using the peak and nominal values. The function is\n\
    fitted to this graph to evaluate the parameters\n\
  - The parameters for the \"Calibrated Hist\" (Nbins,\n\
    Xlow, Xup) should be adjusted to reasonable values.\n\
  - Execute \"Fill Calibrated Hist\". A new histogram will be\n\
    created and filled according to the calibration function\n\
    Note: Each entry in the original histogram is randomly \n\
          shifted within its bin before the calibration is\n\
          applied to avoid binning effects.\n\
\n\
- The Calibration function may be any legal \"TFormula\"\n\
  in practice polynomials of 1. or 2. degree (pol1, pol2)\n\
  are used\n\
\n\
- For test purposes mainly a bare formula containing all \n\
  needed parameters may be given, e.g: 200 + 0.7*x + 0.1*x*x\n\
\n\
- Calibration functions may be saved to and restored from\n\
  a root file. Use \"Save Cal function\" to store it\n\
  and \"Get function from file\" to read from file.\n\
";
   fInteractive = interactive;
   fGaugeNpeaks = 0;
   fSelHist = hist;
   if (!fSelHist) {
      cout << "No hist selected" << endl;
      return;
   }

   if (fSelHist->GetDimension() != 1) {
      cout << "Can only be used with 1-dim hist" << endl;
      return;
   }
   gROOT->GetListOfCleanups()->Add(this);
	fDialog = NULL;
	fDialogSetNominal = NULL;
	fAutoSelectDialog = NULL;
   fCalHist = NULL;
   fCalFunc = NULL;
	fCalValCanvas = NULL;
	fScanCanvas = NULL;
	fEffCanvas = NULL;
   fUpdatePeakListDone = 0;
   RestoreDefaults();

   Int_t maxPeaks = FindNumberOfPeaks();
   if (maxPeaks <= 0) {
      fMaxPeaks = 3;
   } else if (maxPeaks <= MAXPEAKS) {
      fMaxPeaks = maxPeaks;
   } else {
      fMaxPeaks = MAXPEAKS;
   }
   for (Int_t i = 0; i < MAXPEAKS; i++) {
      fX[i] = fY[i] = fXE[i] = fYE[i] = fCont[i] = 0;
      fUse[i] = fAssigned[i] = 0;
   }
   cout << "Setting fMaxPeaks = " << fMaxPeaks << endl;
#ifdef MARABOUVERS
   fHistPresent = (HistPresent*)gROOT->GetList()->FindObject("mypres");
#endif
   static Int_t fFuncNumber = 0;
   fAutoAssigned = 0;
   fParentWindow =  NULL;
   fSelPad = gPad;
//   cout << "fSelPad " <<  fSelPad << endl;
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
   if (fInteractive > 0) {
		TList *row_lab = new TList();
		static void *valp[1000];
		static Int_t dummy = 0;
		Int_t ind = 0;
		static TString cacmd("CalculateFunction()");
		static TString svcmd("SaveFunction()");
		static TString gfcmd("GetFunction()");
		static TString excmd("FillCalibratedHist()");
		static TString udcmd("UpdatePeakList()");
		static TString nvcmd("SetNominalValues()");
		static TString rdcmd("QueryReadGaugeFile()");
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
		row_lab->Add(new TObjString("CommentOnly+Content"));
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
			row_lab->Add(new TObjString("DoubleValue+"));
			valp[ind++] = &fCont[i];
			row_lab->Add(new TObjString("CheckButton+"));
			valp[ind++] = &fUse[i];
		}
		row_lab->Add(new TObjString("CommentOnly_Parameters for Calibrated Hist: "));
		valp[ind++] = &dummy;
		row_lab->Add(new TObjString("StringValue-CalFunction name"));
		valp[ind++] = &fFuncName;
		row_lab->Add(new TObjString("PlainIntVal_Nbins"));
		valp[ind++] = &fCalibratedNbinsX;
		row_lab->Add(new TObjString("DoubleValue-Xlow"));
		valp[ind++] = &fCalibratedXlow;
		row_lab->Add(new TObjString("DoubleValue-Xup"));
		valp[ind++] = &fCalibratedXup;

		row_lab->Add(new TObjString("CommentOnly_Cal source"));
		valp[ind++] = &dummy;
		row_lab->Add(new TObjString("RadioButton+Eu152"));
		valp[ind++] = &fEu152Gauge;
		row_lab->Add(new TObjString("RadioButton+Co60"));
		valp[ind++] = &fCo60Gauge;
		row_lab->Add(new TObjString("RadioButton+Y88"));
		valp[ind++] = &fY88Gauge;
		row_lab->Add(new TObjString("RadioButton+Custom"));
		valp[ind++] = &fCustomGauge;
		row_lab->Add(new TObjString("StringValue_Custom gauge file"));
		valp[ind++] = &fCustomGaugeFile;
      row_lab->Add(new TObjString("CheckButton-Verbose Print"));
      valp[ind++] = &fVerbose;

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
}
//________________________________________________________________________

void CalibrationDialog::SetBuiltinGaugeValues()
{
	fGaugeEnergy.Set(kSc_Npeaks);
	fGaugeError.Set(kSc_Npeaks);
	fGaugeIntensity.Set(kSc_Npeaks);
   TString name;
   Int_t n = 0;
   if        (fEu152Gauge) {
      name = "152Eu";
   } else if ( fCo60Gauge ) {
      name = "60Co";
   } else if ( fY88Gauge ) {
      name = "88Y";
   } else {
      cout << "No builtin gauge source selected" << endl;
      return;
   }
   fGaugeName.Clear();
 	for (Int_t i = 0; i < kSc_Npeaks; i++) {
      TString gn (Sc_Name[i]);
      if ( gn.Contains(name) ) {
			fGaugeEnergy[n]= Sc_Energy[i];
			fGaugeError[n] = Sc_EnError[i];
			fGaugeIntensity[n] = Sc_Intensity[i];
			fGaugeName.Add(new TObjString(Sc_Name[i]));
         fSetFlag[n] = 1;
         n++;
      }
//      TString cn(Sc_Name[i]);
//      if (cn.Contains("Eu"))
//         ofile << Sc_Name[i] << " " << Sc_Energy[i] << " "
//            << Sc_EnError[i] << " " << Sc_Intensity[i] << endl;
	}
   fGaugeNpeaks = n;
}
//________________________________________________________________________

void CalibrationDialog::SetNominalValues()
{
static const Char_t helptext[] =
"This widget allows selection of nominal values\n\
from frequently used calibration sources like Europium\n\
or user (custom) provided file\n\
This file has one line /peak:\n\
\"name energy error rel_intensity\"\n\
\n\
The following options are provided:\n\
  - Select the gauge peaks manually and press:\n\
    \"Choose selected\"\n\
    The selected values must correspond to the\n\
    fitted and selected ones: i.e. One has to activate\n\
    the checkbuttons correctly in both tables.\n\
  - Press \"Auto select\":\n\
    The program will vary gain and offset and find\n\
    the best match between the (fitted) peaks in the\n\
    spectrum and the gauge peaks. The sensivity can\n\
    be adjusted in newly created widget.\n\
    For more details consult \"Help\" in that widget\n\
";

   if (fCustomGauge) {
      if ( ReadGaugeFile() <=0 )  {
//         WarnBox("Read gauge file first", fParentWindow);
         Int_t retval;
         new TGMsgBox(gClient->GetRoot(), fParentWindow,"Warning","Read gauge file first",kMBIconExclamation, kMBDismiss, &retval);
         return;
      }
   } else {
      SetBuiltinGaugeValues();
   }
   TString label;
   TList *row_lab = new TList();
   static TString nvcmd("SetValues()");
   static TString ascmd("AutoSelectDialog()");
   static void *valp[100];
   Int_t ind = 0;
   for (Int_t i = 0; i < fGaugeNpeaks; i++) {
      label = "CheckButton_";
      TObjString *gn = (TObjString*)fGaugeName.At(i);
      label += gn->String();
      label += " E=";
      Int_t e = (Int_t)fGaugeEnergy[i];
      label += e;
      label += " I=";
      label += fGaugeIntensity[i];
      row_lab->Add(new TObjString(label));
      valp[ind++] = &fSetFlag[i];
   }
   row_lab->Add(new TObjString("CommandButt_Choose selected"));
   valp[ind++] = &nvcmd;
   row_lab->Add(new TObjString("CommandButt_Auto Select"));
   valp[ind++] = &ascmd;
   Int_t itemwidth = 200;
   Int_t ok = 0;
   fDialogSetNominal =
   new TGMrbValuesAndText ("Choose nominal values", NULL, &ok, -itemwidth,
                      fParentWindow, NULL, NULL, row_lab, valp,
                      NULL, NULL, helptext, this, this->ClassName());
    if (ok);
//   nvdialog->SetDontCallCloseDown();
}
//________________________________________________________________________

void CalibrationDialog::QueryReadGaugeFile()
{
//      cout << "Not yet impl" << endl;
static const Char_t helptext[] =
"Read gauge peaks from file.\n\
Format: Name(eg. 60Co) energy error intensity\n\
";
	TList *row_l = new TList();
	static TString rdcmd("ReadGaugeFile()");
	static void *va[10];
	Int_t ind = 0;
	row_l->Add(new TObjString("FileRequest_Filename"));
	va[ind++] = &fCustomGaugeFile;
	row_l->Add(new TObjString("CommandButt_Read values"));
	va[ind++] = &rdcmd;
	Int_t itemwidth = 400;
	Int_t ok = 0;
	new TGMrbValuesAndText ("Read gauge values", NULL, &ok, -itemwidth,
							fParentWindow, NULL, NULL, row_l, va,
							NULL, NULL, helptext, this, this->ClassName());
}
//________________________________________________________________________

Int_t CalibrationDialog::ReadGaugeFile()
{
   if (fCustomGaugeFile.Length() <= 0 ||
      gSystem->AccessPathName(fCustomGaugeFile.Data())) {
      TString warn("Cant open ");
      warn += fCustomGaugeFile;
//      WarnBox(warn, fParentWindow);
      Int_t retval;
      new TGMsgBox(gClient->GetRoot(), fParentWindow,"Warning",warn ,kMBIconExclamation, kMBDismiss, &retval);
      return 0;
   }
   ifstream infile;
   infile.open(fCustomGaugeFile.Data(), ios::in);
	if (!infile.good()) {
	cerr	<< "ReadGaugeFile: "
			<< gSystem->GetError() << " - " << infile
			<< endl;
		return 0;
	}
   Int_t n = 0;
   TString line;
   TString val;
   TString del(" ,");
   TObjArray * oa;
   fGaugeEnergy.Set(100);
   fGaugeError.Set(100);
   fGaugeIntensity.Set(100);
  	while ( 1 ) {
		line.ReadLine(infile);
		if (infile.eof()) break;
      cout << "line: " << line << endl;
		oa = line.Tokenize(del);
		Int_t nent = oa->GetEntries();
		if ( nent < 2 ) {
			cout << "Not enough entries: " << nent << endl;
			break;
		}
		fGaugeName.Add( (TObjString*)oa->At(0));

		val = ((TObjString*)oa->At(1))->String();
//       cout << "val " << val<< endl;

		if (!val.IsFloat()) {
			cout << "Illegal double: " << val << " at line: " << n+1 << endl;
			return 0;
		}
		fGaugeEnergy.AddAt(val.Atof(), n);
		if (nent > 2) {
			val = ((TObjString*)oa->At(2))->String();
//         cout << "val " << val<< endl;
			if (!val.IsFloat()) {
				cout << "Illegal double: " << val << " at line: " << n+1 << endl;
				return 0;
			}
			fGaugeError.AddAt(val.Atof(), n);
		} else {
			fGaugeError.AddAt(0.1, n);
      }
		if (nent > 3) {
			val = ((TObjString*)oa->At(3))->String();
//         cout << "val " << val<< endl;
			if (!val.IsFloat()) {
				cout << "Illegal double: " << val << " at line: " << n+1 << endl;
				return 0;
			}
			fGaugeIntensity.AddAt(val.Atof(), n);
		} else {
			fGaugeError.AddAt(0.1, n);
      }
//      cout << "fGaugeEnergy[" << n << "] " << fGaugeEnergy[n] << endl;
		n++;
		if (n >= fGaugeEnergy.GetSize()){
			fGaugeEnergy.Set(n+100);
			fGaugeError.Set(n+100);
			fGaugeIntensity.Set(n+100);
		}
	}
   fGaugeEnergy.Set(n);
   fGaugeError.Set(n);
   fGaugeIntensity.Set(n);
   fGaugeNpeaks = n;
   cout << "ReadGaugeFile, peaks read: " << n << endl;
   return n;
}

//________________________________________________________________________

void CalibrationDialog::AutoSelectDialog()
{
static const Char_t helptext[] =
"  Vary gain and offset and find the best match\n\
   between the (fitted) peaks in the measured spectrum\n\
   and the gauge peaks. \"Xlow\" and \"Xup\" define the range\n\
   of the testbed histogram. Units are those of the \n\
   calibrated histogram, therefore the values of the \n\
   gauge peaks must be within the range of this histogram.\n\
   The sensivity can be adjusted by varying Nbins and \n\
   Gain/Offset Steps.\n\
   Smaller \"Nbins\" leads to a wider binwidth of the \n\
   test histogram resulting in more likely matches\n\
   but finally lead to ambiguities.\n\
   The binwidths should be typically 2-4 times the peakwidth\n\
   \"Accept Limits\" defines the maximum allowed difference in\n\
   the position of the calibrated and gauge peak __after__\n\
   the matching process.\n\
   Peaks with small (gauss)content may be suppressed by\n\
   \"ContentThresh\" measured as fraction of the max peak.\n\
   Watch out, this may suppress real peaks.\n\
 ";
   TString label;
   TList *row_lab = new TList();
   static TString ascmd("ExecuteAutoSelect()");
   static Int_t dummy;
   static void *valp[100];
   Int_t ind = 0;
   row_lab->Add(new TObjString("CommentOnly_Parameters for Testbed histogram"));
   valp[ind++] = &dummy;
   row_lab->Add(new TObjString("PlainIntVal_Nbins"));
   valp[ind++] = &fMatchNbins;
   row_lab->Add(new TObjString("DoubleValue+Xlow"));
   valp[ind++] = &fMatchMin;
   row_lab->Add(new TObjString("DoubleValue+Xup"));
   valp[ind++] = &fMatchMax;
   row_lab->Add(new TObjString("DoubleValue_GainMin"));
   valp[ind++] = &fGainMin;
   row_lab->Add(new TObjString("DoubleValue+GainMax"));
   valp[ind++] = &fGainMax;
   row_lab->Add(new TObjString("DoubleValue+GainStep"));
   valp[ind++] = &fGainStep;
   row_lab->Add(new TObjString("DoubleValue_OffMin"));
   valp[ind++] = &fOffMin;
   row_lab->Add(new TObjString("DoubleValue+OffMax"));
   valp[ind++] = &fOffMax;
   row_lab->Add(new TObjString("DoubleValue+OffStep"));
   valp[ind++] = &fOffStep;
   row_lab->Add(new TObjString("DoubleValue_ContentThresh"));
   valp[ind++] = &fContThresh;
   row_lab->Add(new TObjString("DoubleValue+Accept Limit"));
   valp[ind++] = &fAccept;
   row_lab->Add(new TObjString("CheckButton_Verbose Print"));
   valp[ind++] = &fVerbose;
   row_lab->Add(new TObjString("CommandButt+ExecuteSelect"));
   valp[ind++] = &ascmd;

   Int_t itemwidth = 400;
   Int_t ok = 0;
   fAutoSelectDialog =
   new TGMrbValuesAndText ("Auto select", NULL, &ok, -itemwidth,
                      fParentWindow, NULL, NULL, row_lab, valp,
                      NULL, NULL, helptext, this, this->ClassName());
}
//________________________________________________________________________

Bool_t CalibrationDialog::ExecuteAutoSelect()
{
   if ( fGaugeNpeaks == 0) {
      if ( fCustomGauge ) {
         ReadGaugeFile();
      } else {
         SetBuiltinGaugeValues();
      }
   }
   if (fGaugeNpeaks == 0) {
      cout << "Cant set GaugeValues" << endl;
      return kFALSE;
   }
   UpdatePeakList();
   if (fNpeaks < 2) {
      cout << "Need at least 2 peaks defined" << endl;
      return kFALSE;
   }
   for (Int_t i = 0; i < fGaugeNpeaks; i++) {
	   if (fVerbose)
         cout << "Gauge Peak: " << fGaugeEnergy[i] << endl;
      if (fGaugeEnergy[i] <= fMatchMin || fGaugeEnergy[i] >= fMatchMax) {
          cout << "Gauge Peak: " << fGaugeEnergy[i]
               << " outside testbed histogram range: ["
               << fMatchMin << ", " << fMatchMax <<  "]" << endl;
          return kFALSE;
      }
   }
   DisableDialogs();
   static Int_t print_done = 0;
   for (Int_t i = 0; i < MAXPEAKS; i++)
      fAssigned[i] = 0;
   TH1F * htest;
   htest = (TH1F*)gDirectory->GetList()->FindObject("testbed");
   if (htest) delete htest;
   htest = new TH1F("testbed", "Test bed" ,fMatchNbins, fMatchMin, fMatchMax);
   for (Int_t i = 0; i < fNpeaks; i++)
      fAssigned[i] = 0;
   for (Int_t i = 0; i < fGaugeNpeaks; i++)
      fSetFlag[i] = -1;
// find max content
   Double_t maxcont = 0, maxpos = 0;
	for (Int_t i = 0; i < fNpeaks; i++) {
	   if (fUse[i] > 0  && fCont[i] > maxcont) {
         maxcont = fCont[i];
         maxpos  = fX[i];
      }
	}
   if ( maxcont <= 0 ) {
      cout << "No peak with content > 0 found" << endl;
      return kFALSE;
   }
   Int_t ngood = 0;
   for (Int_t i = 0; i < fNpeaks; i++) {
      if (fUse[i] > 0  && fCont[i] > maxcont * fContThresh) ngood++;
   }
   if (ngood <= 1) {
      cout << setred << "Only 1 peak left after threshold check: "
      << endl << "Maximum cont: " << maxcont << " at: " << maxpos
      << setblack << endl;
      return kFALSE;
   }

   Int_t best = 0;
   Double_t best_off  = 0;
   Double_t best_gain = 0;
   Double_t off, gain;
   Int_t nbscanX = TMath::Nint((fOffMax - fOffMin + fOffStep) / fOffStep);
   Int_t nbscanY = TMath::Nint((fGainMax - fGainMin + fGainStep) / fGainStep);
   TH2C *hscan = (TH2C*)gDirectory->GetList()->FindObject("hscan");
   if (hscan) {
      gDirectory->GetList()->Remove(hscan);
      delete hscan;
   }
   hscan = new TH2C("hscan", "Matches vs offset and gain;Offset;Gain",
                 nbscanX, fOffMin - 0.5*fOffStep, fOffMax + 0.5*fOffStep,
                 nbscanY,fGainMin- 0.5*fGainStep, fGainMax + 0.5*fGainStep);
   for (off = fOffMin; off <= fOffMax; off += fOffStep) {
      for (gain = fGainMin; gain <= fGainMax; gain += fGainStep) {
			htest->Reset();
			for (Int_t i = 0; i < fGaugeNpeaks; i++)
				htest->Fill(fGaugeEnergy[i]);
			for (Int_t i = 0; i < fNpeaks; i++) {
            if (fUse[i] > 0  && fCont[i] >= maxcont * fContThresh) {
				   Double_t e = off + gain*fX[i];
				   htest->Fill(e);
            }
			}
			Int_t twohits = 0;
			for (Int_t i = 1; i <= htest->GetNbinsX(); i++) {
				if ( htest->GetBinContent(i) >= 2) twohits ++;
				if ( htest->GetBinContent(i) >  2 ) {
              if ( print_done < 1 )
                 std::cout << "At least 1 ambigous: " << htest->GetBinContent(i)
                 << " at Energy: " <<  htest->GetBinCenter(i) << std::endl;
                 print_done++;
             }
			}
         hscan->Fill(off,gain,(Char_t)twohits);
			if (twohits > best) {
				best = twohits;
				best_off = off;
				best_gain = gain;
			}
      }
   }
   if (best < 2) {

      cout << " No or only 1 match found, you might need to lower Nbins in testbed histogram" << endl;
      return kFALSE;
   }
   if (fVerbose) {
      std::cout << "Best match: Hits, offset, gain " <<  best << " " << best_off << " " << best_gain<< std::endl;
//      cout << "Checking for ambiguities (should be 1 line only)" << endl;
   }
   Int_t namb = 0;
   for (Int_t io = 1; io <= hscan->GetNbinsX(); io++) {
      for (Int_t ig = 1; ig <= hscan->GetNbinsY(); ig++) {
         Int_t ic = (Int_t)hscan->GetCellContent(io, ig);
         if (ic == best) {
            if (fVerbose && namb > 1)
              printf("Ambiguous: at Offset, gain: %10.4f %10.4f found: %2d matches\n",
                   hscan->GetXaxis()->GetBinCenter(io),
                   hscan->GetYaxis()->GetBinCenter(ig), best);
            namb++;
         }
      }
   }
   if (fVerbose && namb > 1) {
      cout << "Warning: " << namb -1  << " ambiguities found " << endl;
      cout << "-------------------------------------------" << endl;
   }
   Double_t max_cont_ass = 0;
   Int_t nassigned = 0;
   fSelPad->cd();
   for (Int_t i = 0; i < fNpeaks; i++) {
      fAssigned[i] = -1;
      if (fUse[i] > 0  && fCont[i] < maxcont * fContThresh) {
         if (fVerbose)
            cout << "Discard too small peak at: " << fX[i]
                 << " Cont: " << fCont[i] << endl;
         continue;
      }
	   Double_t e = best_off + best_gain*fX[i];
      Double_t ecal = 0;
      Double_t closest = 10000;

 	   for (Int_t j = 0; j < fGaugeNpeaks; j++) {
			if (TMath::Abs(e - fGaugeEnergy[j]) < closest) {
            closest = TMath::Abs(e - fGaugeEnergy[j]);
            ecal = e;
            best = j;
         }
      }
      if (fVerbose)
         printf("Channel, Energy, Gauge: %8.2f %8.2f %8.2f", fX[i], e, fGaugeEnergy[best]);
      if (closest < fAccept) {
         if (fVerbose)
           printf(" accepted %3d\n", best);
         fAssigned[i] = best;
         if ( fSetFlag[best] >= 0 ) {
            Int_t which_ass = -1;
            for (Int_t k = 0; k < i; k++) {
               if (fAssigned[k] == best)
                  which_ass = k;
            }
            cout << setred << "When assigning peak at: " << fX[i] << endl;
            cout << "Warning: Calibration peak "
                 << fGaugeEnergy[best] <<  " already assigned to "
                 << fX[which_ass] << endl;
            cout << "You should decrease \"Accept Limit\" now at: "
                 << fAccept << setblack << endl;
            e = best_off + best_gain*fX[i];
            Double_t epr  = best_off + best_gain*fX[which_ass];

            if ( TMath::Abs(e - fGaugeEnergy[best]) <
               ( TMath::Abs(epr - fGaugeEnergy[best]))) {
               fAssigned[which_ass] = -1;
               cout << "Remove assignment from " << fX[which_ass] << endl;
            } else {
               cout << "Keep assignment for " << fX[which_ass] << endl;
               continue;
            }
            cout << setblack << endl;
         }
         fSetFlag[best] = 1;
         if (fUse[i] > 0  && fCont[i] > max_cont_ass)
            max_cont_ass = fCont[i];
         nassigned++;

      } else {
         if (fVerbose)
            printf("\n");
      }
   }
	for (Int_t i = 0; i < fNpeaks; i++) {
		best = fAssigned[i];
		if ( best < 0 ) continue;
		Int_t bin = fSelHist->FindBin(fX[i]);
		Double_t yv = fSelHist->GetBinContent(bin);
		Double_t yr = fSelHist->GetMaximum();
		Double_t xr = fSelHist->GetBinCenter(fSelHist->GetXaxis()->GetLast())
						- fSelHist->GetBinCenter(fSelHist->GetXaxis()->GetFirst());
		TLine * l = new TLine(fX[i] + 0.025 * xr, yv + 0.025*yr, fX[i], yv);
		l->SetLineWidth(2);
		l->Draw();

		TString t;
		TObjString *gn = (TObjString*)fGaugeName.At(best);
		t = gn->String();

		Int_t ie = (Int_t) fGaugeEnergy[best];
		t +=  "(";
		t += ie;
		t += ")";
		TLatex latex;
		latex.SetTextSize(0.02);
		latex.SetTextColor(kBlue);
		latex.DrawLatex(fX[i] + 0.025 * xr, yv + 0.025*yr, t);
		gPad->Modified();
		gPad->Update();
	}
   TIter next(&fPeakList);
   FhPeak * p;
   Int_t np = 0;
   while ( (p = (FhPeak *) next()) ) {
     Int_t ass = fAssigned[np];
     if (ass >= 0) {
       fY[np]=  fGaugeEnergy[ass];
       fYE[np] = fGaugeError[ass];
       fUse[np] = 1;
       p->SetUsed(1);
       p->SetNominalEnergy(fY[np]);
       p->SetNominalEnergyError(fYE[np]);
       p->SetIntensity(fGaugeIntensity[ass]);
       p->SetRelEfficiency(fCont[np] / max_cont_ass / fGaugeIntensity[ass]);
     } else {
       fUse[np] = 0;
       p->SetUsed(0);
       p->SetNominalEnergy(0);
       p->SetNominalEnergyError(0);
       p->SetIntensity(0);
       p->SetRelEfficiency(0);
     }
     np++;
   }
   if (fVerbose) {
		fScanCanvas = new TCanvas("cscan", "cscan", 500, 500, 500, 500);
		hscan->Draw("col");
		fScanCanvas->Update();
		cout << "Compare measured intensity with published values" << endl;
		TGraph * eff = new TGraph(nassigned);
		eff->SetTitle("Effeciciency vs calibrated energy");
		Int_t na = 0;
	   printf ("  Energy  GaugInt  MeasInt   RelInt Rel/Gauge\n");
		for (Int_t i = 0; i < fNpeaks; i++) {
			Int_t ass = fAssigned[i];
			if (ass >= 0) {
				fY[i]=  fGaugeEnergy[ass];
				Double_t norm_int = fCont[i] / max_cont_ass / fGaugeIntensity[ass];
				printf ("%8.2f %8.2f %8.2f %8.2f %8.2f\n",
               fY[i], fGaugeIntensity[ass], fCont[i], fCont[i] / max_cont_ass, norm_int);

				eff->SetPoint(na, fY[i], norm_int);
				na++;
				fYE[i] = fGaugeError[ass];
				fUse[i] = 1;
			} else {
				fUse[i] = 0;
			}
		}
		fEffCanvas  = new TCanvas("effec", "Efficiency", 200, 600, 500, 200);
		fEffCanvas->Draw();
		eff->Draw("A*");
		eff->GetHistogram()->GetXaxis()->SetLabelSize(0.1);
		eff->GetHistogram()->GetYaxis()->SetLabelSize(0.1);
		fEffCanvas->Update();
   }
   if (fInteractive > 0) {
      if (fDialog)           fDialog->ReloadValues();
      if (fDialogSetNominal) fDialogSetNominal->ReloadValues();
      EnableDialogs();
   }
   SaveDefaults();
   fAutoAssigned = 1;
   return kTRUE;
}

//________________________________________________________________________

void CalibrationDialog::SetValues()
{
   if (fAutoAssigned) {
      cout << "Assignment already done" << endl;
      return;
   }
   Int_t nusp = 0, nug = 0;
   for (Int_t i = 0; i < fMaxPeaks; i++) {
      if (fUse[i] != 0)
         nusp++;
   }
   for (Int_t i = 0; i < fGaugeNpeaks; i++) {
      if (fSetFlag[i] !=0 )
         nug++;
   }
   Int_t ngauge = 0;
   for (Int_t i = 0; i < fMaxPeaks; i++) {
      FhPeak * p = (FhPeak*)fPeakList.At(i);
      if ( fUse[i] != 0 ) {
         while ( fSetFlag[ngauge] == 0 ) {
            ngauge++;
            if (ngauge >= fGaugeNpeaks) {
               cout <<setred << "SetValues(): not enough used gauge peaks "
               <<setblack << endl << "Used peaks in spectrum: " << nusp
               << " Used gauge peaks: " << nug << endl;
               return;
            }
         }
			fY[i]=  fGaugeEnergy[ngauge];
			fYE[i] = Sc_EnError[ngauge];
         p->SetNominalEnergy(fY[i]);
         p->SetNominalEnergyError(fYE[i]);
			cout << "SetValues() fX[" << i << "] = " << fX[i]
				<< "fY[" << i << "] = " << fY[i]<< endl;
			ngauge++;
      }
   }
   fDialog->ReloadValues();
}
//____________________________________________________________________________________

TF1 * CalibrationDialog::CalculateFunction()
{
   if (fCalFunc) {delete fCalFunc; fCalFunc = NULL;}
   fCalFunc = new TF1(fFuncName,(const char*)fFormula);
   if(fCalFunc->GetNpar() < 2){
//      WarnBox("Need at least pol1 (2 pars)", fParentWindow);
      Int_t retval;
      new TGMsgBox(gClient->GetRoot(), fParentWindow,"Warning","Need at least pol1 (2 pars)",kMBIconExclamation, kMBDismiss, &retval);
      delete fCalFunc; fCalFunc = NULL;
      return NULL;
   }
   TIterator * pIter = fPeakList.MakeIterator();
   FhPeak * p;
   Int_t nuse = 0, n = 0;
   while ( (p = (FhPeak *) pIter->Next()) ) {
     p->SetUsed( fUse[n]);
     p->SetNominalEnergy(fY[n]);
     p->SetNominalEnergyError(fYE[n]);
     n++;
//     cout << "n " << fUse[n] << " p->GetUsed() " << p->GetUsed()<< endl;
     if (p->GetUsed()) nuse++;
   }
   if (nuse < 2) {
//      WarnBox("Need at least 2 data points", fParentWindow);
      Int_t retval;
      new TGMsgBox(gClient->GetRoot(), fParentWindow,"Warning","Need at least 2 data points",kMBIconExclamation, kMBDismiss, &retval);
      delete fCalFunc; fCalFunc = NULL;
      return NULL;
   }

   TGraphErrors *gr = new TGraphErrors(nuse);
   pIter = fPeakList.MakeIterator();
   Int_t np = 0;
   while ( (p = (FhPeak *) pIter->Next()) ) {
     if (p->GetUsed()) {
        cout << " " << n << " " << p->GetMean() << " " << p->GetNominalEnergy() << endl;
        gr->SetPoint(np, p->GetMean(), p->GetNominalEnergy());
        gr->SetPointError(np, p->GetMeanError(), p->GetNominalEnergyError());
        np++;
     }
   }
   gr->SetMarkerStyle(4);
   gr->SetMarkerSize(2);
   if (fCalFunc->GetNpar() > 2) {
      for (Int_t i = 2; i < fCalFunc->GetNpar(); i++) {
         fCalFunc->SetParameter(i, 0);
      }
   }
   gr->Fit(fFuncName);
   cout << endl << "Fitted values a: "
        <<  fCalFunc->GetParameter(0)<< " +- " << fCalFunc->GetParError(0) << " b: "
        << fCalFunc->GetParameter(1)<< " +- " << fCalFunc->GetParError(1) << endl;
   if (fVerbose) {
		fCalValCanvas = new TCanvas("cc","cc", 200, 200, 500, 500);
		gr->Draw("AP");
	//   PrintGraph(gr);
		fCalFunc->Draw("SAME");
		fCalFunc->SetLineWidth(1);
		fCalFunc->SetLineColor(7);
		fCalValCanvas->Update();
		Double_t xl = fCalFunc->Eval(fSelHist->GetXaxis()->GetBinLowEdge(1));
		Double_t xu = fCalFunc->Eval(fSelHist->GetXaxis()
							->GetBinUpEdge(fSelHist->GetNbinsX()));
		cout << "fCalibratedNbinsX ,xl, xu " << fCalibratedNbinsX << " xl " << xl<< " xu " << xu<< endl;
   }

   return fCalFunc;
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
   static Double_t dummy = 1;
   static Double_t dummy1 = 2;
   static Double_t dummy2 = 3;
   static TString  dummy4;
   static TString gfcmd("ExecuteGetFunction()");

   row_lab->Add(new TObjString("FileContReq_File name"));
   valp[ind++] = &fFuncFromFile;
//   valp[ind++] = &dummy4;

   row_lab->Add(new TObjString("DoubleValue_dummy"));
   valp[ind++] = &dummy;
   row_lab->Add(new TObjString("DoubleValue_dummy1"));
   valp[ind++] = &dummy1;
   row_lab->Add(new TObjString("DoubleValue_dummy2"));
   valp[ind++] = &dummy2;
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
   cout << "fCalibratedNbinsX ,xl, xu " << fCalibratedNbinsX << " xl" << xl<< "xu " << xu<< endl;
//   THLimitsFinder::Optimize(xl, xu, fCalibratedNbinsX , fCalibratedXlow, fCalibratedXup,
 //                     nbins, BinWidth, "");
//   cout << "nbins , BinWidth " << nbins << " " << BinWidth << endl;
 //  fDialog->ReloadValues();
 //  fCalibratedNbinsX = nbins;
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
   Bool_t bare_function = kFALSE;
   if (fCalFunc == NULL) {
      if (fFormula.Contains("*")) {
         TString question("Should we use: ");
         question += (const char*)fFormula;
         Int_t retval;
         new TGMsgBox(gClient->GetRoot(), fParentWindow,"Question" ,question,
         kMBIconQuestion, kMBYes | kMBNo, &retval);
         if (retval == kMBYes) {
             cout << "Try to use bare formula: " << fFormula << endl;
            fCalFunc = new TF1(fFuncName,(const char*)fFormula);
            if (fCalFunc->GetNdim() == 0) {
               cout << setred << "Formula invalid" << setblack<< endl;
               delete fCalFunc;
               fCalFunc = NULL;
               return;
            }
         } else {
            return;
         }
      }
      bare_function = kTRUE;
   }
   if (fCalFunc == NULL)
      CalculateFunction();
   TString hname_cal;
   hname_cal = fSelHist->GetName();
   hname_cal += "_cal";
   TString title_cal(fSelHist->GetTitle());
   title_cal += "_calibrated";
   title_cal += ";Energy[KeV];Events[";
   title_cal += Form("%4.2f", (fCalibratedXup-fCalibratedXlow)/(Double_t)fCalibratedNbinsX);
   title_cal += " KeV]";
   cout << "title_cal: " << title_cal << endl;
   TH1* hh = (TH1*)gROOT->GetList()->FindObject(hname_cal);
   if (hh) delete hh;
   if      (!strcmp(fSelHist->ClassName(), "TH1F"))
   	fCalHist = new TH1F(hname_cal, title_cal, fCalibratedNbinsX, fCalibratedXlow, fCalibratedXup);
   else if (!strcmp(fSelHist->ClassName(), "TH1D"))
   	fCalHist = new TH1D(hname_cal, title_cal, fCalibratedNbinsX, fCalibratedXlow, fCalibratedXup);
   else if (!strcmp(fSelHist->ClassName(), "TH1S"))
   	fCalHist = new TH1S(hname_cal, title_cal, fCalibratedNbinsX, fCalibratedXlow, fCalibratedXup);
   else
   	fCalHist = new TH1C(hname_cal, title_cal, fCalibratedNbinsX, fCalibratedXlow, fCalibratedXup);

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
   if (bare_function) {
      delete fCalFunc;
      fCalFunc = NULL;
   }
}
//____________________________________________________________________________________

TList * CalibrationDialog::UpdatePeakList()
{
//   Int_t npeaks = 0
   if ( fUpdatePeakListDone > 0 )
       return &fPeakList;
   if ( fVerbose )
      cout << "UpdatePeakList: # of functions: " <<
   fSelHist->GetListOfFunctions()->GetSize() << endl;
   TIter next(fSelHist->GetListOfFunctions());
   TObject *obj;
   TF1 *f;
   TString pname;

   fPeakList.Delete();
   Double_t cont, mean, error, sigma, c_sigma, chi2ondf;
   for (Int_t k = 0; k < fMaxPeaks; k++)
      fX[k] = 0;
   fNpeaks = 0;
   while ( (obj = next()) ) {
      if (obj->IsA() == TF1::Class()) {
         f = (TF1*)obj;
         chi2ondf = f->GetChisquare() / f->GetNDF();
         c_sigma = -1;
         for (Int_t i = 0; i < f->GetNpar(); i++) {
            pname = f->GetParName(i);
            if (pname.BeginsWith("Ga_Sigma_")) {
               c_sigma= f->GetParameter(i);
            }
            if (pname.BeginsWith("Ga_Mean")) {
               mean =  f->GetParameter(i);
               error = f->GetParError(i);
//               cout << "error " << error<< endl;
               cont= 0;
               if (i > 0) cont= f->GetParameter(i-1);
               if (c_sigma >= 0) {
                  sigma = c_sigma;
               } else {
                  sigma= f->GetParameter(i+1);
               }
//               cout << "fNpeaks: " << fNpeaks<<  " Mean: " << mean  << " Error: " << error << endl;
	//          look if already stored
            	Bool_t store_it = kTRUE;
            	if (fNpeaks > 0) {
               	for (Int_t k = 0; k < fNpeaks; k++) {
                  	if (TMath::Abs((fX[k] - mean) / fX[k]) < 0.0001) store_it = kFALSE;
               	}
            	}
            	if (store_it && fNpeaks < fMaxPeaks) {
                  FhPeak *p = new FhPeak(mean);
                  p->SetMeanError(error);
                  p->SetWidth(sigma);
                  p->SetContent(cont);
                  p->SetChi2oNdf(chi2ondf);
                  fPeakList.Add(p);
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
      fPeakList.Delete();
      cout << " Not enough peaks defined: " << fNpeaks << endl;
      return NULL;
   }
//   if (fInteractive > 0) {
 //     fDialog->ReloadValues();
//      fDialogSetNominal->ReloadValues();
 //     EnableDialogs();
//   }
   fPeakList.Sort();
   TIter next1(&fPeakList);
   FhPeak *p;
   fNpeaks = 0;
   while ( (p = (FhPeak*)next1()) ) {
      if (fVerbose) {
         if (fNpeaks == 0)
            p->PrintHeadLine();
         p->Print("");
      }
	   fX [fNpeaks] = p->GetMean();
    	fXE[fNpeaks] = p->GetWidth();
	   fY [fNpeaks] = 0;
	   fYE[fNpeaks] = 1;
	   fUse[fNpeaks] = 1;
      fCont[fNpeaks] = p->GetContent();
      fNpeaks++;
   }
   if (fInteractive > 0) {
      fDialog->ReloadValues();
      cout << "UpdatePeakList:  ReloadValues" << endl;
      EnableDialogs();
   }
   if ( fVerbose )
      cout << "UpdatePeakList: # of peaks: " << fNpeaks << endl;
   fUpdatePeakListDone = 1;
   return &fPeakList;
}
//_______________________________________________________________________

Int_t CalibrationDialog::FindNumberOfPeaks()
{
	Int_t npeaks = 0;
	TIter next(fSelHist->GetListOfFunctions());
	TObject *obj;
	TF1 *f;
	TString pname;
	while ( (obj = next()) ) {
		if (obj->IsA() == TF1::Class()) {
			f = (TF1*)obj;
			for (Int_t i = 0; i < f->GetNpar(); i++) {
				pname = f->GetParName(i);
				if (pname.BeginsWith("Ga_Mean")) {
//					cout << "Mean: " << f->GetParameter(i)
//							<< " Error: " << f->GetParError(i) << endl;
						npeaks++;
				}
			}
		}
	}
   if ( fVerbose )
      cout << "Found " << npeaks << " peaks with Ga_Mean" << endl;
	return npeaks;
}
//_______________________________________________________________________

void CalibrationDialog::SetGaugePoint(Int_t N, Int_t Use, Double_t X, Double_t Y, Double_t Xerr, Double_t Yerr) {
	if ((N >= 0) && (N < fNpeaks)) {
		fX[N] = X;
		fY[N] = Y;
		fXE[N] = Xerr;
		fYE[N] = Yerr;
		fUse[N] = Use;
	}
}
//_______________________________________________________________________

void CalibrationDialog::RestoreDefaults()
{
   TEnv env(".hprrc");
   fCalibratedNbinsX  = env.GetValue("CalibrationDialog.fCalibratedNbinsX", 2000);
   fCalibratedXlow    = env.GetValue("CalibrationDialog.fCalibratedXlow", 0.);
   fCalibratedXup     = env.GetValue("CalibrationDialog.fCalibratedXup", 2000.);
   fFormula = env.GetValue("CalibrationDialog.Formula", "pol1");
   fFuncFromFile = env.GetValue("CalibrationDialog.fFuncFromFile", "workfile.root|TF1|calf");
   fMatchNbins = env.GetValue("CalibrationDialog.fMatchNbins",500);
   fMatchMin  = env.GetValue("CalibrationDialog.fMatchMin",  0   );
   fMatchMax  = env.GetValue("CalibrationDialog.fMatchMax",  2000);
   fGainMin  = env.GetValue("CalibrationDialog.fGainMin", 0.5  );
   fGainMax  = env.GetValue("CalibrationDialog.fGainMax", 1.5  );
   fGainStep = env.GetValue("CalibrationDialog.fGainStep",0.002);
   fOffMin   = env.GetValue("CalibrationDialog.fOffMin",  -100 );
   fOffMax   = env.GetValue("CalibrationDialog.fOffMax",   100 );
   fOffStep  = env.GetValue("CalibrationDialog.fOffStep",  0.5 );
   fAccept   = env.GetValue("CalibrationDialog.fAccept",     2 );
   fContThresh   = env.GetValue("CalibrationDialog.fContThresh",  0.01);
   fVerbose  = env.GetValue("CalibrationDialog.fVerbose",    0 );
//   fInteractive  = env.GetValue("CalibrationDialog.fInteractive", 1);
   fCustomGauge   = env.GetValue("CalibrationDialog.fCustomGauge",     0 );
   fEu152Gauge = env.GetValue("CalibationDialog.fEu152Gauge", 1);
   fCo60Gauge = env.GetValue("CalibationDialog.fCo60Gauge", 0);
   fY88Gauge = env.GetValue("CalibationDialog.fY88Gauge", 0);
   fCustomGaugeFile= env.GetValue("CalibrationDialog.fCustomGaugeFile", "gauge_values.txt");
}
//_______________________________________________________________________

void CalibrationDialog::SaveDefaults()
{
//   cout << "CalibrationDialog:: SaveDefaults() kEnvLocal" << endl;
   TEnv env(".hprrc");
   env.SetValue("CalibrationDialog.BinsX", fCalibratedNbinsX);
   env.SetValue("CalibrationDialog.fCalibratedXlow", fCalibratedXlow);
   env.SetValue("CalibrationDialog.fCalibratedXup", fCalibratedXup);
   env.SetValue("CalibrationDialog.Formula", fFormula.Data());
   env.SetValue("CalibrationDialog.fFuncFromFile", fFuncFromFile);
   env.SetValue("CalibrationDialog.fMatchNbins",fMatchNbins);
   env.SetValue("CalibrationDialog.fMatchMin", fMatchMin);
   env.SetValue("CalibrationDialog.fMatchMax", fMatchMax);
   env.SetValue("CalibrationDialog.fGainMin", fGainMin );
   env.SetValue("CalibrationDialog.fGainMax", fGainMax );
   env.SetValue("CalibrationDialog.fGainStep",fGainStep);
   env.SetValue("CalibrationDialog.fOffMin",  fOffMin  );
   env.SetValue("CalibrationDialog.fOffMax",  fOffMax  );
   env.SetValue("CalibrationDialog.fOffStep", fOffStep );
   env.SetValue("CalibrationDialog.fAccept",  fAccept  );
   env.SetValue("CalibrationDialog.fContThresh", fContThresh);
   env.SetValue("CalibrationDialog.fVerbose", fVerbose);
//   env.SetValue("CalibrationDialog.fInteractive", fInteractive);
   env.SetValue("CalibrationDialog.fCustomGauge", fCustomGauge);
   env.SetValue("CalibationDialog.fEu152Gauge", fEu152Gauge);
   env.SetValue("CalibationDialog.fCo60Gauge", fCo60Gauge);
   env.SetValue("CalibationDialog.fY88Gauge", fY88Gauge);
   env.SetValue("CalibrationDialog.fCustomGaugeFile", fCustomGaugeFile.Data());
   env.SaveLevel(kEnvLocal);
}
//__________________________________________________________________________

CalibrationDialog::~CalibrationDialog()
{
   gROOT->GetListOfCleanups()->Remove(this);
   if ( fCalValCanvas )
		delete fCalValCanvas;
   if ( fScanCanvas )
		delete fScanCanvas;
   if ( fEffCanvas )
		delete fEffCanvas;
   if ( fDialogSetNominal )
     fDialogSetNominal->CloseWindowExt();
   if ( fAutoSelectDialog )
      fAutoSelectDialog->CloseWindowExt();
}
//__________________________________________________________________________

void CalibrationDialog::RecursiveRemove(TObject * obj)
{
//    cout << "CalibrationDialog::RecursiveRemove " << obj << endl;
   if (obj == fDialog)
      fDialog = NULL;
   if (obj == fDialogSetNominal)
      fDialogSetNominal = NULL;
   if (obj == fAutoSelectDialog)
      fAutoSelectDialog = NULL;
   if ( obj == fCalValCanvas )
		fCalValCanvas = NULL;
   if ( obj == fScanCanvas )
		fScanCanvas = NULL;
   if ( obj == fEffCanvas )
		fEffCanvas = NULL;
   if (obj == fSelPad && fInteractive) {
//      cout << "FindPeakDialog: CloseDialog "  << endl;
      CloseDialog();
   }
}
//_______________________________________________________________________

void CalibrationDialog::CloseDialog()
{
//   cout << "CalibrationDialog::CloseDialog() " << endl;
   gROOT->GetListOfCleanups()->Remove(this);
   if ( fDialog ) fDialog->CloseWindowExt();
   delete this;

}
//_______________________________________________________________________

void CalibrationDialog::CloseDown(Int_t wid)
{
//   cout << "CalibrationDialog::CloseDown() " << endl;
   if ( fDialogSetNominal ) {
//       cout << "CalibrationDialog::fDialogSetNominal->CloseWindowExt()" << endl;
      fDialogSetNominal->CloseWindowExt();
      fDialogSetNominal = NULL;
   }
   if ( fAutoSelectDialog ) {
      fAutoSelectDialog->CloseWindowExt();
      fAutoSelectDialog = NULL;
   }
   SaveDefaults();
   delete this;
}
//_______________________________________________________________________

void CalibrationDialog::DisableDialogs()
{
	if (fDialog) fDialog->DisableCancelButton();
	if (fDialogSetNominal) fDialogSetNominal->DisableCancelButton();
	if (fAutoSelectDialog) fAutoSelectDialog->DisableCancelButton();
}
//_______________________________________________________________________

void CalibrationDialog::EnableDialogs()
{
	if (fDialog) fDialog->EnableCancelButton();
	if (fDialogSetNominal) fDialogSetNominal->EnableCancelButton();
	if (fAutoSelectDialog) fAutoSelectDialog->EnableCancelButton();
}
