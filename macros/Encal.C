//________________________________________________________________[ROOT MACRO]
//////////////////////////////////////////////////////////////////////////////
// Name:             Encal.C
// Purpose:          Energy calibration for 1-dim histograms
// Syntax:           .x Encal.C(TObjArray * Histos,
//                               Int_t CalSource,
//                               const Char_t * Energies,
//                               const Char_t * CalFile,
//                               const Char_t * ResFile,
//                               const Char_t * FitFile,
//                               Bool_t ClearFlag,
//                               const Char_t * PrecalFile,
//                               Int_t LowerLim,
//                               Int_t UpperLim,
//                               Int_t PeakFrac,
//                               Double_t Sigma,
//                               Int_t FitMode,
//                               Int_t FitGrouping,
//                               Double_t FitRange,
//                               Int_t DisplayFlag,
//                               Bool_t VerboseMode)
// Arguments:        TObjArray * Histos        -- [0] Histogram file (.root),  histo names
//                   Int_t CalSource           -- Calibration source
//                   Char_t * Energies         -- Calibration energies
//                   Char_t * CalFile          -- Calibration data (.cal)
//                   Char_t * ResFile          -- Calibration results (.res)
//                   Char_t * FitFile          -- Fit results (.root)
//                   Bool_t ClearFlag          -- Clear calibration files (.cal, .res)
//                   Char_t * PrecalFile       -- Precalibration file (.cal, needed if Eu152)
//                   Int_t LowerLim            -- Lower limit [chn]
//                   Int_t UpperLim            -- Upper limit [chn]
//                   Int_t PeakFrac            -- Threshold for peak finder [% of max peak]
//                   Double_t Sigma            -- Sigma
//                   Int_t FitMode             -- Fit mode
//                   Int_t FitGrouping         -- Fit grouping
//                   Double_t FitRange         -- Range for single peak fit [sigma]
//                   Int_t DisplayFlag         -- Display results
//                   Bool_t VerboseMode        -- Verbose mode
// Description:      Energy calibration for 1-dim histograms
// Author:           Rudolf.Lutter
// Mail:             Rudolf.Lutter@lmu.de
// URL:              www.bl.physik.uni-muenchen.de/~Rudolf.Lutter
// Revision:         $Id: Encal.C,v 1.12 2007-02-07 13:04:20 Rudolf.Lutter Exp $
// Date:             Tue Feb  6 09:08:52 2007
//+Exec __________________________________________________[ROOT MACRO BROWSER]
//                   Name:                Encal.C
//                   Title:               Energy calibration for 1-dim histograms
//                   Width:               
//                   Aclic:               +g
//                   NofArgs:             17
//                   Arg1.Name:           HistoFile
//                   Arg1.Title:          Histogram file (.root)
//                   Arg1.Type:           Char_t *
//                   Arg1.EntryType:      FObjListBox
//                   Arg1.Default:        none.root
//                   Arg1.Values:         ROOT files:*.root
//                   Arg1.AddLofValues:   No
//                   Arg1.Base:           dec
//                   Arg1.Orientation:    horizontal
//                   Arg2.Name:           CalSource
//                   Arg2.Title:          Calibration source
//                   Arg2.Type:           Int_t
//                   Arg2.EntryType:      Radio
//                   Arg2.Default:        1
//                   Arg2.Values:         Co60|calibrate using Co60 source=1:Eu152|calibrate using Eu152 source (Co60 precalibration needed)=2:TripleAlpha|calibrate using alpha source (Pu239/Am241/Cm244)=4
//                   Arg2.AddLofValues:   No
//                   Arg2.Base:           dec
//                   Arg2.Orientation:    horizontal
//                   Arg3.Name:           Energies
//                   Arg3.Title:          Calibration energies
//                   Arg3.Type:           Char_t *
//                   Arg3.EntryType:      File
//                   Arg3.Default:        $MARABOU/data/encal/energies.dat
//                   Arg3.AddLofValues:   No
//                   Arg3.Base:           dec
//                   Arg3.Orientation:    horizontal
//                   Arg4.Name:           CalFile
//                   Arg4.Title:          Calibration data (.cal)
//                   Arg4.Type:           Char_t *
//                   Arg4.EntryType:      File
//                   Arg4.Width:          150
//                   Arg4.Default:        Encal.cal
//                   Arg4.Values:         Calib files:*.cal
//                   Arg4.AddLofValues:   No
//                   Arg4.Base:           dec
//                   Arg4.Orientation:    horizontal
//                   Arg5.Name:           ResFile
//                   Arg5.Title:          Calibration results (.res)
//                   Arg5.Type:           Char_t *
//                   Arg5.EntryType:      File
//                   Arg5.Width:          150
//                   Arg5.Default:        Encal.res
//                   Arg5.Values:         Result files:*.res
//                   Arg5.AddLofValues:   No
//                   Arg5.Base:           dec
//                   Arg5.Orientation:    horizontal
//                   Arg6.Name:           FitFile
//                   Arg6.Title:          Fit results (.root)
//                   Arg6.Type:           Char_t *
//                   Arg6.EntryType:      File
//                   Arg6.Width:          150
//                   Arg6.Default:        Encal.root
//                   Arg6.Values:         Fit data files:*.root
//                   Arg6.AddLofValues:   No
//                   Arg6.Base:           dec
//                   Arg6.Orientation:    horizontal
//                   Arg7.Name:           ClearFlag
//                   Arg7.Title:          Clear output files (.cal, .res)
//                   Arg7.Type:           Bool_t
//                   Arg7.EntryType:      YesNo
//                   Arg7.AddLofValues:   No
//                   Arg7.Base:           dec
//                   Arg7.Orientation:    horizontal
//                   Arg8.Name:           PrecalFile
//                   Arg8.Title:          Precalibration file (.cal, needed if Eu152)
//                   Arg8.Type:           Char_t *
//                   Arg8.EntryType:      File
//                   Arg8.Width:          150
//                   Arg8.Default:        Co60.cal
//                   Arg8.Values:         Calib results:*.cal
//                   Arg8.AddLofValues:   No
//                   Arg8.Base:           dec
//                   Arg8.Orientation:    horizontal
//                   Arg9.Name:           LowerLim
//                   Arg9.Title:          Lower limit [chn]
//                   Arg9.Type:           Int_t
//                   Arg9.EntryType:      UpDown-X
//                   Arg9.Default:        0
//                   Arg9.AddLofValues:   No
//                   Arg9.LowerLimit:     0
//                   Arg9.UpperLimit:     4096
//                   Arg9.Increment:      100
//                   Arg9.Base:           dec
//                   Arg9.Orientation:    horizontal
//                   Arg10.Name:          UpperLim
//                   Arg10.Title:         Upper limit [chn]
//                   Arg10.Type:          Int_t
//                   Arg10.EntryType:     UpDown-X
//                   Arg10.Default:       4096
//                   Arg10.AddLofValues:  No
//                   Arg10.LowerLimit:    0
//                   Arg10.UpperLimit:    4096
//                   Arg10.Increment:     100
//                   Arg10.Base:          dec
//                   Arg10.Orientation:   horizontal
//                   Arg11.Name:          PeakFrac
//                   Arg11.Title:         Threshold for peak finder [% of max peak]
//                   Arg11.Type:          Int_t
//                   Arg11.EntryType:     UpDown-X
//                   Arg11.Default:       1
//                   Arg11.AddLofValues:  No
//                   Arg11.LowerLimit:    1
//                   Arg11.UpperLimit:    50
//                   Arg11.Increment:     1
//                   Arg11.Base:          dec
//                   Arg11.Orientation:   horizontal
//                   Arg12.Name:          Sigma
//                   Arg12.Title:         Sigma
//                   Arg12.Type:          Double_t
//                   Arg12.EntryType:     UpDown
//                   Arg12.Default:       3
//                   Arg12.AddLofValues:  No
//                   Arg12.LowerLimit:    0
//                   Arg12.UpperLimit:    10
//                   Arg12.Increment:     .2
//                   Arg12.Base:          dec
//                   Arg12.Orientation:   horizontal
//                   Arg13.Name:          FitMode
//                   Arg13.Title:         Fit mode
//                   Arg13.Type:          Int_t
//                   Arg13.EntryType:     Radio
//                   Arg13.Default:       1
//                   Arg13.Values:        gauss|fit gaussian distribution=1:gauss+tail|fit gaussian + exp tail on left side=2
//                   Arg13.AddLofValues:  No
//                   Arg13.Base:          dec
//                   Arg13.Orientation:   horizontal
//                   Arg14.Name:          FitGrouping
//                   Arg14.Title:         Fit grouping
//                   Arg14.Type:          Int_t
//                   Arg14.EntryType:     Radio
//                   Arg14.Default:       1
//                   Arg14.Values:        single peak|fit each peak separately=1:ensemble|group peaks before fitting=2
//                   Arg14.AddLofValues:  No
//                   Arg14.Base:          dec
//                   Arg14.Orientation:   horizontal
//                   Arg15.Name:          FitRange
//                   Arg15.Title:         Range for peak fit [sigma]
//                   Arg15.Type:          Double_t
//                   Arg15.EntryType:     UpDown
//                   Arg15.Default:       3
//                   Arg15.AddLofValues:  No
//                   Arg15.LowerLimit:    0
//                   Arg15.UpperLimit:    10
//                   Arg15.Increment:     .5
//                   Arg15.Base:          dec
//                   Arg15.Orientation:   horizontal
//                   Arg16.Name:          DisplayFlag
//                   Arg16.Title:         Display results
//                   Arg16.Type:          Int_t
//                   Arg16.EntryType:     Check
//                   Arg16.Default:       1
//                   Arg16.Values:        step|show each fit=1:2dim|show 2-dim histo after calibration=2
//                   Arg16.AddLofValues:  No
//                   Arg16.Base:          dec
//                   Arg16.Orientation:   horizontal
//                   Arg17.Name:          VerboseMode
//                   Arg17.Title:         Verbose mode
//                   Arg17.Type:          Bool_t
//                   Arg17.EntryType:     YesNo
//                   Arg17.Default:       1
//                   Arg17.AddLofValues:  No
//                   Arg17.Base:          dec
//                   Arg17.Orientation:   horizontal
//-Exec
//////////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <iomanip>
#include <fstream>
#include "TObjArray.h"
#include "TObjString.h"
#include "TCanvas.h"
#include "TVirtualX.h"
#include "TButton.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TRandom.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TEnv.h"
#include "TGFrame.h"
#include "TSystem.h"
#include "TSpectrum.h"
#include "TPolyMarker.h"
#include "TMrbLogger.h"
#include "SetColor.h"


enum	{ kCalSourceCo60 = 1 };
enum	{ kCalSourceEu152 = 2 };
enum	{ kCalSourceTripleAlpha = 4 };
enum	{ kFitModeGaus = 1 };
enum	{ kFitModeGausTail = 2 };
enum	{ kFitGroupingSinglePeak = 1 };
enum	{ kFitGroupingEnsemble = 2 };
enum	{ kDisplayFlagStep = 1 };
enum	{ kDisplayFlag2dim = 2 };

enum	{	kButtonOk = 1,
			kButtonDiscard,
			kButtonStop,
			kButtonQuit
		};

enum	{	kColorWhite = 10,
			kColorRed = 2,
			kColorGreen = 3,
			kColorBlue = 4,
			kColorGray = 32
		};

// global vars

TMrbLogger * msg = NULL;

// fitting
Int_t fitNofPeaks = 1;
Int_t fitBinWidth = 1;
Int_t fitTailSide = 1;

// calibration
TString calSource;
TEnv * enTable = NULL;
TEnv * cal = NULL;
TEnv * res = NULL;
TFile * resFit = NULL;

Bool_t isTripleAlpha = kFALSE;
Bool_t isCo60 = kFALSE;
Bool_t isEu152 = kFALSE;
Int_t peaksNeeded = 0;
Bool_t doCal = kFALSE;
TString enFile;

// canvas & display
Int_t buttonFlag = 0;
Bool_t stepIt = kFALSE;
Bool_t show2dim = kFALSE;
TCanvas * canv = NULL;
TButton * statusLine;

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
  ...
*/
   static Float_t sqrt2pi = sqrt(2 * TMath::Pi()), sqrt2 = sqrt(2.);
   Double_t arg;
   Double_t fitval  = 0;
   Double_t bgconst = par[0];
   Double_t bgslope = par[1];
   Double_t sigma   = par[2];
   if (sigma == 0) sigma = 1;               //  force widths /= 0
   fitval = fitval + bgconst + x[0] * bgslope;
   for (Int_t i = 0; i < fitNofPeaks; i ++) {
      arg = (x[0] - par[4 + 2 * i]) / (sqrt2 * sigma);
      fitval = fitval + par[3 + 2 * i] * exp(-arg * arg) / (sqrt2pi * sigma);
   }
   return fitBinWidth * fitval;
}

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
  ...
*/
   static Double_t sqrt2pi = sqrt(2 * TMath::Pi()), sqrt2 = sqrt(2.);
   Double_t xij;
   Double_t arg;
   Double_t tail;
   Double_t g2b;
   Double_t err;
   Double_t norm;
   Double_t fitval = 0;
   Double_t tailfrac  = par[0];
   Double_t tailwidth = par[1];
   Double_t bgconst   = par[2];
   Double_t bgslope   = par[3];
   Double_t sigma = par[4];

//  force widths /= 0
   if (tailwidth == 0)
      tailwidth = 1;
   if (sigma == 0)
      sigma = 1;

   fitval = fitval + bgconst + x[0] * bgslope;
   for (Int_t i = 0; i < fitNofPeaks; i++) {
      xij = (x[0] - par[6 + 2 * i]);
      arg = xij / (sqrt2 * sigma);
      xij *= fitTailSide;
      tail = exp(xij / tailwidth) / tailwidth;
      g2b = 0.5 * sigma / tailwidth;
      err = 0.5 * tailfrac * par[5 + 2 * i] * TMath::Erfc(xij / sigma + g2b);
      norm = exp(0.25 * pow(sigma, 2) / pow(tailwidth, 2));
      fitval = fitval + norm * err * tail 
                      + par[5 + 2 * i] * exp(-arg * arg) / (sqrt2pi * sigma);
   }
   return fitBinWidth * fitval;
}

void WaitForButtonPressed() {
	buttonFlag = 0;
	if (!stepIt) return;
	while (buttonFlag == 0) {
		gSystem->Sleep(50);
		gSystem->ProcessEvents();
	}
}

void ClearCanvas(Int_t PadNo) {
	switch (PadNo) {
		case 0:
		case 1:
			canv->cd(1);
			gPad->Clear();
			if (PadNo == 1) break;
		case 2:
			canv->cd(2);
			gPad->Clear();
			break;
	}
	canv->cd(PadNo);
	canv->Update();
}

void SetStatusLine(const Char_t * Text = "", Int_t FillColor = 10, Int_t LineColor = 1) {
		statusLine->SetTitle(Text);
		statusLine->SetFillColor(FillColor);
		statusLine->SetTextColor(LineColor);
		statusLine->Paint();
		statusLine->Modified();
		statusLine->Update();
}

void OutputMessage(const Char_t * Text, Bool_t ErrFlag = kFALSE, Int_t ColorIndex = -1) {
	const Char_t * tCol = setblack;
	Int_t fgCol = 1;
	switch (ColorIndex) {
		case kColorRed: tCol = setred; fgCol = 1; break;
		case kColorGreen: tCol = setgreen; fgCol = 1; break;
		case kColorBlue: tCol = setblue; fgCol = 10; break;
	}
	if (ColorIndex > 0) SetStatusLine(Text, ColorIndex, fgCol);
	if (ErrFlag) {
		msg->Err() << Text << endl;
		msg->Flush("Encal.C");
	} else {
		msg->Out() << Text << endl;
		msg->Flush("Encal.C", "", tCol);
	}
}

TCanvas * DrawCanvas() {
	Int_t nBtns = 4;
	Int_t canvw = 800;
	Int_t canvh = 600;
	Double_t xpx = 1./canvw;
	Double_t x0 = 100 * xpx;
	Double_t wdth = (canvw - 200) * xpx / nBtns;
	Double_t y0 = 0.03;
	Double_t y1 = 0.08;
	Double_t y2 = 0.081;
	Double_t y3 = 0.132;

	TCanvas * c = new TCanvas("EncalCanv", "Energy Calibration Tool", canvw, canvh);
	c->Divide(1,2);

	Int_t btnNo = 0;
	TButton * btnOk = new TButton("accept", "buttonFlag = kButtonOk;", x0 + btnNo * wdth, y0, x0 + (btnNo + 1) * wdth, y1);
	btnOk->SetFillColor(32);
	btnOk->SetToolTipText("pass data to calibration file and continue with next histo");
	btnOk->Draw();
	btnNo++;
	TButton * btnDiscard = new TButton("discard", "buttonFlag = kButtonDiscard;", x0 + btnNo * wdth, y0, x0 + (btnNo + 1) * wdth, y1);
	btnDiscard->SetFillColor(32);
	btnDiscard->SetToolTipText("discard fit and continue with next histo");
	btnDiscard->Draw();
	btnNo++;
	TButton * btnStop = new TButton("stop", "if (resFit) resFit->Close(); buttonFlag = kButtonStop;", x0 + btnNo * wdth, y0, x0 + (btnNo + 1) * wdth, y1);
	btnStop->SetFillColor(32);
	btnStop->SetToolTipText("leave calibration and return to MacroBrowser");
	btnStop->Draw();
	btnNo++;
	TButton * btnQuit = new TButton("quit", "if (resFit) resFit->Close(); gSystem->Exit(0);", x0 + btnNo * wdth, y0, x0 + (btnNo + 1) * wdth, y1);
	btnQuit->SetFillColor(32);
	btnQuit->SetToolTipText("exit from ROOT");
	btnQuit->Draw();
	statusLine = new TButton(" ", " ", .03, y2, .97, y3);
	statusLine->Draw();

	c->cd(2);
	gPad->SetBottomMargin(.3);

	c->cd();
	c->Update();

	return c;
}

Bool_t CheckCalSource(Int_t CalSource, const Char_t * Energies) {
	peaksNeeded = 0;
	enFile = Energies;
	gSystem->ExpandPathName(enFile);
	if (gSystem->AccessPathName(enFile.Data())) {
		OutputMessage(Form("No such file - %s, can't read calibration energies", enFile.Data()), kTRUE);
		return(kFALSE);
	}
	enTable = new TEnv(enFile.Data());

	switch (CalSource) {
		case kCalSourceTripleAlpha:
			calSource = "TripleAlpha";
			isTripleAlpha = kTRUE;
			break;
		case kCalSourceCo60:
			calSource = "Co60";
			isCo60 = kTRUE;
			break;
		case kCalSourceEu152:
			calSource = "Eu152";
			isEu152 = kTRUE;
			break;
		default:
			OutputMessage(Form("Calibration source not yet implemented - %s, performing peak fitting only", calSource.Data()), kTRUE);
			return(kFALSE);
	}

	TString calNames = enTable->GetValue("Calib.SourceNames", "");
	if (calNames.IsNull()) {
		OutputMessage(Form("File %s doesn't contain calibration energies for source \"%s\"", enFile.Data(), calSource.Data()), kTRUE);
		delete enTable;
		enTable = NULL;
		return(kFALSE);
	}	
		
	peaksNeeded = enTable->GetValue(Form("Calib.%s.NofLines", calSource.Data()), 0);
	if (peaksNeeded == 0) {
		OutputMessage(Form("Error reading \"%s\" from file %s - can't calibrate using %s source", Form("Calib.%s.NofLines",calSource.Data()), enFile.Data(), calSource.Data()), kTRUE);
		delete enTable;
		enTable = NULL;
		return(kFALSE);
	}	
	return(kTRUE);
}

void CloseResultFiles() {
	TList * lofEntries = (TList *) res->GetTable();
	TIterator * iter = lofEntries->MakeIterator();
	TEnvRec * r;
	Int_t nh = 0;
	while (r = (TEnvRec *) iter->Next()) {
		TString e = r->GetName();
		if (e.Contains(".Xmin")) nh++;
	}
	res->SetValue("Calib.NofHistograms", nh);
	res->SaveLevel(kEnvLocal);

	lofEntries = (TList *) cal->GetTable();
	iter = lofEntries->MakeIterator();
	nh = 0;
	while (r = (TEnvRec *) iter->Next()) {
		TString e = r->GetName();
		if (e.Contains(".Xmin")) nh++;
	}
	cal->SetValue("Calib.NofHistograms", nh);
	cal->SaveLevel(kEnvLocal);
}

void ShowResults2dim(TFile * HistoFile, Int_t LowerLim, Int_t UpperLim, Double_t Emin, Double_t Emax) {
	Int_t nofHistos = cal->GetValue("Calib.NofHistograms", 0);
	if (nofHistos == 0) return;

	TString hTitle = Form("Calibration restults: file %s, %d histogram(s)", HistoFile->GetName(), nofHistos);
 	TCanvas * c = new TCanvas();
	TH2S * hCal = new TH2S("hCalResults", hTitle.Data(), (UpperLim - LowerLim), Emin, Emax, nofHistos, 0, nofHistos);
	if (resFit) resFit->Append(hCal);
	TAxis * yAxis = hCal->GetYaxis();
	TList * lofEntries = (TList *) cal->GetTable();
	TIterator * iter = lofEntries->MakeIterator();
	TEnvRec * r;
	Int_t nh = 0;
	TString hName = "";
	while (r = (TEnvRec *) iter->Next()) {
		TString str = r->GetName();
		TObjArray * a = str.Tokenize(".");
		TString hn = ((TObjString *) a->At(1))->GetString();
		if (hName.CompareTo(hn.Data()) != 0) {
			hName = hn;
			Double_t a0 = cal->GetValue(Form("Calib.%s.Offset", hn.Data()), 0.0);
			Double_t a1 = cal->GetValue(Form("Calib.%s.Gain", hn.Data()), 1.0);
			TH1F * h = (TH1F *) HistoFile->Get(hn.Data());
			if (h != NULL) {
				for (Int_t i = LowerLim; i < UpperLim; i++) {
					Int_t bCont = (Int_t) h->GetBinContent(i);
					Double_t bCent = h->GetBinCenter(i);
					for (Int_t j = 0; j < bCont; j++) {
						Double_t x = bCent + gRandom->Rndm() - 0.5;
						x = a0 + a1 * x;
						hCal->Fill((Axis_t) x, (Axis_t) nh);
					}
				}
				nh++;
				yAxis->SetBinLabel(nh, hn.Data());
			}
		}
		delete a;
	}
	hCal->Draw();
	c->Update();
}

void Encal(TObjArray * Histos = NULL,
           Int_t CalSource = kCalSourceCo60,
           const Char_t * Energies = "$MARABOU/data/encal/energies.dat",
           const Char_t * CalFile = "Encal.cal",
           const Char_t * ResFile = "Encal.res",
           const Char_t * FitFile = "Encal.root",
           Bool_t ClearFlag = kFALSE,
           const Char_t * PrecalFile = "Co60.cal",
           Int_t LowerLim = 0,
           Int_t UpperLim = 4096,
           Int_t PeakFrac = 1,
           Double_t Sigma = 3,
           Int_t FitMode = kFitModeGaus,
           Int_t FitGrouping = kFitGroupingSinglePeak,
           Double_t FitRange = 3,
           Int_t DisplayFlag = kDisplayFlagStep,
           Bool_t VerboseMode = kFALSE)
//>>_________________________________________________(do not change this line)
//
{
	msg = new TMrbLogger("Encal.log");

	show2dim = ((DisplayFlag & kDisplayFlag2dim) != 0);
	stepIt = ((DisplayFlag & kDisplayFlagStep) != 0);

	Int_t nofHistos = Histos->GetEntriesFast();
	if (nofHistos == 0) {
		OutputMessage("No histogram file given", kTRUE);
		return;
	}

	TIterator * hIter = Histos->MakeIterator();
	TObjString * hStr;
	hStr = (TObjString *) hIter->Next();

	TString hFileName;
	hFileName.Resize(0);
	if (hStr) {
		hFileName = hStr->GetString();
		hFileName = hFileName.Strip(TString::kBoth);
	}
	if (hFileName.IsNull()) {
		OutputMessage("No histogram file given", kTRUE);
		return;
	}

	TFile * hFile = new TFile(hFileName.Data());
	if (!hFile->IsOpen()) {
		OutputMessage(Form("Can't open histogram file - %s", hFileName.Data()), kTRUE);
		return;
	}

	if (nofHistos == 1) {
		OutputMessage("No histogram(s) selected", kTRUE);
		return;
	}

	doCal = CheckCalSource(CalSource, Energies);

	if (ClearFlag) {
		TString rmCmd = Form("rm -f %s %s", CalFile, ResFile);
		gSystem->Exec(rmCmd.Data());
		if (VerboseMode) OutputMessage(Form("Removing existing files - %s %s", CalFile, ResFile));
	}

	res = new TEnv(ResFile);
	if (VerboseMode) OutputMessage(Form("Writing results to file %s", ResFile));

	res->SetValue("Calib.ROOTFile", hFileName.Data());
	res->SetValue("Calib.Energies", enFile.Data());

	cal = new TEnv(CalFile);
	if (VerboseMode) OutputMessage(Form("Writing calibration data to file %s", CalFile));

	cal->SetValue("Calib.ROOTFile", hFileName.Data());
	
	cal->SetValue("Calib.Source", calSource.Data());
	cal->SetValue("Calib.Energies", enFile.Data());

	TFile * resFit = new TFile(FitFile, "RECREATE");
	if (!resFit->IsOpen()) {
		OutputMessage(Form("Can't open file - %s", FitFile), kTRUE);
		resFit = NULL;
	} else if (VerboseMode) {
		OutputMessage(Form("Writing fit data to file %s", FitFile));
	}

	canv = DrawCanvas();

	Double_t eMin = res->GetValue("Calib.Emin", 1e+20);
	Double_t eMax = res->GetValue("Calib.Emax", 0);
	Int_t xMin = res->GetValue("Calib.Xmin", 100000);
	Int_t xMax = res->GetValue("Calib.Xmax", 0);

	Int_t nofHistosCalibrated = 0;
	while (hStr = (TObjString *) hIter->Next()) {

		TString hist = hStr->GetString();

		canv->cd(1);
		TH1F * h = (TH1F *) hFile->Get(hist.Data());
		if (h == NULL) {
			ClearCanvas(0);
			OutputMessage(Form("No such histogram - %s:%s", hFileName.Data(), hist.Data()), kTRUE);
			WaitForButtonPressed();
			if (buttonFlag == kButtonStop) break; else continue;
		}

		if (UpperLim == 0 || UpperLim < LowerLim) UpperLim = h->GetNbinsX();
		if (LowerLim != 0 || UpperLim != h->GetNbinsX()) h->GetXaxis()->SetRange(LowerLim, UpperLim);

		TSpectrum * s = new TSpectrum(100);

		Double_t pmax = (Double_t) PeakFrac / 100.;
		Int_t nPeaks = s->Search(h, 5, "", pmax);

		TArrayF px, py, fx, fy, fxe, fye, chi2;
		TF1 * pol1 = NULL;
		TString fitOk;

		if (nPeaks > 0) {
	
			px.Set(nPeaks, s->GetPositionX());
			py.Set(nPeaks, s->GetPositionY());

			TArrayF ps;
			TArrayI ind(nPeaks);
			ps.Set(nPeaks, px.GetArray());
			TMath::Sort(nPeaks, ps.GetArray(), ind.GetArray(), kFALSE);
			for (Int_t i = 0; i < nPeaks; i++) {
				Int_t k = ind[i];
				px[i] = ps[k];
			}
			ps.Set(nPeaks, py.GetArray());
			for (Int_t i = 0; i < nPeaks; i++) {
				Int_t k = ind[i];
				py[i] = ps[k];
			}

			cout << endl << "Peaks:" << endl;
			for (Int_t i = 0; i < nPeaks; i++) cout << i << " X=" << px[i] << " Y=" << py[i] << endl;

    		TList * lf = h->GetListOfFunctions();                        
    		TPolyMarker * pm = (TPolyMarker *) lf->FindObject("TPolyMarker");
			pm->SetMarkerColor(kBlue);

			fx.Set(nPeaks);
			fy.Set(nPeaks);
			fxe.Set(nPeaks);
			fye.Set(nPeaks);
			chi2.Set(nPeaks);

			TF1 * fit;
			if (FitMode == kFitModeGausTail) {
				if (FitGrouping == kFitGroupingSinglePeak) {
					fitNofPeaks = 1;
					Int_t nPar = 5 + 2 * fitNofPeaks;
					for (Int_t i = 0; i < nPeaks; i++) {
						TArrayD p(nPar);
						// tail
						p[0] = 1;
						p[1] = Sigma;
						// linear background
						p[2] = h->GetBinContent(h->FindBin(px[i] - FitRange * Sigma));
						p[3] = 0.;
						// gauss
						p[4] = Sigma;
						p[5] = py[i];
						p[6] = px[i];

						fit = new TF1(Form("g%d", i), gaus_tail, px[i] - FitRange * Sigma, px[i] + FitRange * Sigma, nPar);
						fit->SetParameters(p.GetArray());
						cout << "Gaussian/tail params for peak #" << i << ":" << endl;
						for (Int_t k = 0; k < 7; k++) cout << "    p" << k << "=" << p[k] << endl;
						fit->SetLineColor(2);
						h->Fit(fit, (i == 0) ? "R" : "R+");
						fit->GetParameters(p.GetArray());
						fx[i] = p[6];
						fy[i] = p[5];
						fxe[i] = fit->GetParError(6);
						fye[i] = fit->GetParError(5);
						Int_t ndf = TMath::Max(1, fit->GetNDF());
						chi2[i] = fit->GetChisquare() / ndf;
					}
					if (resFit) resFit->Append(h);
				} else if (FitGrouping == kFitGroupingEnsemble){
					fitNofPeaks = nPeaks;
					Int_t nPar = 5 + 2 * fitNofPeaks;
					TArrayD p(nPar);
					// tail
					p[0] = 1;
					p[1] = Sigma;
					// linear background
					p[2] = h->GetBinContent(h->FindBin(px[0] - FitRange * Sigma));
					p[3] = 0.;
					// gauss
					p[4] = Sigma;
					Int_t k = 5;
					for (Int_t i = 0; i < nPeaks; i++, k+= 2) {
						p[k] = py[i];
						p[k + 1] = px[i];
					}

					fit = new TF1("g_group", gaus_tail, px[0] - FitRange * Sigma, px[nPeaks - 1] + FitRange * Sigma, nPar);
					fit->SetParameters(p.GetArray());
					cout << "Gaussian/tail params for group of " << nPeaks << " peaks:" << endl;
					for (Int_t k = 0; k < nPar; k++) cout << "    p" << k << "=" << p[k] << endl;
					fit->SetLineColor(2);
					h->Fit(fit, "R");
					if (resFit) resFit->Append(h);
					fit->GetParameters(p.GetArray());
					Int_t ndf = TMath::Max(1, fit->GetNDF());
					k = 5;
					for (Int_t i = 0; i < nPeaks; i++, k+= 2) {
						fy[i] = p[k];
						fye[i] = fit->GetParError(k);
						fx[i] = p[k + 1];
						fxe[i] = fit->GetParError(k + 1);
						chi2[i] = fit->GetChisquare() / ndf;
					}
				}
			} else if (FitMode == kFitModeGaus) {
				if (FitGrouping == kFitGroupingSinglePeak) {
					fitNofPeaks = 1;
					Int_t nPar = 3 + 2 * fitNofPeaks;
					for (Int_t i = 0; i < nPeaks; i++) {
						TArrayD p(nPar);
						// linear background
						p[0] = h->GetBinContent(h->FindBin(px[i] - FitRange * Sigma));
						p[1] = 0.;
						// gauss
						p[2] = Sigma;
						p[3] = py[i];
						p[4] = px[i];

						fit = new TF1(Form("g%d", i), gaus_lbg, px[i] - FitRange * Sigma, px[i] + FitRange * Sigma, nPar);
						fit->SetParameters(p.GetArray());
						cout << "Gaussian params for peak #" << i << ":" << endl;
						for (Int_t k = 0; k < 5; k++) cout << "    p" << k << "=" << p[k] << endl;
						fit->SetLineColor(2);
						h->Fit(fit, (i == 0) ? "R" : "R+");
						fit->GetParameters(p.GetArray());
						fy[i] = p[3];
						fye[i] = fit->GetParError(3);
						fx[i] = p[4];
						fxe[i] = fit->GetParError(4);
						Int_t ndf = TMath::Max(1, fit->GetNDF());
						chi2[i] = fit->GetChisquare() / ndf;
					}
					if (resFit) resFit->Append(h);
				} else if (FitGrouping == kFitGroupingEnsemble){
					fitNofPeaks = nPeaks;
					Int_t nPar = 3 + 2 * fitNofPeaks;
					TArrayD p(nPar);
					// linear background
					p[0] = h->GetBinContent(h->FindBin(px[0] - FitRange * Sigma));
					p[1] = 0.;
					// gauss
					p[2] = Sigma;
					Int_t k = 3;
					for (Int_t i = 0; i < nPeaks; i++, k += 2) {
						p[k] = py[i];
						p[k + 1] = px[i];
					}

					fit = new TF1("g_group", gaus_lbg, px[0] - FitRange * Sigma, px[nPeaks - 1] + FitRange * Sigma, nPar);
					fit->SetParameters(p.GetArray());
					cout << "Gaussian params for group of " << nPeaks << " peaks:" << endl;
					for (Int_t k = 0; k < nPar; k++) cout << "    p" << k << "=" << p[k] << endl;
					fit->SetLineColor(2);
					h->Fit(fit, "R");
					if (resFit) resFit->Append(h);
					fit->GetParameters(p.GetArray());
					Int_t ndf = TMath::Max(1, fit->GetNDF());
					k = 3;
					for (Int_t i = 0; i < nPeaks; i++, k += 2) {
						fy[i] = p[k];
						fye[i] = fit->GetParError(k);
						fx[i] = p[k + 1];
						fxe[i] = fit->GetParError(k + 1);
						chi2[i] = fit->GetChisquare() / ndf;
					}
				}
			}

			cout << endl << "=========================================================================" << endl;
			cout << "File : " << hFileName << endl;
			cout << "Histo: " << hist << endl;
			cout << "Peaks:  #    X         X(fit)       Chi2" << endl;
			cout << "------------------------------------------------------------------------" << endl;
			for (Int_t i = 0; i < nPeaks; i++) {
				cout << Form("%9d   %7.2f   %7.2f       %7.5f", i, px[i], fx[i], chi2[i]) << endl;
			}
			cout << "=========================================================================" << endl;

			ClearCanvas(2);

			if (doCal) {
				TArrayF calX(peaksNeeded);
				TArrayF calE(peaksNeeded);
				TArrayF calXerr(peaksNeeded);
				TArrayF calEerr(peaksNeeded);
				if (nPeaks >= peaksNeeded) { 
					Int_t k = nPeaks - 1;
					for (Int_t i = peaksNeeded - 1; i >= 0; i--, k--) {
						calX[i] = fx[k];
						calXerr[i] = fxe[k];
					}

 					for (Int_t i = 0; i < peaksNeeded; i++) {
						calE[i] = enTable->GetValue(Form("Calib.%s.Line.%d.E", calSource.Data(), i), 0.0);
						calEerr[i] = enTable->GetValue(Form("Calib.%s.Line.%d.Eerr", calSource.Data(), i), 0.0);
					}

					TGraphErrors * calib = new TGraphErrors(peaksNeeded, calX.GetArray(), calE.GetArray(), calXerr.GetArray(), calEerr.GetArray());
					if (resFit) resFit->Append(calib);
					calib->SetName(Form("calib_%s", hist.Data()));
					calib->SetTitle(Form("%s Calibration for histo %s", calSource.Data(), hist.Data()));
					calib->Draw("A*");
					calib->Fit("pol1");
					pol1 = calib->GetFunction("pol1");
					pol1->SetLineColor(2);

					Int_t low = -1;
					Int_t up = -1;
					for (Int_t i = LowerLim; i < UpperLim; i++) {
						if (h->GetBinContent(i) != 0) {
							if (low == -1) low = i;
							up = i;
						}
					}

					Double_t e = pol1->GetParameter(0) + pol1->GetParameter(1) * low;
					if (e < eMin) {
						xMin = low;
						eMin = e;
					}
					e = pol1->GetParameter(0) + pol1->GetParameter(1) * up;
					if (e > eMax) {
						xMax = up;
						eMax = e;
					}
				} else {
					OutputMessage(Form("Wrong number of peaks - %d (%s calibration needs at least %d peaks)", nPeaks, calSource.Data(), peaksNeeded), kTRUE);
					fitOk = "FALSE";
				}
			}

			canv->Update();
			gSystem->ProcessEvents();
		} else {
			canv->cd(1);
			h->Draw();
			ClearCanvas(2);
			OutputMessage(Form("No peaks found in histogram - %s", hist.Data()), kTRUE, kColorRed);
			gSystem->ProcessEvents();
			fitOk = "FALSE";
		}

		res->SetValue(Form("Calib.%s.Xmin", hist.Data()), LowerLim);
		res->SetValue(Form("Calib.%s.Xmax", hist.Data()), UpperLim);

		res->SetValue(Form("Calib.%s.NofPeaks", hist.Data()), nPeaks);
		for (Int_t i = 0; i < nPeaks; i++) {
			res->SetValue(Form("Calib.%s.Peak.%d.X", hist.Data(), i), px[i]);
			res->SetValue(Form("Calib.%s.Peak.%d.Xfit", hist.Data(), i), fx[i]);
			res->SetValue(Form("Calib.%s.Peak.%d.Xerr", hist.Data(), i), fxe[i]);
			res->SetValue(Form("Calib.%s.Peak.%d.Y", hist.Data(), i), py[i]);
			res->SetValue(Form("Calib.%s.Peak.%d.Yfit", hist.Data(), i), fy[i]);
			res->SetValue(Form("Calib.%s.Peak.%d.Yerr", hist.Data(), i), fye[i]);
			res->SetValue(Form("Calib.%s.Peak.%d.Chi2", hist.Data(), i), chi2[i]);
		}

		if (nPeaks >= peaksNeeded) {
			TString sts;
			if (doCal) {
				sts = Form("File %s, histo %s: E(x) = %5.2f + %5.2f * x", hFileName.Data(), hist.Data(), pol1->GetParameter(0),pol1->GetParameter(1));
			} else {
				sts = Form("File %s, histo %s: %d peak(s)", hFileName.Data(), hist.Data(), nPeaks);
			}
			OutputMessage(sts.Data(), kFALSE, 3);
		}

		WaitForButtonPressed();
		if (buttonFlag != 0) {
			if (buttonFlag == kButtonStop) break;
			if (buttonFlag != kButtonOk) continue;
			if (doCal && (nPeaks >= peaksNeeded)) {
				fitOk = "TRUE";
				cal->SetValue(Form("Calib.%s.Xmin", hist.Data()), LowerLim);
				cal->SetValue(Form("Calib.%s.Xmax", hist.Data()), UpperLim);
				cal->SetValue(Form("Calib.%s.Gain", hist.Data()), pol1->GetParameter(1));
				cal->SetValue(Form("Calib.%s.Offset", hist.Data()), pol1->GetParameter(0));
				res->SetValue(Form("Calib.%s.Gain", hist.Data()), pol1->GetParameter(1));
				res->SetValue(Form("Calib.%s.Offset", hist.Data()), pol1->GetParameter(0));
			} else {
				fitOk = "FALSE";
			}
		} else {
			if (doCal && (nPeaks >= peaksNeeded)) {
				cal->SetValue(Form("Calib.%s.Xmin", hist.Data()), LowerLim);
				cal->SetValue(Form("Calib.%s.Xmax", hist.Data()), UpperLim);
				cal->SetValue(Form("Calib.%s.Gain", hist.Data()), pol1->GetParameter(1));
				cal->SetValue(Form("Calib.%s.Offset", hist.Data()), pol1->GetParameter(0));
				res->SetValue(Form("Calib.%s.Gain", hist.Data()), pol1->GetParameter(1));
				res->SetValue(Form("Calib.%s.Offset", hist.Data()), pol1->GetParameter(0));
				fitOk = "AUTO";
			} else {
				fitOk = "FALSE";
			}
		}
		res->SetValue(Form("Calib.%s.FitOk", hist.Data()), fitOk);
		nofHistosCalibrated++;
	}

	res->SetValue("Calib.Emin", eMin);
	res->SetValue("Calib.Emax", eMax);
	res->SetValue("Calib.Xmin", xMin);
	res->SetValue("Calib.Xmax", xMax);

	CloseResultFiles();

	if (doCal && show2dim) ShowResults2dim(hFile, xMin, xMax, eMin, eMax);

	OutputMessage(Form("End of calibration - %d histogram(s), press \"execute\" to re-calibrate or \"quit\" to exit", nofHistosCalibrated), kFALSE, kColorBlue);

	if (resFit) resFit->Write();

	return;
}
